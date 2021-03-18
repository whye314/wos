TI_GDT equ 0 << 2
RPL0 equ 0 
SELECTOR_VIDEO equ (0x4 << 3) + TI_GDT + RPL0



;put_char
global put_char


put_char:
    ;put_char(char n)
    push ebp
    mov ebp, esp
    pushad
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    pushf
    cli


    ;get cursor high byte
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    in al, dx
    shl ax, 8

    ;get cursor low byte
    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    in al, dx

    mov bx, ax

    mov ecx, [ebp + 8]
    
    cmp cl, 0xd
    jz .is_cr
    cmp cl, 0xa
    jz .is_lf
    cmp cl, 0x8
    jz .is_bs

    jmp .is_other


    .is_bs:
        dec bx
        shl bx, 1
        mov word [gs:bx], 0x00
        shr bx, 1
        jmp .set_cursor

    .is_other:
        shl bx, 1
        mov ch, 0xf
        mov [gs:bx], cx
        shr bx, 1
        inc bx
        jmp .if_full

    .is_cr:
    .is_lf:
        mov ax, bx
        mov dx, 80
        div dl
        shr ax, 8
        sub bx, ax
        add bx, 80
    .if_full:
        cmp bx, 2000
        jl .set_cursor
    .full:
    .roll_screen:
        cld
        mov ecx, 960
        mov esi, 0xc00b80a0
        mov edi, 0xc00b8000
        rep movsd
        mov ebx, 3840

        mov ecx, 80
    .clear_last_line:
        
        mov word [gs:bx], 0x00
        add ebx, 2
        loop .clear_last_line
        mov ebx, 1920

    .set_cursor:

        mov dx, 0x3d4
        mov al, 0xe
        out dx, al
        mov dx, 0x3d5
        mov al, bh
        out dx, al

        mov dx, 0x3d4
        mov al, 0xf
        out dx, al
        mov dx, 0x3d5
        mov al, bl
        out dx, al

        



    popf
    popad
    mov esp, ebp
    pop ebp
    ret


global put_str
put_str:
    ;put_str(uint8_t * s)
    push ebp
    mov ebp, esp
    pushad

    xor ecx, ecx
    mov ebx, [ebp + 8]
    .put_char_loop:
        mov cl, [ebx]
        cmp cl, 0
        jz .str_over
        push ecx
        call put_char
        add esp, 4
        inc ebx
        jmp .put_char_loop

    .str_over:
        popad
        mov esp, ebp
        pop ebp
        ret


global put_int
int_to_str_buf dq 0
    db 0
int_perfix db "0x", 0
put_int:
    ;put_int(uint32_t)
    mov dword [int_to_str_buf], 0
    mov dword [int_to_str_buf + 4], 0
    push ebp
    mov ebp, esp
    pushad

    push int_perfix
    call put_str
    add esp, 4

    mov eax, [ebp + 8]
    mov ecx, 7
    mov edi, int_to_str_buf

    .skip_prefix_0:
        mov edx, eax
        mov ebx, ecx

        shr edx, cl
        shr edx, cl
        shr edx, cl
        shr edx, cl


        and edx, 0xf
        cmp edx, 0
        jne .print_read
        loop .skip_prefix_0

    .print_read:
        inc ecx
    .int_to_char_loop:
        dec ecx
        mov edx, eax
        shr edx, cl
        shr edx, cl
        shr edx, cl
        shr edx, cl
        and edx, 0xf
        inc ecx
        cmp edx, 9
        jle .num
    
    .a2f:
        sub edx, 10
        add edx, 'A'
        mov [edi], dl
        inc edi
        jmp .put_int_loop
    
    .num:
        add edx, '0'
        mov [edi], dl
        inc edi
        jmp .put_int_loop

    .put_int_loop:
        loop .int_to_char_loop
    

    mov edi, int_to_str_buf
    push edi
    call put_str
    add esp, 4
    
    popad
    mov esp, ebp
    pop ebp
    ret
    

