#define APIC_ID 0x20
#define APIC_VERSION 0x30

#define TASK_PRIORITY 0x80
#define ARBITRATION_PRIORITY 0x90
#define PROCESSOR_PRIORITY 0xa0
#define EOI 0xb0
#define REMOTE_READ 0xc0
#define LOGICAL_DESTINATION 0xd0
#define SPURIOUS_INTERRUPT 0xf0

#define ISR_31_0 0x100
#define ISR_63_32 0x110
#define ISR_95_64 0x120
#define ISR_127_96 0x130
#define ISR_159_128 0x140
#define ISR_191_160 0x150
#define ISR_223_192 0x160
#define ISR_255_224 0x170

#define TMR_31_0 0x180
#define TMR_63_32 0x190
#define TMR_95_64 0x1a0
#define TMR_127_96 0x1b0
#define TMR_159_128 0x1c0
#define TMR_191_160 0x1d0
#define TMR_223_192 0x1e0
#define TMR_255_224 0x1f0

#define IRR_31_0 0x200
#define IRR_63_32 0x210
#define IRR_95_64 0x220
#define IRR_127_96 0x230
#define IRR_159_128 0x240
#define IRR_191_160 0x250
#define IRR_223_192 0x260
#define IRR_255_224 0x270

#define ERROR_STATUS 0x280
#define LVT_CMCI 0x2f0
#define ICR_31_0 0x300
#define ICR_63_32 0x310
#define LVT_TIMER 0x320
#define LVT_THERMAL 0x330
#define LVT_PERFORMANCE 0x340
#define LVT_LINT0 0x350
#define LVT_LINT1 0x360
#define LVT_ERROR 0x370

#define INITIRL_COUNT 0x380
#define CURRENT_COUNT 0x390
#define DIVIDE_CONFIG 0x3e0




void localapic_start()
{
}
void localapic_stop()
{
}
void localapic_create()
{
}
void localapic_delete()
{
}
