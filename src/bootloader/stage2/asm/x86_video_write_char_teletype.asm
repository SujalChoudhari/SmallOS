bits 16

section _TEXT class=CODE

; void _cdecl x86_Video_WriteCharTeletype(char c, u8 page)
global _x86_video_write_char_teletype
_x86_video_write_char_teletype:

    ; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    ; save bx
    push bx

    ; [bp + 0] - old call frame
    ; [bp + 2] - return address (small memory model => 2 bytes)
    ; [bp + 4] - first argument (character)
    ; [bp + 6] - second argument (page)
    ; note: bytes are converted to words (you can't push a single byte on the stack)
    mov ah, 0Eh
    mov al, [bp + 4]
    mov bh, [bp + 6]

    int 10h

    ; restore bx
    pop bx

    ; restore old call frame
    mov sp, bp
    pop bp
    ret
