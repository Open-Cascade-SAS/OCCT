// File:	Draw_Main.hxx
// Created:	Thu Dec 30 10:40:39 1999
// Author:	data exchange team
//		<det@burnax.nnov.matra-dtv.fr>

// Defines common framework for declaration of main/WinMain functions 
// for executbales on UNIX and WNT that extends DRAW Test Harness.

// In order to create executable in DRAW environment, in the executable
// the following line should be added:
// DRAW_MAIN

#ifndef Draw_Main_HeaderFile
#define Draw_Main_HeaderFile

#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

#include <Draw_Appli.hxx>
#include <Standard_PCharacter.hxx>

typedef void (*FDraw_InitAppli)(Draw_Interpretor&);

#ifndef WNT
Standard_EXPORT Standard_Integer _main_ (Standard_Integer argc, 
					 Standard_PCharacter argv[], 
					 const FDraw_InitAppli Draw_InitAppli);
#else
#include <windows.h>
Standard_EXPORT Standard_Integer _WinMain_ (HINSTANCE hInstance, 
					    HINSTANCE hPrevinstance, 
					    LPSTR lpCmdLine, 
					    Standard_Integer nCmdShow,
					    const FDraw_InitAppli Draw_InitAppli);

Standard_EXPORT Standard_Integer _main_ (int argc, 
					 char* argv[], 
					 char* envp[], 
					 const FDraw_InitAppli Draw_InitAppli);
#endif

// Declarations of macros DRAW_MAIN to be used in executables instead of explicit main/WinMain
#ifndef WNT
// main()
#define DRAW_MAIN int main (Standard_Integer argc, char* argv[])\
{return _main_ (argc, argv, Draw_InitAppli);}
#else
// WinMain() and main()
#define DRAW_MAIN Standard_Integer PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpCmdLine, Standard_Integer nCmdShow)\
{return _WinMain_ (hInstance, hPrevinstance, lpCmdLine, nCmdShow, Draw_InitAppli);}\
\
int main (int argc, char* argv[], char* envp[])\
{return _main_ (argc, argv, envp, Draw_InitAppli);}  
#endif


#endif
