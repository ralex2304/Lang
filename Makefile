sanitizer = 1

DOCS_DIR = docs


.PHONY: main frontend middleend backend build_front build_middle build_back clean_front clean_middle clean_back

main: build frontend middleend backend

PROG_PATH = Programs/$(prog)

frontend:
	@LANG=ru_RU.CP1251 luit ./frontend/./main -i $(PROG_PATH)/prog.snb -o $(PROG_PATH)/prog.tre

middleend:
	@LANG=ru_RU.CP1251 luit ./middleend/./main -i $(PROG_PATH)/prog.tre -o $(PROG_PATH)/prog.treopt

backend:
	@LANG=ru_RU.CP1251 luit ./backend/./main -i $(PROG_PATH)/prog.treopt -o $(PROG_PATH)/prog.code


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

