[bits 32]

section .text
global switch_to

switch_to:
    push esi
    push edi
    push ebx
    push ebp
    ;pushd

    mov eax, [esp + 20]
    mov [eax], esp

    mov eax, [esp + 24]
    mov esp, [eax]
    ;popd
    pop ebp
    pop ebx
    pop edi
    pop esi

    ret