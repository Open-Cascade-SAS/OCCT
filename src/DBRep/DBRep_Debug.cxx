// File:	DBRep_Debug.cxx
// Created:	Mon Jul 25 14:57:35 1994
// Author:	Remi LEQUETTE
//		<rle@bravox>


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

