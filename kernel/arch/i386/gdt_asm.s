.section .text
.global loadGdt
.type loadGdt, @function
loadGdt:
    mov 4(%esp), %edx
    mov 12(%esp), %eax
    lgdt (%edx)
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %gs
    mov %eax, %fs
    mov %eax, %ss

    pushl 8(%esp)
    push $.setcs
    ljmp *(%esp)
.setcs:
    add $8, %esp
    ret
.size loadGdt, . - loadGdt
