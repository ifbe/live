#include "struct.h"
QWORD disk;


void finddisk()
{
        QWORD addr=0x2808;
        for(;addr<0x3000;addr+=0x10)
        {
                if( *(unsigned int*)addr ==0x61746173)
                {
                        addr-=0x8;
                        disk=(QWORD)(*(unsigned int*)addr);
                        say("sata address:",disk);
			return;
                }
        }
	disk=0;
	return;
}


void fat16()
{
	QWORD partition=(QWORD)(*(DWORD*)0x10001c);
	say("partition:",partition);
	QWORD fat0=partition+(QWORD)(*(WORD*)0x10000e);
	say("fat0:",fat0);
	QWORD fatsize=(QWORD)(*(WORD*)0x100016);
	say("fatsize:",fatsize);
	QWORD clustersize=(QWORD)(*(BYTE*)0x10000d);
	say("clustersize:",clustersize);
	QWORD base=fat0+fatsize*2+32-clustersize*2;
	say("base:",base);
	QWORD root=base+clustersize*2;//=cluster2
	say("root:",root);

	read(0x100000,root,disk,32);
	QWORD p=0x104000;
	for(;p>0x100000;p-=0x20)
	{
		if( *(QWORD*)p==0x202020204556494c )
		{
			if( *(BYTE*)(p+0xb)==0x10) break;
		}
	}
	if(p==0x100000){say("not found,bye!",0);return;}

	QWORD live=((QWORD)(*(WORD*)(p+0x14)))<<16;	//high 16bit
	live+=(QWORD)(*(WORD*)(p+0x1a));		//low 16bit
	say("cd live:",live);

	read(0x100000,fat0,disk,fatsize);
	say("0x100000:whole fat",0);

	QWORD next=live;
	p=0x200000;
	do{
		read(p,base+clustersize*next,disk,clustersize);
		p+=clustersize*0x200;
		next=(QWORD)(*(WORD*)(0x100000+2*next));
		if(next==0xfff7){say("bad cluster",0);return;}
	}while(next>=0xfff8);
}


void fat32()
{
	QWORD partition=(QWORD)(*(DWORD*)0x10001c);
	say("partition:",partition);
	QWORD fat0=partition+(QWORD)(*(WORD*)0x10000e);
	say("fat0:",fat0);
	QWORD fatsize=(QWORD)(*(DWORD*)0x100024);
	say("fatsize:",fatsize);
	QWORD clustersize=(QWORD)(*(BYTE*)0x10000d);
	say("clustersize:",clustersize);
	QWORD base=fat0+fatsize*2-clustersize*2;
	say("base:",base);
	QWORD root=base+clustersize*2;//=cluster2
	say("root:",root);

	read(0x100000,root,disk,clustersize);
	QWORD p=0x100000+clustersize*0x200;
	for(;p>0x100000;p-=0x20)
	{
		if( *(QWORD*)p==0x202020204556494c )
		{
			if( *(BYTE*)(p+0xb)==0x10) break;
		}
	}
	if(p==0x100000){say("not found,bye!",0);return;}

	QWORD live=((QWORD)(*(WORD*)(p+0x14)))<<16;	//high 16bit
	live+=(QWORD)(*(WORD*)(p+0x1a));		//low 16bit
	say("cd live:",live);

	read(0x100000,fat0,disk,fatsize);
	say("0x100000:whole fat",0);

	QWORD next=live;
	p=0x200000;
	do{
		read(p,base+clustersize*next,disk,clustersize);
		p+=clustersize*0x200;
		next=0x100000+4*next;
		if(next==0x0ffffff7){say("bad cluster",0);return;}
	}while(next>=0x0ffffff8);
}


void main()
{
	int x,y,rsi;

	finddisk();
	if(disk==0)return;

	//identify(0x100000,disk);

	read(0x100000,0,disk,2);

	if(*(WORD*)0x1001fe==0xAA55)say("good disk",0);
	else{say("bad disk,bye!",0);return;}

	QWORD offset;
	if(*(QWORD*)0x100200==0x5452415020494645)	//GPT 分区表
	{
		say("gpt part",0);
		read(0x100000,2,disk,32);
		for(offset=0x100000;offset<0x104000;offset+=0x80)    //find esp
		{
			if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
				if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
					offset=*(QWORD*)(offset+0x20);
					break;
				}
			}
		}
		if(offset==0x104000)
		{
			say("no esp,bye!",0);
			return;
		}
	}
	else{						//MBR 分区表
		say("mbr disk",0);
		for(offset=0x1001be;offset<0x1001fe;offset+=0x10)    //find fat?
		{
			if( *(BYTE*)(offset+4)==0x7 | *(BYTE*)(offset+4)==0xb )
			{
				offset=(QWORD)(*(DWORD*)(offset+8));
				break;
			}

		}
		if(offset==0x1000fe)
		{
			say("no fat,bye!",0);
			return;
		}
	}

	read(0x100000,offset,disk,1);	//pbr

	if( *(WORD*)0x10000b !=0x200) {say("not 512B sector,bye!",0);return;}
	if( *(BYTE*)0x100010 !=2) {say("not 2 fat,bye!",0);return;}

	int similarity=50;

	if( *(WORD*)0x100011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x100016 ==0) similarity++;		//fat32为0
	else similarity--;

	if(similarity==48){say("fat16",0); fat16();return;}
	if(similarity==52){say("fat32",0); fat32();return;}
	say("strange partition,bye!",0);
	return;
}
