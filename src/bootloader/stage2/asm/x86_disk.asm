bits 16

section _TEXT class=CODE

; bool _cdecl x86_disk_reset(u8 drive);
global _x86_disk_reset;
_x86_disk_reset:
; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    mov ah, 0
    mov dl,[bp + 4]     ; dl = drive number to reset
    stc
    int 13h

    mov ax, 1
    sbb ax, 0           ; 1 = true, 0= false

; restore old call frame
    mov sp, bp
    pop bp
    ret

; bool _cdecl x86_disk_read(u8 drive, u16 cylinder, u16 head, u16 sector, u8 count, u8 far* out_data);
global _x86_disk_read:
_x86_disk_read:
; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    push bx
    push es

    ; setup args
    mov dl,[bp + 4]     ; dl = drive number to reset
    mov ch, [bp + 6]    ; ch = cylinder (lower 8 bits)
    mov cl, [bp + 7]    ; ch = cylinder (upper 8 bits)
    shl cl, 6

    mov dh, [bp + 10]    ; dh = head

    mov al, [bp+ 8]
    and al, 3Fh
    or cl, al           ; cl = sector to bits (0-5)

    mov al, [bp + 12]   ; al = count

    mov bx, [bp + 16]   ; es:bx far pointer to out_data
    mov es, bx
    mov bx, [bp + 14]

    ; call int13 ah:02h
    mov ah, 02h
    stc
    int 13h

    mov ax, 1
    sbb ax, 0           ; 1 = true, 0= false


    ; restore regs
    pop es
    pop bx

; restore old call frame
    mov sp, bp
    pop bp
    ret

; bool _cdecl x86_disk_get_drive_params(u8 drive, u8* out_drive_type, u8* out_cylinders,u16 out_sectors, u16 out_heads);
global _x86_disk_get_drive_params:
_x86_disk_get_drive_params:
; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    ; save regs
    push es
    push bx
    push si
    push di

    ; call int 13
    mov dl, [bp+ 4] ; dl = disk drive
    mov ah, 08h
    mov di, 0       ; es:di = 0000:000
    stc
    int 13h

    ; return
    mov ax, 1
    sbb ax, 0

    ; out params
    mov si, [bp + 6]    ; drive type
    mov [si], bl

    mov bl, ch          ; lower bits in ch
    mov bh, cl          ; upper bits in cl (6-7)
    shr bh, 6
    mov si, [bp + 8]
    mov [si], bx

    xor ch, ch          ; sectors lower 5 bits in cl
    and cl,3Fh
    mov [si], cx

    mov cl,dh
    mov si, [bp+12]
    mov [si],cx
    ; restore regs
    pop di
    pop si
    pop bx
    pop es

; restore old call frame
    mov sp, bp
    pop bp
    ret
