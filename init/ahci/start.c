void start()
{
	initahci();
	//*((unsigned long long*)0x8000)=(unsigned long long )initahci;
	//函数initahci的地址放到内存,肏蛋的c语言！！！！
	//
	//
	//
	//
	//
	__asm__("leaq initahci(%rip),%rax\n\t"
		"movq %rax,0x8000\n\t");

        unsigned long long disk=finddisk();
	disk&=0xffffffff;
        say("disk:",disk);

        read(0x100000,0,disk,2);
        if( *(unsigned short*)0x1001fe !=0xAA55 ){say("bad disk",0);return;}
        else say("good disk",0);

	unsigned long long fat=findfat(disk);

        if(fat==0) return;
        else read(0x100000,fat,disk,1); //pbr

	unsigned long long type=fstype();

	if(type==16) fat16(disk);
	if(type==32) fat32(disk);
}
