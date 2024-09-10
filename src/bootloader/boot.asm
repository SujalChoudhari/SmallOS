org 0x7C00         ; Set the origin to 0x7C00, the standard start address for bootloaders
bits 16            ; Set the processor to operate in 16-bit mode for compatibility with legacy BIOS

; Define a constant for the newline sequence (carriage return + line feed)
%define ENDL 0x0D, 0x0A 

; FAT12 Boot Sector Header
jmp short start    ; Jump to the start label to begin execution
nop                ; No operation (placeholder)

; BIOS Parameter Block (BPB) - Contains essential file system parameters
bdb_oem:                    db 'MSWIN4.1'           ; OEM identifier (8 bytes) 
bdb_bytes_per_sector:       dw 512                  ; Bytes per sector (2 bytes)
bdb_sectors_per_cluster:    db 1                    ; Sectors per cluster (1 byte)
bdb_reserved_sectors:       dw 1                    ; Number of reserved sectors (2 bytes)
bdb_fat_count:              db 2                    ; Number of File Allocation Tables (FATs) (1 byte)
bdb_dir_entries_count:      dw 0E0h                 ; Number of root directory entries (2 bytes)
bdb_total_sectors:          dw 2880                 ; Total number of sectors (2 bytes), 2880 sectors * 512 bytes = 1.44MB
bdb_media_descriptor_type:  db 0F0h                 ; Media descriptor type (1 byte), 0xF0 for 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                    ; Sectors per FAT (2 bytes)
bdb_sectors_per_track:      dw 18                   ; Sectors per track (2 bytes)
bdb_heads:                  dw 2                    ; Number of heads (2 bytes)
bdb_hidden_sectors:         dd 0                    ; Number of hidden sectors (4 bytes)
bdb_large_sector_count:     dd 0                    ; Total number of sectors for FAT32 (4 bytes, not used for FAT12)

; Extended Boot Record (EBR) - Contains additional volume information
ebr_drive_number:           db 0                    ; Drive number (1 byte), 0x00 for floppy disk, 0x80 for hard disk
reserved_byte:              db 0                    ; Reserved byte (1 byte)
ebr_signature:              db 29h                  ; Extended Boot Record signature (1 byte)
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; Volume serial number (4 bytes), value is arbitrary
ebr_volume_label:           db 'SMALL OS   '        ; Volume label (11 bytes), padded with spaces
ebr_system_id:              db 'FAT12   '           ; File system type (8 bytes)

start:
    jmp main       ; Jump to the main routine to start execution

; Prints a null-terminated string to the screen
; Parameters:
; - DS:SI points to the string to be printed
puts:
    push si        ; Save the SI register (string pointer) on the stack
    push ax        ; Save the AX register (temporary data) on the stack

.loop:
    lodsb           ; Load byte at DS:SI into AL and increment SI
    or al, al       ; Check if the byte is null (0)
    jz .done        ; If zero, jump to the done label

    mov ah, 0x0E    ; Set AH to 0x0E for teletype output (print character)
    mov bh, 10      ; Set BH to page number (usually 0)
    int 0x10        ; Call BIOS interrupt 0x10 for teletype output
    jmp .loop       ; Repeat for the next character

.done:
    pop ax          ; Restore the AX register
    pop si          ; Restore the SI register
    ret             ; Return from the subroutine

main:
    ; Initialize data segments
    mov ax, 0       ; Set AX to 0 for segment initialization
    mov ds, ax      ; Initialize DS segment register to 0
    mov es, ax      ; Initialize ES segment register to 0

    ; Initialize stack
    mov ss, ax      ; Set SS (stack segment) to 0
    mov sp, 0x7C00  ; Set SP (stack pointer) to the top of the boot sector

    ; Print "Hello World!" message
    mov si, msg_hello ; Load address of the "Hello World!" message into SI
    call puts       ; Call the puts subroutine to print the message

    hlt             ; Halt the CPU (end of execution)

.halt:
    jmp .halt       ; Infinite loop to keep the CPU halted

msg_hello: db "Hello World!", ENDL, 0 ; Define the "Hello World!" message with a newline and null terminator

; Pad the boot sector to 510 bytes with zeros
times 510 - ($ - $$) db 0 

; Boot signature (magic number) required by BIOS to identify the boot sector
dw 0xAA55        