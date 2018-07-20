.text
.globl start
start:
jmp multiboot_entry
.align 4
multiboot_header:
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002+0x00000003)
.set MEMINFO, 1 << 1
multiboot_entry:
movl %ebx,0x100000
movl $(stack + 0x4000), %esp
call LynxKernel_Main
loop: hlt
jmp loop
.section ".bss"
.comm stack,0x4000
