void initacpi();
void initfat();
void arterycreate()
{
	say("artery?\n");
	initacpi();
	initfat();
	say("artery!\n");
}
