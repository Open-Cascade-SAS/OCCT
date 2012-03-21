// Created on: 1993-12-15
// Created by: Christophe MARION
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


#include <BRepExtrema_ExtPC.hxx>

#include <BRep_Tool.hxx>
//#include <StdFail_NotDone.hxx>
//#include <Standard_Failure.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>


//=======================================================================
//function : BRepExtrema_ExtPC
//purpose  : 
//=======================================================================

BRepExtrema_ExtPC::BRepExtrema_ExtPC(const TopoDS_Vertex& V, const TopoDS_Edge& E)
{
  Initialize(E);
  Perform(V);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::Initialize(const TopoDS_Edge& E)
{
  Standard_Real U1,U2;
  BRepAdaptor_Curve Curv(E);
  myHC = new BRepAdaptor_HCurve(Curv);
  BRep_Tool::Range(E,U1,U2);
  myExtPC.Initialize(myHC->Curve(),U1,U2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::Perform(const TopoDS_Vertex& V)
{
  gp_Pnt P = BRep_Tool::Pnt(V);
  myExtPC.Perform(P);
}
