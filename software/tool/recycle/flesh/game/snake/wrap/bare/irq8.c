void isr8()
{
	(*(unsigned long long*)0x7f8 )++;
}
void enableint8()
{
	//cmos时间中断
	*(unsigned long long*)0x7f8=0;
	enableidt28();
	enableioapic8();
	enableirq8();
}
void disableint8()
{
}
