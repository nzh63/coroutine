.code
_doSwichTo proc

movups 00h[rcx], xmm6
movups 10h[rcx], xmm7
movups 20h[rcx], xmm8
movups 30h[rcx], xmm9
movups 40h[rcx], xmm10
movups 50h[rcx], xmm11
movups 60h[rcx], xmm12
movups 70h[rcx], xmm13
movups 80h[rcx], xmm14
movups 90h[rcx], xmm15
mov 0a0h[rcx], rsp
mov 0a8h[rcx], r12
mov 0b0h[rcx], r13
mov 0b8h[rcx], r14
mov 0c0h[rcx], r15
mov 0c8h[rcx], rbx
mov 0d0h[rcx], rbp
mov 0d8h[rcx], rdi
mov 0e0h[rcx], rsi
mov rax, gs:[08h]
mov 0e8h[rcx], rax
mov rax, gs:[10h]
mov 0f0h[rcx], rax

movups xmm6, 00h[rdx]
movups xmm7, 10h[rdx]
movups xmm8, 20h[rdx]
movups xmm9, 30h[rdx]
movups xmm10, 40h[rdx]
movups xmm11, 50h[rdx]
movups xmm12, 60h[rdx]
movups xmm13, 70h[rdx]
movups xmm14, 80h[rdx]
movups xmm15, 90h[rdx]
mov rsp, 0a0h[rdx]
mov r12, 0a8h[rdx]
mov r13, 0b0h[rdx]
mov r14, 0b8h[rdx]
mov r15, 0c0h[rdx]
mov rbx, 0c8h[rdx]
mov rbp, 0d0h[rdx]
mov rdi, 0d8h[rdx]
mov rsi, 0e0h[rdx]
mov rax, 0e8h[rdx]
mov gs:[08h], rax
mov rax, 0f0h[rdx]
mov gs:[10h], rax

ret

_doSwichTo endp

end
