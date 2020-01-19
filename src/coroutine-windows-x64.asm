.code
_doSwichTo proc

mov 00h[rcx], rsp
mov 08h[rcx], r12
mov 10h[rcx], r13
mov 18h[rcx], r14
mov 20h[rcx], r15
mov 28h[rcx], rbx
mov 30h[rcx], rbp
mov 38h[rcx], rdi
mov 40h[rcx], rsi
mov rax, gs:[08h]
mov 48h[rcx], rax
mov rax, gs:[10h]
mov 50h[rcx], rax
movups 58h[rcx], xmm6
movups 68h[rcx], xmm7
movups 78h[rcx], xmm8
movups 88h[rcx], xmm9
movups 98h[rcx], xmm10
movups 0a8h[rcx], xmm11
movups 0b8h[rcx], xmm12
movups 0c8h[rcx], xmm13
movups 0d8h[rcx], xmm14
movups 0e8h[rcx], xmm15

mov rsp, 00h[rdx]
mov r12, 08h[rdx]
mov r13, 10h[rdx]
mov r14, 18h[rdx]
mov r15, 20h[rdx]
mov rbx, 28h[rdx]
mov rbp, 30h[rdx]
mov rdi, 38h[rdx]
mov rsi, 40h[rdx]
mov rax, 48h[rdx]
mov gs:[08h], rax
mov rax, 50h[rdx]
mov gs:[10h], rax
movups xmm6, 58h[rdx]
movups xmm7, 68h[rdx]
movups xmm8, 78h[rdx]
movups xmm9, 88h[rdx]
movups xmm10, 98h[rdx]
movups xmm11, 0a8h[rdx]
movups xmm12, 0b8h[rdx]
movups xmm13, 0c8h[rdx]
movups xmm14, 0d8h[rdx]
movups xmm15, 0e8h[rdx]

ret

_doSwichTo endp

end
