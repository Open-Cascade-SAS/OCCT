// OCC_3dApp.cpp: implementation of the OCC_3dApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OCC_3dApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_3dApp::OCC_3dApp()
{
	try
	{myGraphicDevice = new Graphic3d_WNTGraphicDevice();}
	catch(Standard_Failure)
	{
		AfxMessageBox("Fatal error during graphic initialization",MB_ICONSTOP);
		ExitProcess(1);
	}
}

OCC_3dApp::~OCC_3dApp()
{

}
