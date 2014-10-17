;现在cpu已经被初始化完毕
;你被放在不知道哪儿
;你有0xe000的空间可以放任何代码
;比如：



mov rax,0
add rbx,1
sub rcx,2
shl rdx,4

sleep:hlt
jmp sleep
