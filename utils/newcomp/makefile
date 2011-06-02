DESTDIR =
prefix  = /usr
DIR     = $(DESTDIR)$(prefix)/bin

# EXAMPLE_COMPS += SkeletonFilter

EXAMPLE_COMPS += Skeleton
EXAMPLE_COMPS += SkeletonSource
EXAMPLE_COMPS += SkeletonSink

all: newcomp

newcomp:
	@perl create_newcomp $(EXAMPLE_COMPS) > $@
	@chmod +x $@

install:
	@mkdir -p $(DIR)
	@install -m 755 newcomp $(DIR)

uninstall:
	@rm -f $(DIR)/newcomp

clean:
	@rm -f newcomp
