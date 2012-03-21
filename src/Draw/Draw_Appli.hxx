// Created on: 1992-04-07
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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


// JR 21 Oct 1999 : Change for Draw_Init_Appli which is in main and is
//                  called from Draw ===> undefined symbol on UNIX
//                                   ===> duplication of code on NT :
//                  One argument added to DrawAppli : Draw_Init_Appli ===>
//                  Draw_Appli of Draw/TKDraw may call Draw_Init_Appli

#ifndef Draw_Appli_HeaderFile
#define Draw_Appli_HeaderFile


#include <Draw_Viewer.hxx>
#include <Draw.hxx>

typedef void (*FDraw_InitAppli)(Draw_Interpretor&);

#ifdef WNT
#include <windows.h>
//extern void Draw_Appli(HINSTANCE,HINSTANCE,LPSTR,int);
Standard_EXPORT void Draw_Appli(HINSTANCE,HINSTANCE,LPSTR,int,
                       const FDraw_InitAppli Draw_InitAppli);
#else
extern void Draw_Appli(Standard_Integer argc, char** argv,
                       const FDraw_InitAppli Draw_InitAppli);
#endif



#if defined(WNT) && !defined(HAVE_NO_DLL)
#ifndef __Draw_API
# ifdef __Draw_DLL
#  define __Draw_API __declspec ( dllexport )
# else
#  define __Draw_API __declspec ( dllimport )
# endif
#endif
#else
#  define __Draw_API  
#endif


#ifndef WNT
extern Draw_Viewer dout;
extern Standard_Boolean Draw_Batch;
#endif

#define atof(X) Draw::Atof(X)
#define atoi(X) Draw::Atoi(X)

class Draw_SaveAndRestore {

  public :

//    __Draw_API Draw_SaveAndRestore 
    Standard_EXPORT Draw_SaveAndRestore 
      (const char* name,
       Standard_Boolean (*test)(const Handle(Draw_Drawable3D)&),
       void (*save)(const Handle(Draw_Drawable3D)&, ostream&),
       Handle(Draw_Drawable3D) (*restore) (istream&),
       Standard_Boolean display = Standard_True);


  const char* Name() const {return myName;}
  Standard_Boolean Test(const Handle(Draw_Drawable3D)&d);
  void Save(const Handle(Draw_Drawable3D)& d, ostream& os) const;
  Handle(Draw_Drawable3D) Restore(istream&) const;
  Standard_Boolean Disp() const {return myDisplay;}
  Draw_SaveAndRestore* Next() {return myNext;}

  private :
    
    const char* myName;
    Standard_Boolean (*myTest)(const Handle(Draw_Drawable3D)&);
    void (*mySave)(const Handle(Draw_Drawable3D)&, ostream&);
    Handle(Draw_Drawable3D) (*myRestore) (istream&);
    Standard_Boolean myDisplay;
    Draw_SaveAndRestore* myNext;
    
};

#endif



