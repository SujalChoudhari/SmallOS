org 0x7C00 ; set offset of rest of the instructions
bits 16 ; backwords compatibility directive, emmits 32 bit code.


main:
	hlt

.halt:
	jmp .halt

times 510 - ($-$$) db 0 

; $-$$ gives out the program size in bytes  (declare bytes 0 ),
; we pad out 510 bytes with zeros and remaining 2 will be our signature
; $ is a sepcial symbol that is equal to the memory offset of current line,
; $ is  "		"		"		"		"		"  offset of the beginning of the current section (our program)

dw 0AA55h ; declare word, (2 bytes) , signature what bios looks for.
