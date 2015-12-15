/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
#include "HaikuPlot.h"

#include <View.h>
#include <Application.h>
#include <Button.h>
#include <TextView.h>
#include <Bitmap.h>
#include <Alert.h>
#include <Directory.h>
#include <Path.h>
#include <File.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <TranslationUtils.h>
#include <stdlib.h>
#include <Roster.h>
#include <NodeMonitor.h>

enum
{
	LOAD_PLOT = 'ldpt',
	GENERATE_PLOT = 'gnpt',
	TEXT_EDITED = 'txed'
};

HaikuPlot::HaikuPlot(void)
	: BWindow(BRect(100,100,700,600), "HaikuPlot", B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS)
{
	BRect r(Bounds());
	r.bottom = 20;
	BMenuBar *fMenuBar = new BMenuBar(r, "menubar");
	AddChild(fMenuBar);
	
	BMenu *fFileMenu = new BMenu("File");
	
	fFileMenu->AddItem(new BMenuItem("Load Plot", new BMessage(LOAD_PLOT),
		'L', B_COMMAND_KEY));
	fFileMenu->AddItem(new BMenuItem("Generate Plot",
		new BMessage(GENERATE_PLOT), 'G', B_COMMAND_KEY));
	
	fMenuBar->AddItem(fFileMenu);
	
	fPictureView = new BView(BRect(0,20,700,600), "picture_view",
		B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(fPictureView);
	
	BMessenger msgr(NULL, this);
	fOpenPanel = new BFilePanel(B_OPEN_PANEL, &msgr, NULL, 0, false);
}

bool HaikuPlot::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void HaikuPlot::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped())
	{
		entry_ref ref;
		if (msg->FindRef("refs", &ref) != B_OK)
			return;
		
		GeneratePlot(ref);
		return;
	}
	
	switch (msg->what)
	{
		case LOAD_PLOT:
		{
			fOpenPanel->Show();
			
			loading_plot = true;
			
			break;
		}
		case GENERATE_PLOT:
		{
			fOpenPanel->Show();
			loading_plot = false;
			
			break;
		}
		case B_REFS_RECEIVED:
		{
			loading = false;
			entry_ref ref;
			if (msg->FindRef("refs", &ref) != B_OK)
				break;
			
			if (loading_plot)
				LoadPlot(ref);
			else
				GeneratePlot(ref);
			
			break;
		}
		case B_NODE_MONITOR:
		{
			HandleNodeMonitoring(msg);
			
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

void HaikuPlot::HandleNodeMonitoring(BMessage *msg)
{
	int32 opcode;
	if (msg->FindInt32("opcode", &opcode) != B_OK)
		return;
	
	switch (opcode)
	{
		case B_STAT_CHANGED:
		{
			GeneratePlot(fRef);
			
			break;
		}
		default:
		{
			break;
		}
	}
}

void HaikuPlot::GeneratePlot(const entry_ref &ref)
{
	PrepareNodeMonitoring(ref);
	
	BEntry entry(&ref, true);
	entry_ref real_ref;
	entry.GetRef(&real_ref);
	
	BFile file(&real_ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	BPath path(&real_ref);
	
	BString *command = new
		BString("gnuplot-x86 -e 'set output \"outpic.png\"' ");
	command->Append(path.Path());
	
	if (system(command->String()) == 0)
	{
		BEntry entry("outpic.png");
		entry_ref pic_ref;
		entry.GetRef(&pic_ref);
		
		LoadPlot(pic_ref);
	}
}

void HaikuPlot::PrepareNodeMonitoring(const entry_ref &ref)
{
	BFile file(&ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	BEntry entry(&ref, true);
	entry.GetRef(&fRef);
	entry.GetNodeRef(&fNodeRef);
	
	watch_node(&fNodeRef, B_WATCH_STAT, this);
}

void HaikuPlot::LoadPlot(const entry_ref &ref)
{
	fPictureView->ClearViewBitmap();
	
	BEntry entry(&ref, true);
	entry_ref real_ref;
	entry.GetRef(&real_ref);
	
	BFile file(&real_ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	BPath path(&real_ref);
	fPictureBitmap = BTranslationUtils::GetBitmap(path.Path());
	
	fPictureView->SetViewBitmap(fPictureBitmap);
	fPictureView->ResizeTo(fPictureBitmap->Bounds().Width(),
		fPictureBitmap->Bounds().Height());
	
	this->ResizeTo(fPictureView->Bounds().Width(),
		fPictureView->Bounds().Height() + 20);
}
