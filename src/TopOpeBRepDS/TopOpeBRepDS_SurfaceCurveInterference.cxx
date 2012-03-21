// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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



#include <TopOpeBRepDS_SurfaceCurveInterference.ixx>

#ifdef DEB
#include <TopOpeBRepDS_Dumper.hxx>
#endif

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference()
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const TopOpeBRepDS_Transition& T,
   const TopOpeBRepDS_Kind ST, 
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT, 
   const Standard_Integer G, 
   const Handle(Geom2d_Curve)& PC) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G),
  myPCurve(PC)
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const Handle(TopOpeBRepDS_Interference)& I) :
  TopOpeBRepDS_Interference(I)
{
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& TopOpeBRepDS_SurfaceCurveInterference::PCurve
       ()const 
{
  return myPCurve;
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_SurfaceCurveInterference::PCurve
  (const Handle(Geom2d_Curve)& PC)
{
  myPCurve = PC;
}


//=======================================================================
//function : DumpPCurve
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_SurfaceCurveInterference::DumpPCurve
  (Standard_OStream& OS,
   const Standard_Boolean compact)const
{
#ifdef DEB
  Dump(OS); OS<<endl;

  OS<<"PCurve ";
  if (!PCurve().IsNull()) TopOpeBRepDS_Dumper::Print(PCurve(),OS,compact);
  else OS<<" is null";
  OS<<endl;
#endif

  return OS;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_SurfaceCurveInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"SCI ";TopOpeBRepDS_Dumper::PrintType(myPCurve,OS);OS<<" ";
  TopOpeBRepDS_Interference::Dump(OS);
#endif

  return OS;
}
