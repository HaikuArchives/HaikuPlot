/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
 #include "AboutWindow.h"
 
 #include <Application.h>
 #include <Screen.h>
 
 
AboutWindow::AboutWindow(void)
	: BWindow(BRect(100,100,400,400), "About", B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	BScreen screen;
	BRect screenrect(screen.Frame());
	
	AboutView *aboutview = new AboutView(Bounds());
	AddChild(aboutview);

	MoveTo((screenrect.Width()-Frame().Width())/2,
		(screenrect.Height()-Frame().Height())/2);
}

AboutView::AboutView(BRect frame)
	: BTextView(frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetText("Current Version: 1.0.0\n\nWritten by: Vale Tolpegin");
	
	MakeEditable(false);
}
