/*
 * Copyright 2012-2013 @marbocub <marbocub @ gmail com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include <SupportDefs.h>
#include <Application.h>
#include "PanedView.h"
#include "FilerView.h"
#include "BafxWindow.h"
#include "BafxMessage.h"


class BafxApp : public BApplication
{
public:
	BafxApp(void) : BApplication("application/x-vnd.marbocub-BafxApp")
	{
		mWindow = new BafxWindow(
			BRect(100,100,100+640,100+480),
			"BAfx App",
			B_DOCUMENT_WINDOW,
			0
		);
		mWindow->Show();
	}
private:
	virtual void ReadyToRun(void);
	BafxWindow*	mWindow;
};

void
BafxApp::ReadyToRun(void)
{
//	mWindow->PutText("Bafx started.\n\n");
}

int
main (void)
{
    new BafxApp();
    be_app->Run();
    delete be_app;
    bafx_free_keymap();
	
    return 0;
}
