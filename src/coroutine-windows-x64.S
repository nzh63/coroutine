.globl _doSwichTo
_doSwichTo:
mov %rsp, 0x00(%rcx)
mov %r12, 0x08(%rcx)
mov %r13, 0x10(%rcx)
mov %r14, 0x18(%rcx)
mov %r15, 0x20(%rcx)
mov %rbx, 0x28(%rcx)
mov %rbp, 0x30(%rcx)
mov %rdi, 0x38(%rcx)
mov %rsi, 0x40(%rcx)
mov %gs:(0x08), %rax
mov %rax, 0x48(%rcx)
mov %gs:(0x10), %rax
mov %rax, 0x50(%rcx)
movups %xmm6, 0x58(%rcx)
movups %xmm7, 0x68(%rcx)
movups %xmm8, 0x78(%rcx)
movups %xmm9, 0x88(%rcx)
movups %xmm10, 0x98(%rcx)
movups %xmm11, 0xa8(%rcx)
movups %xmm12, 0xb8(%rcx)
movups %xmm13, 0xc8(%rcx)
movups %xmm14, 0xd8(%rcx)
movups %xmm15, 0xe8(%rcx)

mov 0x00(%rdx), %rsp
mov 0x08(%rdx), %r12
mov 0x10(%rdx), %r13
mov 0x18(%rdx), %r14
mov 0x20(%rdx), %r15
mov 0x28(%rdx), %rbx
mov 0x30(%rdx), %rbp
mov 0x38(%rdx), %rdi
mov 0x40(%rdx), %rsi
mov 0x48(%rdx), %rax
mov %rax, %gs:(0x08)
mov 0x50(%rdx), %rax
mov %rax, %gs:(0x10)
movups 0x58(%rdx), %xmm6
movups 0x68(%rdx), %xmm7
movups 0x78(%rdx), %xmm8
movups 0x88(%rdx), %xmm9
movups 0x98(%rdx), %xmm10
movups 0xa8(%rdx), %xmm11
movups 0xb8(%rdx), %xmm12
movups 0xc8(%rdx), %xmm13
movups 0xd8(%rdx), %xmm14
movups 0xe8(%rdx), %xmm15

retq
