int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++) key+=memory[i];
		if(key<0) key=-key;
        return key;
}