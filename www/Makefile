SUBDIRS += operatorPanel

.PHONY: subdirs $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@

install:

clean:
	@$(MAKE) -C operatorPanel clean
