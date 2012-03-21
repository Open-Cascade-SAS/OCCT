// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#ifndef _TopOpeBRepTool_STATE_HeaderFile
#define _TopOpeBRepTool_STATE_HeaderFile

#ifdef DEB

#include <Standard_Type.hxx>
#include <TopAbs_State.hxx>

// -----------------------------------------------------------------------
// TopOpeBRepTool_STATE : class of 4 booleans matching TopAbs_State values
// -----------------------------------------------------------------------

class TopOpeBRepTool_STATE {

 public:
  TopOpeBRepTool_STATE(const char* name, 
		       const Standard_Boolean b = Standard_False);
  void Set(const Standard_Boolean b);
  void Set(const TopAbs_State S, const Standard_Boolean b);
  void Set(const Standard_Boolean b, Standard_Integer n, char** a);
  Standard_Boolean Get(const TopAbs_State S);
  Standard_Boolean Get() { return myonetrue; }
  void Print();

 private:
  Standard_Boolean myin,myout,myon,myunknown;
  Standard_Boolean myonetrue;
  char myname[100];

};

// #define DEB
#endif

// #define _TopOpeBRepTool_STATE_HeaderFile
#endif
