
bits 16

section _TEXT class=CODE
;========================================================================
;==     Name:           I4D                                            ==
;==     Operation:      Signed 4 byte divide                           ==
;==     Inputs:         DX;AX   Dividend                               ==
;==                     CX;BX   Divisor                                ==
;==     Outputs:        DX;AX   Quotient                               ==
;==                     CX;BX   Remainder (same sign as dividend)      ==
;==     Volatile:       none                                           ==
;========================================================================
global __I4D
__I4D:
    push bp
    mov bp, sp

    ; Combine DX:AX into a 32-bit dividend in EDX
    shl edx, 16
    mov dx, ax

    ; Combine CX:BX into a 32-bit divisor in ECX
    shl ecx, 16
    mov cx, bx

    ; Perform signed division
    mov eax, edx    ; Move dividend to EAX
    cdq             ; Sign-extend EAX into EDX:EAX
    idiv ecx        ; Perform signed division

    ; Store quotient in DX:AX
    mov dx, ax
    shr eax, 16
    mov ax, dx

    ; Store remainder in CX:BX
    mov cx, dx
    shr edx, 16
    mov bx, cx

    pop bp
    ret
