void start()
{
	enableirq8();
	main();
	disableirq8();
}
