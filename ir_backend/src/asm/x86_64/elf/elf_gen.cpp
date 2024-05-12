#include "elf_gen.h"

#define OBJ_CHECK_(clause_, err_fmt_, ...)                                                      \
            do {                                                                                \
                if (!(clause_)) {                                                               \
                    fprintf(stderr, "IOLib obj file error: " err_fmt_ "\n", ## __VA_ARGS__);    \
                    return Status::INPUT_ERROR;                                                 \
                }                                                                               \
            } while (0)

Status::Statuses IOLibObj::ctor(const char* obj_filename) {
    assert(obj_filename);

    STATUS_CHECK(file_open_read_close(obj_filename, (char**)&buf));

    Elf64_Ehdr* header = (Elf64_Ehdr*)buf;

    OBJ_CHECK_(memcmp(header->e_ident + EI_MAG0, ELFMAG, SELFMAG) == 0,     "invalid ELF header ident");
    OBJ_CHECK_(header->e_ident[EI_CLASS]      == ELFCLASS64,                "must be 64 bit");
    OBJ_CHECK_(header->e_ident[EI_VERSION]    == EV_CURRENT,                "invalid version");
    OBJ_CHECK_(header->e_ident[EI_DATA]       == ELFDATA2LSB,               "must be little-endian");
    OBJ_CHECK_(header->e_ident[EI_OSABI]      == ELFOSABI_SYSV,             "OSABI version must be SYSV");
    OBJ_CHECK_(header->e_ident[EI_ABIVERSION] == ELFOSABI_SYSV,             "ABI version must be SYSV");

    OBJ_CHECK_(header->e_type                 == ET_REL,                    "must be relocatable obj file");
    OBJ_CHECK_(header->e_machine              == EM_X86_64,                 "must be for x86-64");
    OBJ_CHECK_(header->e_version              == EV_CURRENT,                "invalid version");
    OBJ_CHECK_(header->e_ehsize               == sizeof(Elf64_Ehdr),        "invalid ELF header size");

    OBJ_CHECK_(header->e_shoff                == sizeof(Elf64_Ehdr),        "invalid ELF header size");
    OBJ_CHECK_(header->e_ehsize               == sizeof(Elf64_Shdr),        "invalid section header size");
    OBJ_CHECK_(header->e_shstrndx             != SHN_UNDEF,                 "must have string table");

    s_hdrs_arr = (Elf64_Shdr*)((uintptr_t)buf + header->e_shoff);

    Elf64_Shdr* s_strhdr = s_hdrs_arr + header->e_shstrndx;

    const char* str_table = (const char*)buf + s_strhdr->sh_offset;

    for (size_t i = 0; i < header->e_shnum; i++) {
        Elf64_Shdr hdr = s_hdrs_arr[i];

        if      (hdr.sh_type == SHT_PROGBITS && strcmp(".rodata",    str_table + hdr.sh_name) == 0)
            s_rodata_h = s_hdrs_arr + i;

        else if (hdr.sh_type == SHT_PROGBITS && strcmp(".data",      str_table + hdr.sh_name) == 0)
            s_data_h = s_hdrs_arr + i;

        else if (hdr.sh_type == SHT_PROGBITS && strcmp(".text",      str_table + hdr.sh_name) == 0)
            s_text_h = s_hdrs_arr + i;

        else if (hdr.sh_type == SHT_RELA     && strcmp(".rela.text", str_table + hdr.sh_name) == 0)
            s_rela_h = s_hdrs_arr + i;

        else if (hdr.sh_type == SHT_SYMTAB)
            s_symtab_h = s_hdrs_arr + i;

        else if (hdr.sh_type == SHT_STRTAB)
            s_strtab_h = s_hdrs_arr + i;
    }

    OBJ_CHECK_(s_rodata_h != nullptr, ".rodata segment not found");
    OBJ_CHECK_(s_data_h   != nullptr, ".data segment not found");
    OBJ_CHECK_(s_text_h   != nullptr, ".text segment not found");
    OBJ_CHECK_(s_rela_h   != nullptr, ".rela_text segment not found");
    OBJ_CHECK_(s_symtab_h != nullptr, ".symtab segment not found");

    STATUS_CHECK(get_symbol_offset("doubleio_in",  &funcs_offs.in));
    STATUS_CHECK(get_symbol_offset("doubleio_out", &funcs_offs.out));

    return Status::NORMAL_WORK;
}

Status::Statuses IOLibObj::get_symbol_offset(const char* name, size_t* offs) {
    assert(name);
    assert(offs);

    const char* strs = (char*)buf + s_strtab_h->sh_offset;

    for (size_t i = 0; i < s_symtab_h->sh_size / s_symtab_h->sh_entsize; i++) {
        Elf64_Sym sym = ((Elf64_Sym*)((uintptr_t)buf + s_symtab_h->sh_offset))[i];

        if (strcmp(strs + sym.st_name, name) == 0) {
            *offs = sym.st_value;
            return Status::NORMAL_WORK;
        }
    }

    fprintf(stderr, "IOLib obj file error: symbol \"%s\" not found\n", name);
    return Status::INPUT_ERROR;
}

Status::Statuses IOLibObj::fixup_rela(const Elf64_Phdr* rodata, const Elf64_Phdr* data) {
    assert(rodata);
    assert(data);

    Elf64_Rela* relas = (Elf64_Rela*)((uintptr_t)buf + s_rela_h->sh_offset);

    assert(s_rela_h->sh_entsize == sizeof(Elf64_Rela));
    for (size_t i = 0; i < s_rela_h->sh_size / s_rela_h->sh_entsize; i++) {
        Elf64_Rela rela = relas[i];

        size_t sym_i = ELF64_R_SYM(rela.r_info);
        Elf64_Sym sym = ((Elf64_Sym*)((uintptr_t)buf + s_symtab_h->sh_offset))[sym_i];

        size_t segm_vmem_begin = 0;

        if      (s_hdrs_arr + sym.st_shndx == s_rodata_h) segm_vmem_begin = rodata->p_vaddr;
        else if (s_hdrs_arr + sym.st_shndx == s_data_h)   segm_vmem_begin = data->p_vaddr;
        else {
            fprintf(stderr, "IOLib obj file error: unsoppoerted segment for fixup");
            return Status::NORMAL_WORK;
        }

        char* dest = (char*)buf + s_text_h->sh_offset + rela.r_offset;

        if (ELF64_R_TYPE(rela.r_info) == R_X86_64_32S) {
            uint32_t val32 = (uint32_t)(segm_vmem_begin + (size_t)rela.r_addend);

            memcpy(dest, &val32, sizeof(val32));

        } else if (ELF64_R_TYPE(rela.r_info) == R_X86_64_64) {
            uint64_t val64 = (uint64_t)(segm_vmem_begin + (size_t)rela.r_addend);

            memcpy(dest, &val64, sizeof(val64));

        } else {
            fprintf(stderr, "IOLib obj file error: unknown relocation type\n");
            return Status::NORMAL_WORK;
        }
    }

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::ctor(const char* iolib_obj_filename) {
    assert(iolib_obj_filename);

    if (!buf.ctor(sizeof(char))) {
        buf.dtor();
        return Status::MEMORY_EXCEED;
    }

    STATUS_CHECK(iolib.ctor(iolib_obj_filename),    buf.dtor(); iolib.dtor());

    STATUS_CHECK(init_elf_hdr_and_phdrs_(),         buf.dtor(); iolib.dtor());

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::dtor() {
    buf.dtor();
    return iolib.dtor();
}

Status::Statuses ElfData::init_elf_hdr_and_phdrs_() {

    if (!buf.push_zero_elems(sizeof(Elf64_Ehdr))) return Status::MEMORY_EXCEED;

    Elf64_Ehdr* ehdr = header();

    memcpy(ehdr->e_ident + EI_MAG0, ELFMAG, SELFMAG);
    ehdr->e_ident[EI_CLASS]       = ELFCLASS64;
    ehdr->e_ident[EI_VERSION]     = EV_CURRENT;
    ehdr->e_ident[EI_DATA]        = ELFDATA2LSB;
    ehdr->e_ident[EI_OSABI]       = ELFOSABI_SYSV;
    ehdr->e_ident[EI_ABIVERSION]  = ELFOSABI_SYSV;

    ehdr->e_type                  = ET_EXEC;
    ehdr->e_machine               = EM_X86_64;
    ehdr->e_version               = EV_CURRENT;
    ehdr->e_entry                 = 0;
    ehdr->e_phoff                 = sizeof(Elf64_Ehdr);
    ehdr->e_shoff                 = 0;         //< no section headers
    ehdr->e_ehsize                = sizeof(Elf64_Ehdr);
    ehdr->e_phentsize             = sizeof(Elf64_Phdr);
    ehdr->e_phnum                 = ElfData::PHDR_NUM;
    ehdr->e_shentsize             = sizeof(Elf64_Shdr);
    ehdr->e_shnum                 = 0;         //< no section headers
    ehdr->e_shstrndx              = SHN_UNDEF; //< no str section

    STATUS_CHECK(init_elf_phdrs_());

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::init_elf_phdrs_() {

    Elf64_Phdr phdr = {};
    if (!buf.push_zero_elems(sizeof(Elf64_Phdr) * ElfData::PHDR_NUM))
        return Status::MEMORY_EXCEED;

    // info phdr
    phdr.p_type = PT_LOAD;
    phdr.p_flags = PF_R;
    phdr.p_offset = 0;
    phdr.p_vaddr  = ElfData::VMEM_START;
    phdr.p_paddr  = ElfData::VMEM_START;
    phdr.p_filesz = (Elf64_Xword)buf.size();
    phdr.p_memsz  = (Elf64_Xword)buf.size();
    phdr.p_align  = ElfData::PAGE_SIZE;

    memcpy(header() + 1, &phdr, sizeof(Elf64_Phdr));

    // .rodata phdr
    phdr.p_type   = PT_LOAD;
    phdr.p_offset = 0;
    phdr.p_vaddr  = 0;
    phdr.p_paddr  = 0;
    phdr.p_filesz = 0;
    phdr.p_memsz  = 0;
    phdr.p_align  = ElfData::PAGE_SIZE;

    phdr.p_flags  = PF_R;
    memcpy(phdr_rodata(), &phdr, sizeof(Elf64_Phdr));

    // .data phdr
    phdr.p_flags = PF_R | PF_W;
    memcpy(phdr_data(), &phdr, sizeof(Elf64_Phdr));

    // .text phdr
    phdr.p_flags = PF_R | PF_X;
    memcpy(phdr_text(), &phdr, sizeof(Elf64_Phdr));

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::write_to_file(const char* elf_filename) {
    assert(elf_filename);

    STATUS_CHECK(file_open_write_bytes_close(elf_filename, (char*)buf[0], (size_t)buf.size()));

    if (chmod(elf_filename, S_IREAD | S_IWRITE | S_IEXEC) != 0) {
        fprintf(stderr, "Setting ELF file permissions failed\n");
        return Status::FILE_ERROR;
    }

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::start_rodata() {
    if (!buf.align(phdr_rodata()->p_align))
        return Status::MEMORY_EXCEED;

    phdr_rodata()->p_offset = (size_t)buf.size();
    phdr_rodata()->p_vaddr  = count_vmem_alignment(phdr_rodata()[-1].p_vaddr + phdr_rodata()[-1].p_memsz,
                                                   phdr_rodata()->p_align);
    phdr_rodata()->p_paddr  = phdr_rodata()->p_vaddr;

    // iolib .rodata insert
    if (!buf.push_back_several((char*)iolib.buf + iolib.s_rodata_h->sh_offset, iolib.s_rodata_h->sh_size))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::start_data() {
    phdr_data()->p_offset = (size_t)buf.size();
    phdr_data()->p_vaddr  = count_vmem_alignment(phdr_data()[-1].p_vaddr + phdr_data()[-1].p_memsz,
                                                 phdr_data()->p_align)
                          + phdr_data()->p_offset % phdr_data()->p_align;

    phdr_data()->p_paddr  = phdr_data()->p_vaddr;

    // iolib .data insert
    if (!buf.push_back_several((char*)iolib.buf + iolib.s_data_h->sh_offset, iolib.s_data_h->sh_size))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ElfData::start_text() {
    if (!buf.align(phdr_text()->p_align))
        return Status::MEMORY_EXCEED;

    phdr_text()->p_offset = (size_t)buf.size();
    phdr_text()->p_vaddr  = count_vmem_alignment(phdr_text()[-1].p_vaddr + phdr_text()[-1].p_memsz,
                                                 phdr_text()->p_align);
    phdr_text()->p_paddr  = phdr_text()->p_vaddr;

    STATUS_CHECK(iolib.fixup_rela(phdr_rodata(), phdr_data()));

    // iolib .text insert
    if (!buf.push_back_several((char*)iolib.buf + iolib.s_text_h->sh_offset, iolib.s_text_h->sh_size))
        return Status::MEMORY_EXCEED;

    header()->e_entry = phdr_text()->p_vaddr + ((size_t)buf.size() - phdr_text()->p_offset);

    return Status::NORMAL_WORK;
}
