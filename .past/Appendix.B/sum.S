.global sum ;

sum: ;
    movq $0, %rax ;
L1:
    cmpq $0, %rdi ;
    je L2
    addq %rdi, %rax ;
    decq %rdi ;
    jmp L1
L2:
    ret ;