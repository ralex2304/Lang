
DOCS_DIR = docs


.PHONY: frontend middleend backend build_front build_middle build_back

build: build_front build_middle build_back

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
