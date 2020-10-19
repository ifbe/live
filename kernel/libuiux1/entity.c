void terminit();
void say(void*, ...);




void entitycreate()
{
	say("entity prep{\n");
	terminit();
	say("}entity done\n");
}
void entitydelete()
{
	say("entity free{\n");
	say("}entity gone\n");
}
