// OCC_2dApp.cpp: implementation of the OCC_2dApp class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_2dApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_2dApp::OCC_2dApp()
{
	try
    {myGraphicDevice = new WNT_GraphicDevice();}
	catch(Standard_Failure)
	{
		AfxMessageBox("Fatal error during graphic initialization",MB_ICONSTOP);
		ExitProcess(1);
	}
}

