org 0x7C00

main:
    mov ah, 0x0e
    mov al, 'A'
    int 0x10
    jmp main

times 510 - ($ - $$) db 0
dw 0xaa55