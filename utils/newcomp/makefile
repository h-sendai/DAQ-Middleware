prefix = /usr
DIR    = $(prefix)/bin

all: prepare newcomp

prepare:
	@set -e; for i in ../../examples/Skeleton/*; do cp $${i} $$(basename $${i}).in; done

newcomp: newcomp.template Makefile.in Skeleton.h.in Skeleton.cpp.in SkeletonComp.cpp.in
	@perl create_newcomp > $@
	@chmod +x $@

install:
	@mkdir -p $(DIR)
	@install -m 755 newcomp $(DIR)

uninstall:
	@rm -f $(DIR)/newcomp

clean:
	@rm -f *.in
	@rm -f newcomp
