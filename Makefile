
DOCS_DIR = docs


.PHONY: main frontend middleend backend build_front build_middle build_back

main: build frontend backend

PROG_PATH = Programs/$(prog)

frontend:
	@LANG=ru_RU.CP1251 luit ./frontend/./main -i $(PROG_PATH)/prog.snb -o $(PROG_PATH)/prog.tre

middleend:
	@LANG=ru_RU.CP1251 luit ./middleend/./main

backend:
	@LANG=ru_RU.CP1251 luit ./backend/./main -i $(PROG_PATH)/prog.tre -o $(PROG_PATH)/prog.code


build: build_front build_back

build_front:
	@cd ./frontend && make

build_middle:
	@cd ./middleend && make

build_back:
	@cd ./backend && make

.PHONY: doxygen dox

DOCS_TARGET = $(DOCS_DIR)/docs_generated

doxygen dox: | $(DOCS_DIR)
	@doxygen docs/Doxyfile

$(DOCS_DIR):
	@mkdir ./$@

clean:
	@rm -rf ./$(DOCS_TARGET)
