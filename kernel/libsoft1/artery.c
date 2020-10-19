void initacpi();
void initfat();
void say(void*, ...);




void arterycreate()
{
	say("artery prep{\n");
	initacpi();
	initfat();
	say("}artery done\n");
}
void arterydelete()
{
	say("artery free{\n");
	say("}artery gone\n");
}
