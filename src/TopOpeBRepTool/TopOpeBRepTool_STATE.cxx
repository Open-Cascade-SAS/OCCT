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


#ifdef DEB

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
  Standard_Boolean b;
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

// #define DEB
#endif
