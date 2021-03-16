org 0x7c00
BaseOfSatck equ 0x7c00

Label_Start:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BaseOfSatck


    ;clear screen
    mov ax, 0600h
    mov bx, 0700h
    mov cx, 0
    mov dx, 0184fh
    int 10h
    ;set focus
    mov ax, 0200h
    mov bx, 0000h
    mov dx, 0000h
    int 10h
    ;ptint "Start Boot"
    mov ax, 1301h                   ;ah=13h(print string), al=01h(set the attribute of string)
    mov bx, 000fh                   ;bh=00h, bl=0fh,
    mov dx, 0000h
    mov cx, 10
    push ax
    mov ax, ds
    mov es, ax
    pop ax
    mov bp, StartBootMessage
    int 10h                         ;int 10h video operation
    ;reset floppy
    xor ah, ah
    xor dl, dl
    int 13h                         ;int 13h disk operation

    jmp $


StartBootMessage: db "Start Boot"

;fill 0
times 510-($-$$) db 0
dw 0xaa55