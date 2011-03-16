// OCC.cpp : Defines the entry point for the DLL application.
//
//__declspec(dllexport)

#include "stdafx.h"
#include "OCCViewer.h"
#pragma unmanaged
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	return TRUE;
}

