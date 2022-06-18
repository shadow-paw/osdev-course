cpu 586
bits 32

global _init, _fini

section .init
_init:
	push ebp
	mov  ebp, esp

section .fini
_fini:
	push ebp
	mov  ebp, esp
