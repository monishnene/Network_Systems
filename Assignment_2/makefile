	common_sources=	udp_server.c \
			server_support.c
	
	common_includes = server_support.h 

	common_flags =	-g \
			-O0 \
			-lpthread\
			-pthread \
			-lrt \
			-lm \
			-lc

	upload_files =	makefile \
		  	udp_server.c \
			server_support.c \
			server_support.h \
			udp_server.elf  

	port_number = 6313

	CC=gcc
	flags =	$(common_flags)
	sources =	$(common_sources)
	includes =	$(common_includes)
	objects:= $(sources:.c=.o)
	deps:= $(sources:.c=.d)	

.PHONY:	compile-all \
	clean \
	build \
	upload \
	go \
	run 

build:	udp_server.elf	

%.elf:	$(objects)	$(deps)
	$(CC)	$(flags)	-o	$@	$(objects)	

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

go:
	ssh	mone9255@elra-01.cs.colorado.edu

run:
	./udp_server.elf	$(port_number)

local:
	./udp_server.elf	$(port_number)

clean :
	rm	-f	*.o	*.s	*.i	*.o	*.d	*.elf *.swp
