.code16
.section .fat
.global _start
# We DO NOT Use Boot Sector of FAT
# 3 bytes
_start:
  jmp boot
  nop
# OEMName: 8 bytes
.ascii "MIROS  "
# Bytes per sector
.word 512
# Sectors per cluster
.byte 1
# Reserved sectors count
.word 32
# NumFATs (always 2)
.byte 2
# RootEntCnt (0 in FAT32)
.word 0
# TotSec16 (0 in FAT32)
.word 0
# Media (media type 0xF8 for hard disk)
.byte 0xF8
# FATSz16 (Sectors per FAT; 0 in FAT32)
.word 0
# SecPerTrk (32 is my opinion. In this drive, it doesn't matter.)
.word 32
# NumHeads (I DO NOT know, so I type 2 in it)
.word 2
# HiddSec
.long 1
# TotSec32
.long 98303 # 98304 - 1
# DrvNum
.byte 0
# Reserved
.byte 0
# BootSig
.byte 0x80
# VolID
.long 0x52735273
# VolLab (11 bytes)
.ascii "MIROS DISK "
# FilSysType (8 bytes)
.ascii "FAT32   "
boot:
  jmp .
.org 510, 0x0
.byte 0x55
.byte 0xaa
