// Created on: 1993-07-13
// Created by: Remi LEQUETTE
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



#include <BRepAdaptor_Curve2d.ixx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : BRepAdaptor_Curve2d
//purpose  : 
//=======================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d() 
{
}


//=======================================================================
//function : BRepAdaptor_Curve2d
//purpose  : 
//=======================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d(const TopoDS_Edge& E, 
					 const TopoDS_Face& F) 
{
  Initialize(E,F);
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void  BRepAdaptor_Curve2d::Initialize(const TopoDS_Edge& E, 
				      const TopoDS_Face& F)
{
  myEdge = E;
  myFace = F;
  Standard_Real pf,pl;
  const Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,F,pf,pl);
  Geom2dAdaptor_Curve::Load(PC,pf,pl);
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepAdaptor_Curve2d::Edge() const
{
  return myEdge;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepAdaptor_Curve2d::Face() const
{
  return myFace;
}


