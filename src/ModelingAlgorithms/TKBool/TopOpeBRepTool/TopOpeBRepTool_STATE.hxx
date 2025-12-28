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

class TopOpeBRepTool_STATE
{

public:
  TopOpeBRepTool_STATE(const char* name, const bool b = false);
  void Set(const bool b);
  void Set(const TopAbs_State S, const bool b);
  void Set(const bool b, int n, char** a);
  bool Get(const TopAbs_State S);

  bool Get() { return myonetrue; }

  void Print();

private:
  bool myin, myout, myon, myunknown;
  bool myonetrue;
  char myname[100];
};

#endif /* OCCT_DEBUG */

#endif /* _TopOpeBRepTool_STATE_HeaderFile */
