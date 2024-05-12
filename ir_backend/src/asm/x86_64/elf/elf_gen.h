#ifndef ELF_GEN_H_
#define ELF_GEN_H_

#include <elf.h>
#include <sys/stat.h>

#include "utils/vector.h"
#include "utils/statuses.h"
#include "file/file.h"

struct IOLibObj {
    void* buf = nullptr;

    Elf64_Shdr* s_hdrs_arr = nullptr;

    Elf64_Shdr* s_rodata_h = nullptr;
    Elf64_Shdr* s_data_h   = nullptr;
    Elf64_Shdr* s_text_h   = nullptr;
    Elf64_Shdr* s_rela_h   = nullptr;
    Elf64_Shdr* s_symtab_h = nullptr;
    Elf64_Shdr* s_strtab_h = nullptr;

    struct Functions {
        size_t in  = 0;
        size_t out = 0;
    } funcs_offs;

    Status::Statuses ctor(const char* obj_filename);

    Status::Statuses dtor() {
        FREE(buf);
        s_hdrs_arr = nullptr;
        s_rodata_h = nullptr;
        s_data_h   = nullptr;
        s_text_h   = nullptr;
        s_rela_h   = nullptr;
        s_strtab_h = nullptr;
        return Status::NORMAL_WORK;
    };

    Status::Statuses fixup_rela(const Elf64_Phdr* rodata, const Elf64_Phdr* data);

    Status::Statuses get_symbol_offset(const char* name, size_t* offs);
};

struct ElfData {
  public:

    static const size_t PAGE_SIZE  = 0x1000; //< 4KB
    static const size_t VMEM_START = 0x400000;

    Vector buf = {};

    Elf64_Ehdr* header()      { return (Elf64_Ehdr*)buf[0]; };
    Elf64_Phdr* phdr_rodata() { return (Elf64_Phdr*)buf[sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) * 1]; };
    Elf64_Phdr* phdr_data()   { return (Elf64_Phdr*)buf[sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) * 2]; };
    Elf64_Phdr* phdr_text()   { return (Elf64_Phdr*)buf[sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) * 3]; };

    static const size_t PHDR_NUM = 4; //< the first phdr is for info + 3 phdrs

    IOLibObj iolib = {};

    struct {
        size_t eps_vaddr = 0;
        size_t double_neg_const_vaddr = 0;
        size_t global_section_vaddr = 0;
    } globals;

    Status::Statuses ctor(const char* iolib_obj_filename);

    Status::Statuses dtor();

    Status::Statuses write_to_file(const char* elf_filename);

    Status::Statuses start_rodata();

    Status::Statuses start_data();

    Status::Statuses start_text();

    void end_segment(Elf64_Phdr* phdr) {
        phdr->p_filesz = (size_t)buf.size() - phdr->p_offset;
        phdr->p_memsz = phdr->p_filesz;
    };

    size_t count_vmem_alignment(size_t min_addr, size_t alignment) {
        if (min_addr % alignment == 0)
            return min_addr;

        return min_addr + alignment - (min_addr % alignment);
    };

  private:
    Status::Statuses init_elf_phdrs_();

    Status::Statuses init_elf_hdr_and_phdrs_();

};

#endif //< #ifndef ELF_GEN_H
