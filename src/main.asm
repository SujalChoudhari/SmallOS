org 0x7C00         ; Set the offset for the start of the bootloader code
bits 16            ; Use 16-bit mode for compatibility with legacy BIOS

%define ENDL 0x0D, 0x0A ; Define a constant for the newline sequence (carriage return + line feed)

start:
    jmp main       ; Jump to the main routine to start execution

; Prints a null-terminated string to the screen
; Parameters:
; - DS:SI points to the string to be printed
puts:
    push si        ; Save SI register (string pointer) on stack
    push ax        ; Save AX register (temporary data) on stack

.loop:
    lodsb           ; Load byte at DS:SI into AL, increment SI
    or al, al       ; Check if the byte is null (0)
    jz .done        ; If zero, jump to done

    mov ah, 0x0E    ; Set AH to 0x0E for teletype output (print character)
    mov bh, 10      ; Set BH to page number (usually 0)
    int 0x10        ; Call BIOS interrupt for teletype output
    jmp .loop       ; Repeat for the next character

.done:
    pop ax          ; Restore AX register
    pop si          ; Restore SI register
    ret             ; Return from subroutine

main:
    ; Initialize data segments
    mov ax, 0       ; Set AX to 0 for segment initialization
    mov ds, ax      ; Initialize DS segment register
    mov es, ax      ; Initialize ES segment register

    ; Initialize stack
    mov ss, ax      ; Set SS to 0 for stack segment
    mov sp, 0x7C00  ; Set SP to the top of the stack

    ; Print "Hello World!" message
    mov si, msg_hello ; Load address of message into SI
    call puts       ; Call the puts subroutine to print the message

    hlt             ; Halt the CPU (end of execution)

.halt:
    jmp .halt       ; Infinite loop to halt the CPU

msg_hello: db "Hello World!", ENDL, 0 ; Define the "Hello World!" message followed by newline and null terminator

times 510 - ($ - $$) db 0 ; Pad the boot sector to 510 bytes with zeros

dw 0xAA55        ; Boot signature (magic number) required by BIOS to identify the boot sector

