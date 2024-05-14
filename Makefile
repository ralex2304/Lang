SHELL = /bin/bash
.SHELLFLAGS = -o pipefail -c

DOCS_DIR = docs

CFLAGS_SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,$\
				   float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
				   object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
				   undefined,unreachable,vla-bound,vptr

.PHONY: main frontend middleend backend ir_backend asm build_front build_middle build_back build_ir_back clean_front clean_middle clean_back clean_ir_back

main: build frontend middleend backend ir_backend run

PROG_PATH = Programs/$(prog)

IOLIB_PATH = Programs/doubleiolib

frontend:
	@./frontend/./main -i $(PROG_PATH)/prog.snb -o $(PROG_PATH)/prog.tre 2>&1 | iconv -f cp1251 -t utf8 || (echo "Error raised by frontend"; exit 1)

middleend:
	@./middleend/./main -i $(PROG_PATH)/prog.tre -o $(PROG_PATH)/prog.treopt 2>&1 | iconv -f cp1251 -t utf8 || (echo "Error raised by middleend"; exit 1)

backend:
	@./backend/./main -i $(PROG_PATH)/prog.treopt -o $(PROG_PATH)/prog.ir 2>&1 | iconv -f cp1251 -t utf8 || (echo "Error raised by backend"; exit 1)

ir_backend: $(IOLIB_PATH).o
	@./ir_backend/./main -i $(PROG_PATH)/prog.ir -S $(PROG_PATH)/prog.nasm -o $(PROG_PATH)/prog -m x86_64 2>&1 | iconv -f cp1251 -t utf8 || (echo "Error raised by ir_backend"; exit 1)

run: asm
	@./$(PROG_PATH)/prog

asm:
	@nasm -f elf64 -g -F dwarf -l $(PROG_PATH)/prog.lst -I Programs/ $(PROG_PATH)/prog.nasm -o $(PROG_PATH)/lstprog.o
	@ld -g -no-pie -o $(PROG_PATH)/lstprog $(PROG_PATH)/lstprog.o
#@g++ $(CFLAGS_SANITIZER) -g -F dwarf -no-pie -o $(PROG_PATH)/prog $(PROG_PATH)/prog.o

$(IOLIB_PATH).o: $(IOLIB_PATH).nasm
	@nasm -f elf64 $(IOLIB_PATH).nasm -o $(IOLIB_PATH).o

build: build_front build_middle build_back build_ir_back

build_front:
	@cd ./frontend && make $(if $(release), release=1) $(if $(nobear), nobear=1)

build_middle:
	@cd ./middleend && make $(if $(release), release=1) $(if $(nobear), nobear=1)

build_back:
	@cd ./backend && make $(if $(release), release=1) $(if $(nobear), nobear=1)

build_ir_back:
	@cd ./ir_backend && make $(if $(release), release=1) $(if $(nobear), nobear=1)

clean_front:
	@cd ./frontend && make clean

clean_middle:
	@cd ./middleend && make clean

clean_back:
	@cd ./backend && make clean

clean_ir_back:
	@cd ./ir_backend && make clean

.PHONY: doxygen dox

DOCS_TARGET = $(DOCS_DIR)/docs_generated

doxygen dox: | $(DOCS_DIR)
	@doxygen docs/Doxyfile

$(DOCS_DIR):
	@mkdir ./$@

clean: clean_front clean_middle clean_back clean_ir_back
	@rm -rf ./$(DOCS_TARGET)
	@rm -rf ./compile_commands.json

