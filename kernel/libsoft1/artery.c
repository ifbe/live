void initacpi();
void initfat();
void say(void*, ...);




void arterycreate()
{
	say("artery?\n");
	initacpi();
	initfat();
	say("artery!\n");
}
