/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#ifndef HAIKUPLOT_H
#define HAIKUPLOT_H

#include <Window.h>

#include <MenuBar.h>
#include <String.h>
#include <FilePanel.h>
#include <Entry.h>

class HaikuPlot : public BWindow
{
	public:
		HaikuPlot(void);
		
		bool QuitRequested(void);
		void MessageReceived(BMessage *msg);
		
	private:
		void LoadPlot(const entry_ref &ref);
		
		BFilePanel *fOpenPanel;
		BBitmap *fPictureBitmap;
		BPicture *fPicturePicture;
		BView *fPictureView;
};

#endif
