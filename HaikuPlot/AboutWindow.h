/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <Window.h>
#include <View.h>
#include <TextView.h>

class AboutView : public BTextView
{
	public:
		AboutView(BRect frame);
		
		bool QuitRequested(void);
};

class AboutWindow : public BWindow
{
	public:
		AboutWindow(void);		
};

#endif
