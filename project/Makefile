
SUBDIRS= BasePlatform

define make_subdir
 @for subdir in $(SUBDIRS) ; do \
 ( cd $$subdir && 'pwd' && if [ -f Makefile ]; then make $@ || exit "$$?"; fi;) \
 done;
endef

all :
	$(call make_subdir , all)
 
install :
	$(call make_subdir , install)

clean:
	$(call make_subdir , clean)
