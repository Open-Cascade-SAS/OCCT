// Created on: 1999-12-30
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
