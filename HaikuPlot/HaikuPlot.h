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
#include <TextControl.h>
#include <Node.h>

class HaikuPlot : public BWindow
{
	public:
		HaikuPlot(void);
		
		bool QuitRequested(void);
		void MessageReceived(BMessage *msg);
		
	private:
		void LoadPlot(const entry_ref &ref);
		void LoadPlot(void);
		void GeneratePlot(const entry_ref &ref);
		
		void PrepareNodeMonitoring(const entry_ref &ref);
		void HandleNodeMonitoring(BMessage *msg);
		
		BFilePanel *fOpenPanel;
		BBitmap *fPictureBitmap;
		BView *fPictureView;
		
		bool loading;
		bool loading_plot;
		
		entry_ref fRef;
		node_ref fNodeRef;
};

#endif
