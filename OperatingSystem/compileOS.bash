#!/bin/bash

nasm bootload.asm
source ../PathSetter.bash

# This section of the bash file is where we compile
# and rename set the name of the compiled c files.
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o uprog1.o uprog1.c
bcc -ansi -c -o uprog2.o uprog2.c
bcc -ansi -c -o uprog3.o uprog3.c
bcc -ansi -c -o userlib.o userlib.c
bcc -ansi -c -o shell.o shell.c

bcc -ansi -c -o proc.o proc.c



# as86 assembles the kernel, and lib  files, and
# renames them accordingly.
as86 kernel.asm -o kernel_asm.o
as86 lib.asm -o lib.o

# ld86 is the linker for the operating system.
# We are linking the compiled c files and the
# assembled libraries. (Kernel_asm.o or lib.o)
ld86 -o uprog1 -d uprog1.o lib.o
ld86 -o uprog2 -d uprog2.o lib.o
ld86 -o uprog3 -d uprog3.o lib.o

ld86 -o shell -d shell.o userlib.o lib.o

ld86 -o kernel -d kernel.o proc.o kernel_asm.o


dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc seek=0
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 conv=notrunc seek=30


# In the following part of the bash file
# we load the user programs into the loadfile
# unix executable file in the project directory.
./loadFile shell

./loadFile message.txt
./loadFile uprog1
./loadFile uprog2
./loadFile uprog3

# At this point we run the file using
# the following command.
bochs -f opsys.bxrc