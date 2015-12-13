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

enum
{
	LOAD_PLOT = 'ldpt'
};

HaikuPlot::HaikuPlot(void)
	: BWindow(BRect(100,100,700,600), "HaikuPlot", B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS)
{
	BView *input_view = new BView(BRect(0,0,700,100), "input_textview",
		B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(input_view);
		
	BButton *command = new BButton(BRect(0,0,100,50), "commandbutton",
		"Command:", new BMessage(LOAD_PLOT), B_FOLLOW_LEFT |
		B_FOLLOW_BOTTOM);
	BTextView *input_textview = new BTextView(BRect(105,0,700,50), "input",
		B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED | B_FRAME_EVENTS);
	input_textview->MakeEditable(true);
	
	input_view->AddChild(command);
	input_view->AddChild(input_textview);
	
	fPictureView = new BView(BRect(0,110,700,600), "picture_view",
		B_FOLLOW_NONE, B_WILL_DRAW);
	
	BBitmap *blank_bitmap = new BBitmap(fPictureView.bounds(), B_CMAP8,
		true);
	BView test_view = new BView(blank_bitmap.bounds(), "test_view",
		B_FOLLOW_NONE, B_WILL_DRAW);
	blank_bitmap->AddChild(test_view);
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
	switch (msg->what)
	{
		case LOAD_PLOT:
		{
			fOpenPanel->Show();
			break;
		}
		case B_REFS_RECEIVED:
		{
			entry_ref ref;
			if (msg->FindRef("refs", &ref) != B_OK)
				break;
			LoadPlot(ref);
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
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
}
