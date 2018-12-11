[BITS 16]
SECTION .loader_start
global loader_start
loader_start:
  jmp loader_main
gdt_addr:
  GDT_BASE:
    dd 0x00000000
    dd 0x00000000
  GDT_CODE:
    dd 0x0000FFFF
    dd 0x00CF9A00
  GDT_DATA:
    dd 0x0000FFFF
    dd 0x00CF9200
  GDT_VIDEO:
    dd 0x80000007
    dd 0x00C0920B
gdt_end:
GDT_SIZE equ gdt_end - gdt_addr
GDT_LIMIT equ GDT_SIZE - 1
CODE_SELECTOR equ 0x8
DATA_SELECTOR equ 0x10
VIDEO_SELECTOR equ 0x18
gdt_ptr:
  dw GDT_LIMIT
  dd gdt_addr
loader_main:
  ; Print String
  mov ax, 0x1300
  mov bx, 0x0007
  mov cx, 52
  mov dx, 0x0200
  mov bp, acknoledgement
  int 0x10

  in al, 0x92
  or al, 2
  out 0x92, al

  lgdt [gdt_ptr]

  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  
  jmp CODE_SELECTOR:p_mode
acknoledgement db "[16 bits] Miros: For You, my Parents, and my friends."
[BITS 32]
p_mode:
  mov esp, 0x900

  mov dx, 0x300
  mov ecx, 74
  mov ebp, ack_1_msg
  call write_str

  mov dx, 0x400
  mov ecx, 57
  mov ebp, ack_2_msg
  call write_str

  xor eax, eax
  xor ebx, ebx
  xor ecx, ecx
  xor edx, edx


  jmp $
ack_1_msg db "[32 bits]        For Wancat, whom the best programmer-friend I have known."
ack_2_msg db "                 For yun_zhen, whom chats with me always."
write_str:
  ; dh, dl position to write
  ; ecx: count of string
  ; ebp: offset of string (start)
  mov ax, VIDEO_SELECTOR
  mov gs, ax
  mov al, dh
  mov dh, 80 ; decimal
  mul dh
  mov dh, 0
  add word ax, dx
  mov bx, ax
.loop_write:
  shl bx, 1
  mov byte al, [ds:bp]
  mov byte [gs:bx], al
  add bx, 1
  mov byte [gs:bx], 0x07
  add bx, 1
  shr bx, 1
  add bp, 1
  loop .loop_write
  ret
