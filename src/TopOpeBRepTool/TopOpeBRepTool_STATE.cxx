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

#ifdef OCCT_DEBUG

#include <TopOpeBRepTool_STATE.hxx>

TopOpeBRepTool_STATE::TopOpeBRepTool_STATE
  (const char* name, const Standard_Boolean b) :
  myin(Standard_False),myout(Standard_False),
  myon(Standard_False),myunknown(Standard_False),
  myonetrue(Standard_False)
{ 
  strcpy(myname,name);
  Set(b);
}

void TopOpeBRepTool_STATE::Set(const Standard_Boolean b)
{
  Set(TopAbs_IN,b); 
  Set(TopAbs_OUT,b);
  Set(TopAbs_ON,b); 
  Set(TopAbs_UNKNOWN,b);
}

void TopOpeBRepTool_STATE::Set
  (const TopAbs_State S,const Standard_Boolean b)
{
  switch(S) {
  case TopAbs_IN : myin = b; break;
  case TopAbs_OUT : myout = b; break;
  case TopAbs_ON : myon = b; break;
  case TopAbs_UNKNOWN : myunknown = b; break;
  }
  myonetrue = myin || myout || myon || myunknown;
}

void TopOpeBRepTool_STATE::Set(const Standard_Boolean b,
			       Standard_Integer n, char** a)
{ 
  if (!n) Set(b);
  else {
    Set(Standard_False);
    for (Standard_Integer i=0; i < n; i++) {
      const char *p = a[i];
      if      ( !strcmp(p,"IN") )      Set(TopAbs_IN,b);
      else if ( !strcmp(p,"OUT") )     Set(TopAbs_OUT,b);
      else if ( !strcmp(p,"ON") )      Set(TopAbs_ON,b);
      else if ( !strcmp(p,"UNKNOWN") ) Set(TopAbs_UNKNOWN,b);
    }
    Print();
  }
}

Standard_Boolean TopOpeBRepTool_STATE::Get(const TopAbs_State S)
{
  Standard_Boolean b = Standard_False;
  switch(S) {
  case TopAbs_IN : b = myin; break;
  case TopAbs_OUT : b = myout; break;
  case TopAbs_ON : b = myon; break;
  case TopAbs_UNKNOWN : b = myunknown; break;
  }
  return b;
}

void TopOpeBRepTool_STATE::Print()
{
  cout<<myname<<" : ";
  cout<<"IN/OUT/ON/UNKNOWN = ";
  cout<<Get(TopAbs_IN)<<Get(TopAbs_OUT)<<Get(TopAbs_ON)<<Get(TopAbs_UNKNOWN);
  cout<<endl;
}

#endif
