section .text
global _start_
extern _cstart

_start_:
    cli
    mov ax, ds
    mov es, ax
    mov sp, 0
    mov bp, sp
    sti

    ; Pass boot drive in dl to _cstart
    xor dh, dh
    push dx
    call _cstart

    cli
    hlt
