/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef HAIKUPLOT_H
#define HAIKUPLOT_H

#include <Bitmap.h>
#include <Directory.h>
#include <Entry.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <Node.h>
#include <Path.h>
#include <SplitView.h>
#include <String.h>
#include <TranslationUtils.h>
#include <TranslationDefs.h>
#include <TranslatorRoster.h>
#include <Window.h>

class HaikuPlot : public BWindow
{
	public:
		HaikuPlot();

		bool		QuitRequested();
		void		MessageReceived(BMessage *msg);

	private:
		void		_BuildLayout();

		void		LoadPlot(const entry_ref &ref);
		void		LoadPlot();
		void		GeneratePlot(const entry_ref &ref);

		void		PrepareNodeMonitoring(const entry_ref &ref);
		void		HandleNodeMonitoring(BMessage *msg);

		void		_SaveAs(BMessage *message);
		void		_SaveToFile(BMessage *message);
		void		SaveToFile(BDirectory* dir, const char* name, BBitmap* bitmap,
						   const translation_format* format);

		void		SaveScript();

		status_t	_SaveSettings();
		status_t	_LoadSettings();

		BFilePanel*	fOpenPanel;
		BFilePanel*	fSavePanel;
		BBitmap*	fPictureBitmap;
		BView*		fPictureView;
		BTextView*	fScriptView;

		BSplitView*	fMainSplitView;

		bool		loading_plot;
		entry_ref	fRef;
		node_ref	fNodeRef;
		BPath		fScriptPath;
};

#endif
