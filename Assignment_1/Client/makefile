	common_sources=	udp_client.c \
			client_support.c
	
	common_includes = client_support.h 

	common_flags =	-g \
			-O0 \
			-lpthread\
			-pthread \
			-lrt \
			-lm \
			-lc
	
	ip =	128.138.201.66

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
	run \
	local

build:	udp_client.elf	

%.elf:	$(objects)	$(deps)	
	$(CC)	$(flags)	-o	$@	$(objects)

%.i:%.c		$(includes)
	$(CC)	$(flags)	-E	$<	-o	$@

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

run:
	./udp_client.elf	$(ip)	$(port_number)

local:
	./udp_client.elf	localhost	$(port_number)

clean :
	rm	-f	*.o	*.s	*.i	*.o	*.d	*.elf *.swp
