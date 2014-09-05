// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <TestTopOpe_VarsTopo.hxx>
#include <TopOpeBRepTool.hxx>

VarsTopo::VarsTopo() : mytolarc(0.), mytoltan(0.)
{
  Init();
}

void VarsTopo::Init()
{
  myOCT = TopOpeBRepTool_APPROX;
  myC2D = Standard_True;
  mytol3xdef = mytol2xdef = mytol3x = mytol2x = 1.e-7;
  myforcetoli = Standard_False;
  myclear = Standard_True;
  myexecmode = 0;
  myverbmode = 0;
}

void VarsTopo::SetTolxDef()
{
  mytol3x = mytol3xdef; 
  mytol2x = mytol2xdef;
}

void VarsTopo::SetTolx(const Standard_Real tol3x, const Standard_Real tol2x)
{ 
  mytol3x = tol3x;
  mytol2x = tol2x;
}

void VarsTopo::GetTolx(Standard_Real& tol3x, Standard_Real& tol2x) const
{
  tol3x = mytol3x;
  tol2x = mytol2x;
}

void VarsTopo::SetToliDef()
{
  myforcetoli = Standard_False;
}

void VarsTopo::SetToli(const Standard_Real tolarc,const Standard_Real toltan)
{
  mytolarc = tolarc;
  mytoltan = toltan;
  myforcetoli = Standard_True;
}

void VarsTopo::GetToli(Standard_Real& tolarc, Standard_Real& toltan) const
{
  tolarc = mytolarc;
  toltan = mytoltan;
}

Standard_Boolean VarsTopo::GetForceToli() const
{
  return myforcetoli;
}

void VarsTopo::SetClear(const Standard_Boolean b)
{
  myclear = b;
}

Standard_Boolean VarsTopo::GetClear() const
{
  return myclear;
}

void VarsTopo::SetOCT(char* noct)
{
  if      (noct == NULL) return;
  else if (!strcmp(noct,"-a")) SetOCT(TopOpeBRepTool_APPROX);
  else if (!strcmp(noct,"-p")) SetOCT(TopOpeBRepTool_BSPLINE1);
  else if (!strcmp(noct,"-i")) SetOCT(TopOpeBRepTool_INTERPOL);
}

void VarsTopo::SetOCT(const TopOpeBRepTool_OutCurveType oct)
{
  myOCT = oct;
}

TopOpeBRepTool_OutCurveType VarsTopo::GetOCT() const
{
  return myOCT;
}

void VarsTopo::SetC2D(char* nc2d)
{
  if      (nc2d == NULL) return;  
  else if (!strcasecmp(nc2d,"-no2d")) SetC2D(Standard_True);
  else if (!strcasecmp(nc2d,"-2d"))   SetC2D(Standard_False);
}

void VarsTopo::SetC2D(const Standard_Boolean c2d)
{
  myC2D = c2d;
}

Standard_Boolean VarsTopo::GetC2D() const
{
  return myC2D;
}

void VarsTopo::DumpOCT(Standard_OStream& OS) const
{
  if (myOCT == TopOpeBRepTool_APPROX) 
    OS<<"section curves approximated"<<"\n";
  if (myOCT == TopOpeBRepTool_BSPLINE1) 
    OS<<"section curves discretizated"<<"\n";
}

void VarsTopo::DumpC2D(Standard_OStream& OS) const
{
  if (!myC2D) {
    OS<<"section without pcurve";  OS<<"\n";
  }
  else {
    OS<<"section with pcurve";  OS<<"\n";
  }
}

void VarsTopo::DumpTolx(Standard_OStream& OS) const
{
  OS<<"approximation tolerances : ";
  OS<<"(3d "<<mytol3x<<"), (2d "<<mytol2x<<")"<<"\n";
}

void VarsTopo::DumpToli(Standard_OStream& OS) const
{
  if (myforcetoli) {
    OS<<"intersection tolerances : ";
    OS<<"(tolarc "<<mytolarc<<"), (toltan "<<mytoltan<<")"<<"\n";
  }
  else {
    OS<<"intersection with shape tolerances"<<"\n";
  }
}

void VarsTopo::DumpClear(Standard_OStream& OS) const
{
  if (myclear) OS<<"clear ON"<<"\n";
  else         OS<<"clear OFF"<<"\n";
}

void VarsTopo::DumpMode(Standard_OStream& OS) const
{
  OS<<"execution mode : "<<myexecmode<<"\n";
}

void VarsTopo::DumpVerbose(Standard_OStream& OS) const
{
  OS<<"verbose mode : "<<myverbmode<<"\n";
}

void VarsTopo::Dump(Standard_OStream& OS) const 
{
  OS<<"# "; DumpOCT(OS);
  OS<<"# "; DumpC2D(OS);
  OS<<"# "; DumpTolx(OS);
  OS<<"# "; DumpToli(OS);
  OS<<"# "; DumpMode(OS);
  OS<<"# "; DumpVerbose(OS);
  OS<<"# "; DumpClear(OS);
}

void VarsTopo::SetMode(const Standard_Integer mode) {myexecmode = mode;}
Standard_Integer VarsTopo::GetMode() const {return myexecmode;}
void VarsTopo::SetVerbose(const Standard_Integer mode) {myverbmode = mode;}
Standard_Integer VarsTopo::GetVerbose() const {return myverbmode;}
