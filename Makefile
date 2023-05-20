SUBDIRS := $(wildcard sheet*)
TARGET=all

all: $(SUBDIRS)

clean: $(SUBDIRS)

ls:
	echo $(SUBDIRS)
	echo $(MAKECMDGOALS)

$(SUBDIRS):
	$(MAKE) --directory=$@ $(MAKECMDGOALS)


.PHONY: all $(SUBDIRS)