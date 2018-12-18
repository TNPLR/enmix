SECTION mbr vstart=0x7c00
BOOT_START:
  mov ax, cs ; init all sement register to cx (0x0)
  mov ds, ax
  mov es, ax
  mov ss, ax
BOOT_CLS:
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
BOOT_SET_CURSOR:
  mov ah, 0x02
  mov bh, 0
  mov dh, 1
  mov dl, 0
  int 0x10
BOOT_WRITE_TEST:
  ; int 0x10 with ah = 0x0A to write letter
  ; mov ah, 0x0A
  ; mov al, 'T' // 0x59
  mov ax, 0x0959
  mov bx, 0x07
  mov cx, 1
  int 0x10
READ_LOADER:
  mov ax, DAP
  mov si, ax
  mov ah, 0x42
  mov dl, 0x80
  int 0x13
  mov ax, 0x900
  mov ss, ax
  jmp 0x900
BOOT_HALT:
  cli
  hlt
BOOT_END:
DAP:
  db 0x10
  db 0
  dw 8
  dd 0x00000900
  dd 0x00000001
  dd 0x00000000
  times 440 - ($ - $$) db 0x0
MBR_START:
  dd 0x52735273 ; Magic Value
  dw 0x0000
  db 0x80
  db 0x01
  db 0x02
  db 0x00
  db 0x00 ; Partition type
  db 0x01
  db 0x20
  db 0x00
  dd 0x00000001
  dd 0x00003C00
MBR_END:
  times 510 - ($ - $$) db 0x0
  db 0x55, 0xAA
