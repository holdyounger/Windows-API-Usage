
.data
	syscallId dword 0h
	comment *
		ȫ�ֱ������⣬���÷�ʽ���Իᵼ�µ��õĺ�������
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