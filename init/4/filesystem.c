#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

void fat16(QWORD disk)
{
	QWORD partition=(QWORD)(*(DWORD*)0x10001c);
	say("partition:",partition);
	QWORD fat0=partition+(QWORD)(*(WORD*)0x10000e);
	say("fat0:",fat0);
	QWORD fatsize=(QWORD)(*(WORD*)0x100016);
	say("fatsize:",fatsize);
	QWORD clustersize=(QWORD)(*(BYTE*)0x10000d);
	say("clustersize:",clustersize);
	QWORD cluster0=fat0+fatsize*2+32-clustersize*2;
	say("cluster0:",cluster0);

	read(0x100000,fat0+fatsize*2,disk,32);	//read root
	say("cd root:",fat0+fatsize*2);

	QWORD name;

	//fat16_cd(0x202020204556494c);
	name=0x202020204556494c;

	//void fat16_cd(QWORD name)
	{
		QWORD p=0x104000;
		for(;p>0x100000;p-=0x20)
		{
			if( *(QWORD*)p==name )
			{
				if( *(BYTE*)(p+0xb)==0x10) break;
			}
		}
		if(p==0x100000){say("directory not found,bye!",0);return;}

		QWORD directory=(QWORD)(*(WORD*)(p+0x1a)); //fat16,only 16bit

		read(0x100000,cluster0+directory*clustersize,disk,clustersize);
		say("changed directory:",cluster0+directory*clustersize);
	}


	//fat16_load(0x202020204556494c);
	name=0x202020204556494c;

	//void fat16_load(QWORD name)
	{
		QWORD p=0x104000;
		for(;p>0x100000;p-=0x20){ if( *(QWORD*)p==name ) break;	}
		if(p==0x100000){say("file not found,bye!",0);return;}
	
		QWORD file=(QWORD)(*(WORD*)(p+0x1a));	//fat16,only 16bit
		say("first cluster of file:",file);

		p=0x100000;
		read(0x40000,fat0,disk,0x200);	//fat16,cache all
		while(p<0x200000)
		{
			read(p,cluster0+clustersize*file,disk,clustersize);
			say("read:",cluster0+clustersize*file);
			p+=clustersize*0x200;

			file=(QWORD)(*(WORD*)(0x40000+2*file));

			if(file<2){say("impossible cluster,bye!",0);return;}
			if(file==0xfff7){say("bad cluster,bye!",0);return;}
			if(file>=0xfff8) break;
		}
		say("total bytes:",p-0x100000);
	}

}


void fat32(QWORD disk)
{
    QWORD partition=(QWORD)(*(DWORD*)0x10001c);
    say("partition:",partition);
    QWORD fat0=partition+(QWORD)(*(WORD*)0x10000e);
    say("fat0:",fat0);
    QWORD fatsize=(QWORD)(*(DWORD*)0x100024);
    say("fatsize:",fatsize);
    QWORD clustersize=(QWORD)(*(BYTE*)0x10000d);
    say("clustersize:",clustersize);
    QWORD cluster0=fat0+fatsize*2-clustersize*2;
    say("cluster0:",cluster0);

    read(0x100000,cluster0+clustersize*2,disk,clustersize); //read root
    say("cd root:",cluster0+clustersize*2);

    QWORD name;

    //fat32_cd(0x202020204556494c);
    name=0x202020204556494c;

    //void fat32_cd(QWORD name)
    {
        QWORD p=0x100000+clustersize*0x200;
        for(;p>0x100000;p-=0x20)
        {
            if( *(QWORD*)p== name)
            {
                if( *(BYTE*)(p+0xb)==0x10) break;
            }
        }
        if(p==0x100000){say("directory not found,bye!",0);return;}
 
        QWORD directory=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
        directory+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit

        read(0x100000,cluster0+directory*clustersize,disk,clustersize);
        say("changed directory:",cluster0+directory*clustersize);
    }


    //fat32_load(0x202020204556494c);
    //name=0x202020204556494c;//live
    //name=0x2020202020204433;//3d
    name=0x202020544e495250;

    //void fat32_load(QWORD name)
    {
        QWORD p=0x100000+clustersize*0x200;
        for(;p>0x100000;p-=0x20)
        {
            if( *(QWORD*)p== name) break;
        }
        if(p==0x100000){say("file not found,bye!",0);return;}

        QWORD file=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
        file+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit
        say("first cluster of file:",file);

        QWORD cacheblock=0; //512 sectors per block
        QWORD i;
        p=0x100000;
        for(i=0;i<0x40;i++){
            read(0x40000+i*0x1000,fat0+cacheblock*0x200+8*i,disk,8);
        }
        while(p<0x200000)
        {
            read(p,cluster0+clustersize*file,disk,clustersize);
            //say("read:",cluster0+clustersize*file);

            p+=clustersize*0x200;
            if( (file/0x10000) !=cacheblock) //0x10000 perblock
            {
                cacheblock=file/0x10000;
                for(i=0;i<0x40;i++){
                read(0x40000+i*0x1000,fat0+cacheblock*0x200+8*i,disk,8);
                }
                say("reload cache:",fat0+cacheblock*0x200);
            }

            file=(QWORD)(*(DWORD*)(0x40000+4*(file%0x10000)));
            if(file<2){say("impossible cluster,bye!",0);return;}
            if(file==0x0ffffff7){say("bad cluster,bye!",0);return;}
            if(file>=0x0ffffff8){say("last cluster:",file);break;}
        }
    say("total bytes:",p-0x100000);
    say("0x40000:",*(QWORD*)0x40000);
    }

}
