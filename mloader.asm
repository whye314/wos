%include "boot.inc"



SECTION LOADER vstart=LOADER_BASE_ADDR
    LOADER_STACK_TOP equ LOADER_BASE_ADDR
    jmp loader_start



    ;gdt
    GDT_BASE: dq 0x0

    CODE_DESC: dd 0x0000ffff
                dd DESC_CODE_HIGH4
    
    DATA_DESC: dd 0x0000ffff
                    dd DESC_DATA_HIGH4

    STACK_DESC: dd 0x00000000
                dd DESC_STACK_HIGH4
    
    VIDEO_DESC: dd 0x80000007
                dd DESC_VIDEO_HIGH4
    
    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1


    SELECTOR_CODE equ (0x1 << 3) + TI_GDT + RPL0
    SELECTOR_DATA equ (0x2 << 3) + TI_GDT + RPL0
    SELECTOR_STACK equ (0x3 << 3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x4 << 3) + TI_GDT + RPL0

    gdt_ptr dw GDT_LIMIT
            dd GDT_BASE
    

    times (0x200 - ($ - $$)) db 0
    total_mem_byte dd 0
    ards_buf times 200 db 0

    loaderMsg1 db "gdt initialization finished."
    loaderMsg2 db "protection initialization finished."
    mem_error_msg db "get memory size error."
    page_finish db "page initization finished."
    kernel_load_finish db "kernel load finished."
    jmp_to_kernel db "jump to kernel."

    ards_n dw 0
    

    

    pro_headers_n dw 0
    pro_header_size dw 0
    pro_header_offset dd 0




    loader_start:
        mov ax, cs
        mov es, ax

        ;clear
        mov ax, 0600h
        mov bx, 0fh
        mov cx, 0
        mov dx, 184fh
        int 10h

        ;print

        mov ax, 1301h
        mov bx, 00fh
        mov cx, 28
        mov dx, 0
        mov bp, loaderMsg1
        mov sp, LOADER_STACK_TOP
        int 10h


        ;get memory size
        xor ebx, ebx
        mov edx, 0x534d4150
        mov di, ards_buf
        
        .e820_get_mem_loop:
            mov eax, 0xe820
            mov ecx, 20
            int 15h
            jc .read_size_error
            add di, cx
            inc word [ards_n]
            cmp ebx, 0
            jnz .e820_get_mem_loop
        mov ecx, [ards_n]
        mov ebx, ards_buf
        xor edx, edx
        .find_max_ards:
            mov eax, [ebx]
            add eax, [ebx+8]
            add ebx, 20
            cmp edx, eax
            jge .next_ards
            mov edx, eax

        .next_ards:
            loop .find_max_ards
            jmp .mem_get_ok
        
        .mem_get_ok:
            mov [total_mem_byte], edx



        ;A20
        in al, 0x92
        or al, 10b
        out 0x92, al

        ;load gdt
        lgdt [gdt_ptr]

        ;cr0[0] = 1
        mov eax, cr0    ;not ax
        or eax, 1
        mov cr0, eax
        
        jmp dword SELECTOR_CODE: protection_mode_start

        .read_size_error:
            mov ax, 1301h
            mov bx, 0fh
            mov cx, 22
            mov dx, 0100h
            mov bp, mem_error_msg
            int 10h

            jmp $
    

    
    
    [bits 32]
    protection_mode_start:
        mov ax, SELECTOR_DATA
        mov ds, ax
        mov es, ax
        mov ax, SELECTOR_STACK
        mov ss, ax
        mov esp, LOADER_STACK_TOP
        mov ax, SELECTOR_VIDEO
        mov gs, ax



        mov bp, loaderMsg2
        mov al, 0xf
        mov ecx, 35
        mov bh, 1
        mov bl, 0
        mov dx, 0
        call print


        ;read kernel from disk
        mov eax, KERNEL_SECTOR
        mov ebx, KERNEL_ADDR_BIN
        mov ecx, 200
        call rd_disk_m_16;

        mov bp, kernel_load_finish
        mov al, 0xf
        mov ecx, 21
        mov bx, 0x200
        call print


        call set_page

        sgdt [gdt_ptr]

        mov eax, [gdt_ptr + 2]; base of gdt
        or dword [eax + 0x20 + 4], 0xc0000000
        add dword [gdt_ptr+2], 0xc0000000
        add esp, 0xc0000000

        mov eax, PAGE_DIR_ROOT
        mov cr3, eax

        mov eax, cr0
        or eax, 1 << 31
        mov cr0, eax
        lgdt [gdt_ptr]

        jmp ($+2)

        mov ecx, 26
        mov al, 0xf
        mov bx, 0x300
        mov bp, page_finish
        call print


        ;init kernel
        call kernel_init;0x4d5
        mov esp, 0xc009f000

        ;print
        mov eax, 0fh
        mov ebx, 0x400
        mov ecx, 15
        mov ebp, jmp_to_kernel
        call print


        ;set cursor
        mov ebx, 0x190
        mov dx, 0x3d4
        mov al, 0x0e
        out dx, al
        mov dx, 0x3d5
        mov al, bh
        out dx, al

        mov dx, 0x3d4
        mov al, 0xf
        out dx, al
        mov al, bl
        mov dx, 0x3d5
        out dx, al

        jmp 0xc0001500;0xe19





        jmp $

        ;con not use bios interrupt in protection mode!
        ; mov ax, 0x1301
        ; mov bx, 0x000f
        ; mov cx, 35
        ; mov dx, 0100h
        ; mov bp, loaderMsg2
        ; int 10h
    rd_disk_m_16:
                ;ax = sector
                ;bx = destination address
                ;cx = sector count
        ;bakeup eax ecx

        mov esi, eax
        mov edi, ecx
        xor edx, edx
        
        ;set sector count
        mov dx, 0x1f2
        mov al, cl
        out dx, al
        mov eax, esi

        ;set LBA
        ;set low LBA
        mov dx, 0x1f3
        out dx, al

        ;set mid LBA
        shr eax, 8
        mov dx, 0x1f4
        out dx, al

        ;sey high LBA
        shr eax, 8
        mov dx, 0x1f5
        out dx, al

        ;set device
        shr eax, 8
        and eax, 0xf
        or eax, 0xe0
        mov dx, 0x1f6
        out dx, al

        ;set command
        mov dx, 0x1f7
        mov al, 0x20
        out dx, al

        ;not ready
        .not_ready:
            in al, dx
            and al, 0x88
            cmp al, 0x8
            jnz .not_ready
            
        ;disk ready
        ;sector count*512/2 
        mov ax, di
        mov dx, 256
        mul dx
        mov cx, ax

        mov dx, 0x1f0

        ;read loop
        .read_loop:
            in ax, dx
            mov [ebx], ax
            add ebx, 2
            loop .read_loop
            ret

    set_page:
        xor edi, edi
        mov ecx, 4096
        mov eax, PAGE_DIR_ROOT
        .clear_pde:
            mov byte [eax+edi], 0
            inc edi
            loop .clear_pde
        
        cr3_ptr dd PAGE_DIR_ROOT

        .create_pde:
            mov eax, PAGE_DIR_ROOT
            or eax, PG_P | PG_RW_W | PG_US_U
            mov [PAGE_DIR_ROOT + 4092], eax
            add eax, 0x1000
            mov [PAGE_DIR_ROOT], eax
            mov [PAGE_DIR_ROOT+0xc00], eax
        
        ;create pte
        mov ecx, 256
        mov esi, 0
        mov edx, PG_US_U | PG_RW_W | PG_P
        mov eax, PAGE_DIR_ROOT
        add eax, 0x1000
        .create_pte:
            mov [eax+esi*4], edx
            add edx, 4096
            inc esi
            loop .create_pte
            
        ;create kernel pde
        add eax, 0x1000
        or eax, PG_US_U | PG_RW_W | PG_P
        mov ecx, 254
        mov esi, 0x301

        .create_kernel_pde:
            mov [PAGE_DIR_ROOT + esi * 4], eax
            inc esi
            add eax, 0x1000
            loop .create_kernel_pde
        
        ret







        ;print
    print:
        ;es: bp = msg
        ;al = attribute
        ;cx = len
        ;bx = (row, col), bh = row, bl = col
        mov dx, ax
        mov al, bh
        mov bh, 160
        mul bh
        xor bh, bh
        add ax, bx
        add ax, bx
        mov bx, ax



        .loop_once:
            
            mov al, [es: bp]
            mov [gs: bx], al
            mov [gs: bx+1], dl
            add bx, 2
            add bp, 1
            loop .loop_once
        
        ret


    kernel_init:;0x5ff

        xor eax, eax
        xor ebx, ebx
        xor ecx, ecx
        xor edx, edx

        
        mov bx, [KERNEL_ADDR_BIN + 44]
        mov [pro_headers_n], bx


        mov bx, [KERNEL_ADDR_BIN + 42]
        mov [pro_header_size], bx

        
        mov ebx, [KERNEL_ADDR_BIN + 28]
        mov [pro_header_offset], ebx
        add ebx, KERNEL_ADDR_BIN

        mov dx, [pro_header_size]
        mov cx, [pro_headers_n]
        and ecx, 0x0000ffff
        and edx, 0x0000ffff

        .init_each_pro:
            ;program address = ebx

            cmp dword [ebx], 0;PT_NULL
            je .loop_process

            push dword [ebx + 16];program file size
            push dword [ebx + 8];program dest address
            ;program src address
            mov eax, [ebx + 4]
            add eax, KERNEL_ADDR_BIN
            push dword eax
            call mem_cpy
            add esp, 12

        .loop_process:
            add ebx, edx
            loop .init_each_pro
        
        ret
    

    mem_cpy:
        push ebp
        mov ebp, esp
        ;mem_cpy(src, des, sizer)
        cld;address autoadd
        push ecx
        mov esi, [ebp + 8]
        mov edi, [ebp + 12]

        mov ecx, [ebp + 16]
        rep movsb

        pop ecx
        mov esp, ebp
        pop ebp
        ret


        
            






        



    
