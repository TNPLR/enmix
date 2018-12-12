[BITS 16]
SECTION .loader_start vstart=0x900
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
  mov ax, DATA_SELECTOR
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov ss, ax
  mov ax, VIDEO_SELECTOR
  mov gs, ax

  mov esp, 0x900

  mov dx, 0x300
  mov ecx, 74
  mov ebp, ack_1_msg
  call write_str

  mov dx, 0x400
  mov ecx, 66
  mov ebp, ack_2_msg
  call write_str

  mov eax, 0x9
  mov ebx, 0x400000
  mov ecx, 0x1000 ; 2 MB
  call ide_reader

  jmp $
ack_1_msg db "[32 bits]        For Wancat, whom the best programmer-friend I have known."
ack_2_msg db "                 For yun_zhen and yu_han, who chat with me always."
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
; ide_reader: Read ATA drive
; eax: LBA
; ebx: ADDR to Write
; ecx: How many sectors to read
ide_reader:
.ide_loop:
  push eax
  push ebx
  push ecx
  mov ecx, 1
  call read_ide
  pop ecx
  pop ebx
  pop eax
  add eax, 1
  add ebx, 512
  loop .ide_loop
  ret
read_ide:
  mov esi, eax
  mov edi, ecx
  mov dx, 0x1f2
  mov al, cl
  out dx, al

  mov eax, esi

  mov dx, 0x1f3
  out dx, al
  mov cl, 8
  shr eax, cl
  mov dx, 0x1f4
  out dx, al

  shr eax, cl
  mov dx, 0x1f5
  out dx, al

  shr eax, cl
  and al, 0x0f
  or al, 0xe0 ;LBA mode
  mov dx, 0x1f6
  out dx, al

  mov dx, 0x1f7
  mov al, 0x20
  out dx, al
.not_ready:
  nop
  in al, dx
  and al, 0x88
  cmp al, 0x08
  jnz .not_ready

  mov eax, edi
  mov edx, 128
  mul edx
  mov ecx, eax

  mov dx, 0x1f0
.go_on_read:
  in eax, dx
  mov [ebx], eax
  add ebx, 4
  loop .go_on_read
  ret
