bits 16

section _TEXT class=CODE

; void _cdecl x86_div64_32(u64 divident,u32 divisor, u64* out_quotient, u32* out_remainder)
global _x86_div64_32
_x86_div64_32:
; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    push bx

    ; divide upper 32 bits
    mov eax, [bp+8] ; eax = upper 32 bits of divident
    mov ecx, [bp+12]; ecx = divisor
    xor edx, edx
    div ecx  ; eax = quotient, edx remainder

    ; store upper 32 bits in quotient
    mov bx, [bp +16]
    mov [bx + 4], eax

    ; divide the lower 2 bits
    mov eax, [bp+ 4]    ;  eax =  lower 32 bits, edx old reminder
    div ecx

    mov [bx], eax
    mov bx, [bp + 18]
    mov [bx], edx

    pop bx

; restore old call frame
    mov sp, bp
    pop bp
    ret
