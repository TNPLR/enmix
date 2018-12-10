SECTION mbr vstart=0x7c00
MBR_START:
  mov ax, cs ; init all sement register to cx (0x0)
  mov ds, ax
  mov es, ax
  mov ss, ax
MBR_CLS:
  ; int 0x10 with ah = 0x06 to clear screen
  ; mov ah, 0x6
  ; mov al, 0x0
  mov ax, 0x600
  ; mov bh, 0x7
  mov bh, 0x7
  ; mov ch, 0x0
  ; mov cl, 0x0
  mov cx, 0x0;
  ; mov dh, 0x18 25 row (0~24)
  ; mov dl, 0x4F 80 column (0~79)
  mov dx, 0x184F
  int 0x10
MBR_WRITE_TEST:
  ; int 0x10 with ah = 0x0A to write letter
  ; mov ah, 0x0A
  ; mov al, 'Y' // 0x59
  mov ax, 0x0A59
  mov bh, 0
  mov cx, 1
  int 0x10
MBR_HALT:
  cli
  hlt
MBR_END:
  times 510 - ($ - $$) db 0x0
  db 0x55, 0xAA
