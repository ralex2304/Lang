sanitizer = 1

DOCS_DIR = docs

CFLAGS_SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,$\
				   float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
				   object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
				   undefined,unreachable,vla-bound,vptr

.PHONY: main frontend middleend backend asm build_front build_middle build_back clean_front clean_middle clean_back

main: build frontend middleend backend asm

PROG_PATH = Programs/$(prog)

frontend:
	@LANG=ru_RU.CP1251 luit ./frontend/./main -i $(PROG_PATH)/prog.snb -o $(PROG_PATH)/prog.tre

middleend:
	@LANG=ru_RU.CP1251 luit ./middleend/./main -i $(PROG_PATH)/prog.tre -o $(PROG_PATH)/prog.treopt

backend:
	@LANG=ru_RU.CP1251 luit ./backend/./main -i $(PROG_PATH)/prog.treopt -o $(PROG_PATH)/prog.nasm -a x86_64

asm:
	@nasm -f elf64 -g -F dwarf -l $(PROG_PATH)/prog.lst $(PROG_PATH)/prog.nasm -o $(PROG_PATH)/prog.o
	@g++ $(CFLAGS_SANITIZER) -g -F dwarf -no-pie -o $(PROG_PATH)/prog $(PROG_PATH)/prog.o
	@./$(PROG_PATH)/prog



build: build_front build_middle build_back

build_front:
	@cd ./frontend && make $(if $(sanitizer), sanitizer=1)

build_middle:
	@cd ./middleend && make $(if $(sanitizer), sanitizer=1)

build_back:
	@cd ./backend && make $(if $(sanitizer), sanitizer=1)

clean_front:
	@cd ./frontend && make clean

clean_middle:
	@cd ./middleend && make clean

clean_back:
	@cd ./backend && make clean

.PHONY: doxygen dox

DOCS_TARGET = $(DOCS_DIR)/docs_generated

doxygen dox: | $(DOCS_DIR)
	@doxygen docs/Doxyfile

$(DOCS_DIR):
	@mkdir ./$@

clean: clean_front clean_back clean_middle
	@rm -rf ./$(DOCS_TARGET)

