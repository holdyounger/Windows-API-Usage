.code
 
NtCreateThreadEx proc
    mov r10, rcx
    mov eax, 0C7h
    syscall
    ret
NtCreateThreadEx endp
 
end