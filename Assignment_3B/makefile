	common_sources=	proxy.c \
			proxy_support.c
	
	common_includes = proxy_support.h 

	common_flags =	-g \
			-O0 \
			-lpthread\
			-pthread \
			-lrt \
			-lm \
			-lc \

	port_number = 8000

	CC=gcc
	flags =	$(common_flags)
	sources =	$(common_sources)
	includes =	$(common_includes)
	objects:= $(sources:.c=.o)
	deps:= $(sources:.c=.d)	

.PHONY:	compile-all \
	clean \
	build \
	upload 

build:	proxy.elf	

%.elf:	$(objects)	$(deps)
	$(CC)	$(flags)	-o	$@	$(objects)	-lcrypto	-lssl

%.i:%.c		$(includes)
	$(CC)	$(flags)	-S	$<	-o	$@

%.asm:%.c	$(includes)
	$(CC)	$(flags)	-S	$<	-o	$@

%.o:%.c		$(includes)
	$(CC)	$(flags)	-c	$<	-o	$@

%.o:%.s		$(includes)
	$(CC)	$(flags)	-c	$<	-o	$@
	
%.d:%.c		$(includes)
	$(CC)	$(flags)	-M	$<	-o	$@	

%.d:%.s		$(includes)
	$(CC)	$(flags)	-M	$<	-o	$@

compile-all:	$(objects)

upload:
	scp	$(upload_files)	mone9255@elra-01.cs.colorado.edu:~/

run:
	./proxy.elf	$(port_number)

clean :
	rm	-f	*.o	*.s	*.i	*.o	*.d	*.elf *.swp
