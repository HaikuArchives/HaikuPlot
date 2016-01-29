/*
 * Copyright 2015 Vale Tolpegin <valetolpegin@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "HaikuPlot.h"

#include <Application.h>
#include <Bitmap.h>
#include <BitmapStream.h>
#include <Button.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <NodeInfo.h>
#include <NodeMonitor.h>
#include <Path.h>
#include <private/interface/AboutWindow.h>
#include <Roster.h>
#include <stdio.h>
#include <stdlib.h>
#include <SupportDefs.h>
#include <TextView.h>
#include <View.h>

enum
{
	LOAD_PLOT = 'ldpt',
	GENERATE_PLOT = 'gnpt',
	MSG_SAVE_PANEL = 'mgsp',
	MSG_OUTPUT_TYPE = 'BTMN',
	SAVE_PLOT = 'svas',
	SHOW_ABOUT = 'swat',
	MSG_GENERATE_SCRIPT = 'mgpt',
	MSG_SAVE_SCRIPT = 'mgss',
	MSG_SAVE_SETTINGS = 'svse'
};

const char* kTypeField = "be:type";
const char* kTranslatorField = "be:translator";

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "HaikuPlot"

HaikuPlot::HaikuPlot(void)
	: BWindow(BRect(50,50,1000,600), B_TRANSLATE_SYSTEM_NAME("HaikuPlot"),
		B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS),
	fSavePanel(NULL)
{
	_BuildLayout();

	BMessenger msgr(NULL, this);
	fOpenPanel = new BFilePanel(B_OPEN_PANEL, &msgr, NULL, 0, false);

	fScriptPath = BPath();

	_LoadSettings();
}


void
HaikuPlot::_BuildLayout()
{
	BRect r(Bounds());
	r.bottom = 20;

	BMenuBar *fMenuBar = new BMenuBar(r, "menubar");
	BMenu *fFileMenu = new BMenu(B_TRANSLATE("File"));
	BMenu *fMenuSaveAs = new BMenu(B_TRANSLATE("Export as"), B_ITEMS_IN_COLUMN);
	BMenu *fSettingsMenu = new BMenu(B_TRANSLATE("Settings"));
	BMenu *fScriptMenu = new BMenu(B_TRANSLATE("Script"));

	fFileMenu->AddItem(new BMenuItem(B_TRANSLATE("Open Plot"), new BMessage(LOAD_PLOT),
		'L', B_COMMAND_KEY));
	fFileMenu->AddItem(new BMenuItem(B_TRANSLATE("Open Script"),
		new BMessage(GENERATE_PLOT), 'G', B_COMMAND_KEY));
	fFileMenu->AddSeparatorItem();
	BTranslationUtils::AddTranslationItems(fMenuSaveAs,
		B_TRANSLATOR_BITMAP);
	fFileMenu->AddItem(fMenuSaveAs);

	fMenuBar->AddItem(fFileMenu);

	fScriptMenu->AddItem(new BMenuItem(B_TRANSLATE("Generate..."),
		new BMessage(MSG_GENERATE_SCRIPT), 'P', B_COMMAND_KEY));
	fScriptMenu->AddSeparatorItem();
	fScriptMenu->AddItem(new BMenuItem(B_TRANSLATE("Save..."),
		new BMessage(MSG_SAVE_SCRIPT), 'S', B_COMMAND_KEY));

	fMenuBar->AddItem(fScriptMenu);

	fSettingsMenu->AddItem(new BMenuItem(B_TRANSLATE("About"),
		new BMessage(SHOW_ABOUT), 'A', B_COMMAND_KEY));

	fMenuBar->AddItem(fSettingsMenu);

	fPictureView = new BView(BRect(0,20,650,600), "picture_view",
		B_FOLLOW_NONE, B_WILL_DRAW);

	fScriptView = new BTextView("script_view", B_WILL_DRAW);

	static const float spacing = be_control_look->DefaultItemSpacing() / 2;
	fMainSplitView =
		BLayoutBuilder::Split<>(B_HORIZONTAL)
			.AddGroup(B_VERTICAL)
				.Add(fPictureView)
			.End()
			.AddGroup(B_VERTICAL, spacing / 2)
				.Add(fScriptView)
			.End()
		.View();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(fMenuBar)
		.Add(fMainSplitView);
}


bool
HaikuPlot::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
HaikuPlot::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped()) {
		entry_ref ref;
		if (msg->FindRef("refs", &ref) != B_OK)
			return;

		GeneratePlot(ref);
		return;
	}

	switch (msg->what) {
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
		case MSG_OUTPUT_TYPE:
		{
			if (!fSavePanel)
				_SaveAs(msg);

			break;
		}
		case MSG_SAVE_PANEL:
		{
			_SaveToFile(msg);

			break;
		}
		case B_REFS_RECEIVED:
		{
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
		case SHOW_ABOUT:
		{
			BAboutWindow *about = new BAboutWindow(B_TRANSLATE("HaikuPlot"),
				"application/x-vnd.haikuplot");

			about->AddDescription(B_TRANSLATE("A GUI interface to the popular command line graphing tool gnuplot"));
			about->AddCopyright(2015, "Vale Tolpegin");

			about->Show();

			break;
		}
		case MSG_GENERATE_SCRIPT:
		{
			SaveScript();

			break;
		}
		case MSG_SAVE_SCRIPT:
		{
			SaveScript();

			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
HaikuPlot::HandleNodeMonitoring(BMessage *msg)
{
	int32 opcode;
	if (msg->FindInt32("opcode", &opcode) != B_OK)
		return;

	switch (opcode) {
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


void
HaikuPlot::SaveScript(void)
{
	BFile file(&fRef, B_READ_WRITE | B_CREATE_FILE);

	if (file.InitCheck() != B_OK)
		return;

	BTranslationUtils::WriteStyledEditFile(fScriptView, &file, "");
}


void
HaikuPlot::GeneratePlot(const entry_ref &ref)
{
	PrepareNodeMonitoring(ref);

	BEntry entry(&ref, true);
	entry_ref real_ref;
	entry.GetRef(&real_ref);

	BFile file(&real_ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	fScriptView->SetText("");
	if (BTranslationUtils::GetStyledText(&file, fScriptView) != B_OK) {
		return;
	}

	fScriptView->SetText(B_TRANSLATE(fScriptView->Text()));

	BPath path(&real_ref);
	fScriptPath = BPath(path.Path());

	BString *command = new
		BString("gnuplot-x86 -e 'set output \"outpic.png\"' ");
	command->Append(path.Path());

	if (system(command->String()) == 0) {
		BEntry entry("outpic.png");
		entry_ref pic_ref;
		entry.GetRef(&pic_ref);

		LoadPlot(pic_ref);
	}

	_SaveSettings();
}


void
HaikuPlot::PrepareNodeMonitoring(const entry_ref &ref)
{
	BFile file(&ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	BEntry entry(&ref, true);
	entry.GetRef(&fRef);
	entry.GetNodeRef(&fNodeRef);

	watch_node(&fNodeRef, B_WATCH_STAT, this);
}


void
HaikuPlot::LoadPlot(const entry_ref &ref)
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

	fPictureView->SetViewBitmap(fPictureBitmap,
		B_FOLLOW_TOP | B_FOLLOW_LEFT, 0);
	fPictureView->ResizeTo(fPictureBitmap->Bounds().Width(),
		fPictureBitmap->Bounds().Height());
}


status_t
HaikuPlot::_SaveSettings()
{
	BPath p;
	BFile f;
	BMessage m(MSG_SAVE_SETTINGS);

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
		return B_ERROR;
	p.Append("HaikuPlot");

	f.SetTo(p.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (f.InitCheck() != B_OK)
		return B_ERROR;

	status_t status;
	if (strcmp(fScriptPath.Path(), "") != 0) {
		status = m.AddString("script_path", fScriptPath.Path());

		if (status != B_OK)
			return B_ERROR;
	}

	if (m.Flatten(&f) != B_OK)
		return B_ERROR;

	return B_OK;
}


status_t
HaikuPlot::_LoadSettings()
{
	BPath p;
	BFile f;
	BMessage m(MSG_SAVE_SETTINGS);

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
		return B_ERROR;
	p.Append("HaikuPlot");

	f.SetTo(p.Path(), B_READ_ONLY);
	if (f.InitCheck() != B_OK)
		return B_ERROR;

	if (m.Unflatten(&f) != B_OK)
		return B_ERROR;

	BString scriptPath;
	if (m.FindString("script_path", &scriptPath) != B_OK)
		return B_ERROR;

	BEntry entry(scriptPath.String());
	entry_ref ref;
	entry.GetRef(&ref);

	GeneratePlot(ref);

	return B_OK;
}


void
HaikuPlot::_SaveAs(BMessage* message)
{
	// Read the translator and output type the user chose
	translator_id outTranslator;
	uint32 outType;
	if (message->FindInt32(kTranslatorField,
			reinterpret_cast<int32 *>(&outTranslator)) != B_OK
		|| message->FindInt32(kTypeField,
			reinterpret_cast<int32 *>(&outType)) != B_OK)
		return;

	// Add the chosen translator and output type to the
	// message that the save panel will send back
	BMessage panelMsg(MSG_SAVE_PANEL);
	panelMsg.AddInt32(kTranslatorField, outTranslator);
	panelMsg.AddInt32(kTypeField, outType);

	// Create save panel and show it
	BMessenger target(this);
	fSavePanel = new (std::nothrow) BFilePanel(B_SAVE_PANEL,
		&target, NULL, 0, false, &panelMsg);
	if (!fSavePanel)
		return;

	fSavePanel->Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
	fSavePanel->Show();
}


void
HaikuPlot::_SaveToFile(BMessage* message)
{
	// Read in where the file should be saved
	entry_ref dirRef;
	if (message->FindRef("directory", &dirRef) != B_OK)
		return;

	const char* filename;
	if (message->FindString("name", &filename) != B_OK)
		return;

	// Read in the translator and type to be used
	// to save the output image
	translator_id outTranslator;
	uint32 outType;
	if (message->FindInt32(kTranslatorField,
			reinterpret_cast<int32 *>(&outTranslator)) != B_OK
		|| message->FindInt32(kTypeField,
			reinterpret_cast<int32 *>(&outType)) != B_OK)
		return;

	// Find the translator_format information needed to
	// write a MIME attribute for the image file
	BTranslatorRoster* roster = BTranslatorRoster::Default();
	const translation_format* outFormat = NULL;
	int32 outCount = 0;
	if (roster->GetOutputFormats(outTranslator, &outFormat, &outCount) != B_OK
		|| outCount < 1)
		return;

	int32 i;
	for (i = 0; i < outCount; i++) {
		if (outFormat[i].group == B_TRANSLATOR_BITMAP && outFormat[i].type
				== outType)
			break;
	}
	if (i == outCount)
		return;

		// Write out the image file
	BDirectory dir(&dirRef);
	SaveToFile(&dir, filename, NULL, &outFormat[i]);
}


void
HaikuPlot::SaveToFile(BDirectory* dir, const char* name,
	BBitmap* bitmap, const translation_format* format)
{
	if (bitmap == NULL) {
		// If no bitmap is supplied, write out the whole image
		bitmap = fPictureBitmap;
	}

	BBitmapStream stream(bitmap);

	bool loop = true;
	while (loop) {
		BTranslatorRoster* roster = BTranslatorRoster::Default();
		if (!roster)
			break;
		// write data
		BFile file(dir, name, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		if (file.InitCheck() != B_OK)
			break;
		if (roster->Translate(&stream, NULL, NULL, &file, format->type) < B_OK)
			break;
		// set mime type
		BNodeInfo info(&file);
		if (info.InitCheck() == B_OK)
			info.SetType(format->MIME);

		loop = false;
			// break out of loop gracefully (indicates no errors)
	}

	stream.DetachBitmap(&bitmap);
		// Don't allow the bitmap to be deleted, this is
		// especially important when using fBitmap as the bitmap
}
