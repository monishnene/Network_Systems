	common_sources=	dfs.c \
			dfs_support.c
	
	common_includes = dfs_support.h 

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
			dfs.elf 

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

build:	dfs.elf	

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

run:
	./dfs.elf 8160 &
	./dfs.elf 8161 &
	./dfs.elf 8162 &
	./dfs.elf 8163 &

clean :
	rm	-f	*.o	*.s	*.i	*.o	*.d	*.elf *.swp
