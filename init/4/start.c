void start()
{
	initahci();
	main();
	void (*f)()=(void*)0x20000;
asm("int3");
	f();
}
