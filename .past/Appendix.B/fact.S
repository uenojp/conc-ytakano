.global fact ;

fact: ;
    movq $1, %rax ;
L1:
    cmpq $0, %rdi ;
    je L2
    mulq %rdi ;
    decq %rdi
    jmp L1
L2:
    ret ;