#if (APPLE)
  #define FUNCTION_NAME(name) _##name
#else
  #define FUNCTION_NAME(name) name
#endif

# Make symbols visible for linker
.globl FUNCTION_NAME(Capture)
.globl FUNCTION_NAME(JumpTo)
.intel_syntax noprefix

# Solution starts here
# You need just two instructions: 'movq' and 'ret'

FUNCTION_NAME(Capture):
# save to context
    mov rax,           [rsp]
    mov [rdi],         rax
    mov [rdi + 8],     rbx
    mov [rdi + 16],    rbp
    mov [rdi + 24],    r12
    mov [rdi + 32],    r13
    mov [rdi + 40],    r14
    mov [rdi + 48],    r15
    ret

FUNCTION_NAME(JumpTo):
    mov rax,    [rdi]
    mov [rsp],  rax
    mov rbx,    [rdi + 8]
    mov rbp,    [rdi + 16]
    mov r12,    [rdi + 24]
    mov r13,    [rdi + 32]
    mov r14,    [rdi + 40]
    mov r15,    [rdi + 48]
    ret
