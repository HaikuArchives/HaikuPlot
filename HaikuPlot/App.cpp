/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <OS.h>
#include <stdlib.h>

#include "HaikuPlot.h"

class App : public BApplication
{
	public:
		App(void);
};

App::App(void)
	: BApplication("application/x-vnd.haikuplot")
{
	HaikuPlot* mainWin = new HaikuPlot();
	mainWin->Show();
}


int
main(void)
{
	App app;
	app.Run();
	return 0;
}
