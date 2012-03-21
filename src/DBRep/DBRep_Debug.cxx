// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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



// command that can be called within the debugger

#include <DBRep.hxx>
#include <BRepTools.hxx>
#include <TopTools_LocationSet.hxx>
#include <TopoDS_Shape.hxx>


void DBRep_Set(char* name, const TopoDS_Shape& S)
{
 DBRep::Set(name,S);
}

void DBRep_Get(char* name, TopoDS_Shape& S)
{
  char n[255];
  strcpy(n,name);
  Standard_CString cs = (Standard_CString)n;
  S = DBRep::Get(cs);
  if (*name == '.')
    cout << "Name : " << n << endl;
}

void DBRep_Dump(const TopoDS_Shape& S)
{
  cout <<"\n\n";
  BRepTools::Dump(S,cout);
  cout <<endl;
}


void DBRep_DumpLoc(const TopLoc_Location& L)
{
  cout <<"\n\n";
  TopTools_LocationSet LS;
  LS.Add(L);
  LS.Dump(cout);
  cout <<endl;
}

