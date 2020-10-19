void initplat();
void say(void*, ...);




void devicecreate()
{
	say("device prep{\n");
	initplat();
	say("}device done\n");
}
void devicedelete()
{
	say("device free{\n");
	say("}device gone\n");
}
