
// CCSwisssys2.h : main header file for the CCSwisssys2 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

void SetupGradeCombobox(CComboBox &box);
#include "DatFile.h"
#include <map>
#include <sstream>
#include <algorithm>
#include <string>
#include <cctype>

// CCCSwisssys2App:
// See CCSwisssys2.cpp for the implementation of this class
//

class CCCSwisssys2App : public CWinApp
{
public:
	CCCSwisssys2App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCCSwisssys2App theApp;
