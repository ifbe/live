void wait_cycles(unsigned int n);
int mbox_call(unsigned char ch);
void say(void*, ...);




#define MMIO_BASE       0x3F000000

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(MMIO_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(MMIO_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(MMIO_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000
#define MBOX_REQUEST    0
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8
#define MBOX_TAG_SETPOWER       0x28001
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0
volatile unsigned int  __attribute__((aligned(16))) mbox[36];




#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020
void reboot()
{
	unsigned int r;
	say("@reboot\n");

	r = *PM_RSTS; r &= ~0xfffffaaa;
	*PM_RSTS = PM_WDOG_MAGIC | r;   // boot from partition 0
	*PM_WDOG = PM_WDOG_MAGIC | 10;
	*PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}
void poweroff()
{
	unsigned long r;
	say("@poweroff\n");

	// power off devices one by one
	for(r=0;r<16;r++) {
		mbox[0]=8*4;
		mbox[1]=MBOX_REQUEST;
		mbox[2]=MBOX_TAG_SETPOWER; // set power state
		mbox[3]=8;
		mbox[4]=8;
		mbox[5]=(unsigned int)r;   // device id
		mbox[6]=0;                 // bit 0: off, bit 1: no wait
		mbox[7]=MBOX_TAG_LAST;
		mbox_call(MBOX_CH_PROP);
	}
	// power off gpio pins (but not VCC pins)
	*GPFSEL0 = 0;
	*GPFSEL1 = 0;
	*GPFSEL2 = 0;
	*GPFSEL3 = 0;
	*GPFSEL4 = 0;
	*GPFSEL5 = 0;
	*GPPUD = 0;
	wait_cycles(150);
	*GPPUDCLK0 = 0xffffffff;
	*GPPUDCLK1 = 0xffffffff;
	wait_cycles(150);
	*GPPUDCLK0 = 0; *GPPUDCLK1 = 0;        // flush GPIO setup

	// power off the SoC (GPU + CPU)
	r = *PM_RSTS; r &= ~0xfffffaaa;
	r |= 0x555;    // partition 63 used to indicate halt
	*PM_RSTS = PM_WDOG_MAGIC | r;
	*PM_WDOG = PM_WDOG_MAGIC | 10;
	*PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}
