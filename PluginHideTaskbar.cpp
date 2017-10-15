/*
  Copyright (C) 2016 Jeffrey Morley

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <Windows.h>
#include <cstdio>
#include "../../API/RainmeterAPI.h"
#include <shellapi.h>

// This isn't defined for me for some reason.
#ifndef ABM_SETSTATE
#define ABM_SETSTATE 0x0000000A
#define ABM_WINDOWPOSCHANGED 0x00000009
#endif

enum Instance
{
	BOTH,
	FIRST,
	SECOND
};

struct Measure
{
	Instance type;
	void* rm;
	Measure() : type(BOTH), rm() {}
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
	measure->rm = rm;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;
	LPCWSTR barInstance = RmReadString(rm, L"BARINSTANCE", L"Both");
	if (_wcsicmp(barInstance, L"BOTH") == 0)
	{
		measure->type = BOTH;
	}
	if (_wcsicmp(barInstance, L"FIRST") == 0)
	{
		measure->type = FIRST;
	}
	if (_wcsicmp(barInstance, L"SECOND") == 0)
	{
		measure->type = SECOND;
	}
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	double currentState = 0.0;
	APPBARDATA abd = { sizeof abd };
	UINT uState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
	HWND hwnd = FindWindow(L"Shell_traywnd", L"");

	switch (measure->type)
	{
	case BOTH:
		hwnd = FindWindow(L"Shell_traywnd", L"");
		break;
	case FIRST:
		hwnd = FindWindow(L"Shell_traywnd", L"");
		break;
	case SECOND:
		hwnd = FindWindow(L"Shell_SecondaryTrayWnd", L"");
		break;
	}
	
	currentState = (uState & ABS_AUTOHIDE) ? 2 : 1;
	return (!IsWindowVisible(hwnd)) ? -currentState : currentState;

}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
	Measure* measure = (Measure*)data;

	APPBARDATA abd = { sizeof abd };
	HWND hwnd = FindWindow(L"Shell_traywnd", L"");
	HWND hwnd2 = FindWindow(L"Shell_SecondaryTrayWnd", L"");
	
	if (_wcsicmp(args, L"HIDE") == 0)
	{
		if (measure->type == BOTH)
		{
			ShowWindow(hwnd, 0);
			ShowWindow(hwnd2, 0);
		}
		else if (measure->type == FIRST)
		{
			ShowWindow(hwnd, 0);
		}
		else if (measure->type == SECOND)
		{
			ShowWindow(hwnd2, 0);
		}
	}
	else if (_wcsicmp(args, L"SHOW") == 0)
	{
		if (measure->type == BOTH)
		{
			ShowWindow(hwnd, 8);
			ShowWindow(hwnd2, 8);
		}
		else if (measure->type == FIRST)
		{
			ShowWindow(hwnd, 8);
		}
		else if (measure->type == SECOND)
		{
			ShowWindow(hwnd2, 8);
		}
	}
	else if (_wcsicmp(args, L"AUTOHIDE") == 0)
	{
		abd.lParam = ABS_AUTOHIDE;
		SHAppBarMessage(ABM_SETSTATE, &abd);
		SHAppBarMessage(ABM_WINDOWPOSCHANGED, &abd);
	}
	else if (_wcsicmp(args, L"ALWAYSSHOW") == 0)
	{
		abd.lParam = ABS_ALWAYSONTOP;
		SHAppBarMessage(ABM_SETSTATE, &abd);
		SHAppBarMessage(ABM_WINDOWPOSCHANGED, &abd);
	}
	else
		RmLogF(measure->rm, LOG_WARNING, L"HideTaskbar: Invalid command: %s", args);

}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
