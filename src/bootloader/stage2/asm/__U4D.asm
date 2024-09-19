bits 16

section _TEXT class=CODE


global __U4D
__U4D:
    shl edx, 16     ; dx to upper half of edx
    mov dx, ax      ; edx = dividend
    mov eax, edx    ; eax = dividend
    xor edx, edx

    shl ecx, 16     ; cx to upper half of ecx
    mov cx, bx      ; ecx = divisor

    div ecx         ; eax = quotient, edx = reminder
    mov ebx, edx
    mov ecx, edx
    shr ecx, 16

    mov edx, eax
    shr edx, 16

    ret
