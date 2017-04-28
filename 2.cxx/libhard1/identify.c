#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int ahciidentify(u64 dev, u64 rdi);
int ideidentify(u64 dev, u64 rdi);




void identify(u64 dev, u64 rdi)
{
	ahciidentify(dev, rdi);
}
