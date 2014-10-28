// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _TopOpeBRepTool_STATE_HeaderFile
#define _TopOpeBRepTool_STATE_HeaderFile

#ifdef OCCT_DEBUG

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

#endif /* OCCT_DEBUG */

#endif /* _TopOpeBRepTool_STATE_HeaderFile */
