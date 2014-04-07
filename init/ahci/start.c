#define qword unsigned long long
#define word unsigned short

void start()
{
	initahci();

        finddisk();

	readtable();

	readpart();

	automount();

        //fat16_cd(0x202020204556494c);
        //fat16_load(0x202020204556494c);
    //fat32_cd(0x202020204556494c);

    //fat32_load(0x202020204556494c);
    //    //name=0x202020204556494c;//live
    //        //name=0x2020202020204433;//3d
    //            fat32_load(name=0x202020544e495250);
    //
	asm("int3");
}
