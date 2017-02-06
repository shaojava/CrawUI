#pragma once

#define WM_SHOWCHROME WM_USER+3300
//wparam bool bshow;lparam not use

#define WM_MOVECHROME WM_USER+3301
//wparam H:x L:y;lparam H:w L:h

struct ChromeMouseMsg
{
	WPARAM wParam;
	LPARAM lParam;
};

enum COPYDATETYPE
{
	HOMEPAGE = 0,
	MOUSEMESSAGE = 1
};