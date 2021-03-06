#define MMIO_BASE	   0x3F000000
#define SYSTMR_LO		((volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI		((volatile unsigned int*)(MMIO_BASE+0x00003008))
void say(void*, ...);




void wait_cycles(unsigned int n)
{
	if(0 == n)return;
	while(n--) { asm volatile("nop"); }
}
void wait_msec(unsigned int n)
{
	register unsigned long f, t, r;
	// get the current counter frequency
	asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
	// read the current counter
	asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
	// calculate expire value for counter
	t+=((f/1000)*n)/1000;
	do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}
unsigned long get_system_timer()
{
	unsigned int h=-1, l;
	// we must read MMIO area as two separate 32 bit reads
	h=*SYSTMR_HI;
	l=*SYSTMR_LO;
	// we have to repeat it if high word changed during read
	if(h!=*SYSTMR_HI) {
		h=*SYSTMR_HI;
		l=*SYSTMR_LO;
	}
	// compose long int value
	return ((unsigned long) h << 32) | l;
}
void wait_msec_st(unsigned int n)
{
	// we must check if it's non-zero, because qemu does not emulate
	// system timer, and returning constant zero would mean infinite loop
	unsigned long t = get_system_timer();
	if(0 == t)return;

	while(get_system_timer() < t+n);
}
