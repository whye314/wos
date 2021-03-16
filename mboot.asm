
%include "boot.inc"
SECTION MBR vstart=0x7c00
    jmp boot
    helloString db "Hello, this is MBR."
boot:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 7c00h

    ; clear screen

    mov ax, 0600h
    mov bx, 1700h
    mov cx, 0
    mov dx, 184fh
    int 10h

    ; mov ah, 3
    ; mov bx, 0
    ; int 10h

    mov ax, 1301h
    mov bx, 000fh
    mov cx, 019
    mov dx, 0000h
    mov bp, helloString

    int 10h


    
    mov ax, helloString

    mov eax, LOADER_START_SECTOR
    mov bx, LOADER_BASE_ADDR
    mov cx, 4
    call rd_disk_m_16

    jmp LOADER_BASE_ADDR

    rd_disk_m_16:
                ;ax = sector
                ;bx = destination address
                ;cx = sector count
        ;bakeup eax ecx
        mov esi, eax
        mov edi, ecx
        
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
            mov [bx], ax
            add bx, 2
            loop .read_loop
            ret
    



    





    
        


jmp $

times 510-($ - $$) db 0

dw 0xaa55