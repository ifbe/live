	.file	"start.c"
	.section	.rodata
.LC0:
	.string	"disk:"
.LC1:
	.string	"bad disk"
.LC2:
	.string	"good disk"
	.text
	.globl	start
	.type	start, @function
start:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$0, %eax
	call	initahci
#APP
# 11 "start.c" 1
	leaq initahci,%rax
	movq %rax,0x8000
	
# 0 "" 2
#NO_APP
	movl	$0, %eax
	call	finddisk
	cltq
	movq	%rax, -8(%rbp)
	movl	$4294967295, %eax
	andq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	say
	movq	-8(%rbp), %rax
	movl	$2, %ecx
	movq	%rax, %rdx
	movl	$0, %esi
	movl	$1048576, %edi
	movl	$0, %eax
	call	read
	movl	$1049086, %eax
	movzwl	(%rax), %eax
	cmpw	$-21931, %ax
	je	.L2
	movl	$0, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	say
	jmp	.L1
.L2:
	movl	$0, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	say
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	findfat
	cltq
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L4
	jmp	.L1
.L4:
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movl	$1, %ecx
	movq	%rax, %rsi
	movl	$1048576, %edi
	movl	$0, %eax
	call	read
	movl	$0, %eax
	call	fstype
	cltq
	movq	%rax, -24(%rbp)
	cmpq	$16, -24(%rbp)
	jne	.L5
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	fat16
.L5:
	cmpq	$32, -24(%rbp)
	jne	.L1
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	fat32
.L1:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	start, .-start
	.ident	"GCC: (GNU) 4.8.2 20131212 (Red Hat 4.8.2-7)"
	.section	.note.GNU-stack,"",@progbits
