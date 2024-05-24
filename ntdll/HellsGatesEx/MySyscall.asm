
.data
	syscallId dword 0h
	comment *
		全局变量问题，调用方式不对会导致调用的函数不变
	* comment
.code

	MySyscallWrapper proc
		mov syscallId,0
		mov syscallId,ecx
		ret
	MySyscallWrapper endp

	MySyscall proc
		mov r10,rcx
		mov eax,syscallId
		syscall
		ret

	MySyscall endp

end