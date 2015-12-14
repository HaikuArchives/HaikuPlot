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
	BRect r(Bounds());
	r.bottom = 20;
	BMenuBar *fMenuBar = new BMenuBar(r, "menubar");
	AddChild(fMenuBar);
	
	BMenu *fFileMenu = new BMenu("File");
	
	fFileMenu->AddItem(new BMenuItem("Load", new BMessage(LOAD_PLOT), 'L',
		B_COMMAND_KEY));
	
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
	fPictureView->ResizeTo(fPictureBitmap->Bounds().Width(),
		fPictureBitmap->Bounds().Height());
	
	this->ResizeTo(fPictureView->Bounds().Width(),
		fPictureView->Bounds().Height() + 20);
}
