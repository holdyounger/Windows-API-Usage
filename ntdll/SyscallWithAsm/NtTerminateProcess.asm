.code
 
NtTerminateProcess proc
    mov r10, rcx
    mov eax, 02Ch
    syscall
    ret
NtTerminateProcess endp
 
end