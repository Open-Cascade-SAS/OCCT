// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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

