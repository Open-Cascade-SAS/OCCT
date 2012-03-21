// Created on: 1999-03-09
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <ShapeCustom_TrsfModification.ixx>
#include <BRep_TVertex.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>

//=======================================================================
//function : ShapeCustom_TrsfModification
//purpose  : 
//=======================================================================

ShapeCustom_TrsfModification::ShapeCustom_TrsfModification(const gp_Trsf& T):
       BRepTools_TrsfModification(T)
{
}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewSurface(const TopoDS_Face& F,
							   Handle(Geom_Surface)& S,
							   TopLoc_Location& L,
							   Standard_Real& Tol,
							   Standard_Boolean& RevWires,
							   Standard_Boolean& RevFace) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewSurface(F, S, L, Tol, RevWires, RevFace);
  Tol = (*((Handle(BRep_TFace)*)&F.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewCurve(const TopoDS_Edge& E,
							 Handle(Geom_Curve)& C,
							 TopLoc_Location& L,
							 Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewCurve (E, C, L, Tol);
  Tol = (*((Handle(BRep_TEdge)*)&E.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewPoint(const TopoDS_Vertex& V,
							 gp_Pnt& P,
							 Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewPoint (V, P, Tol);
  Tol = (*((Handle(BRep_TVertex)*)&V.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewCurve2d(const TopoDS_Edge& E,
							   const TopoDS_Face& F,
							   const TopoDS_Edge& NewE,
							   const TopoDS_Face& NewF,
							   Handle(Geom2d_Curve)& C,
							   Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewCurve2d (E, F, NewE, NewF, C, Tol);
  Tol = (*((Handle(BRep_TEdge)*)&E.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewParameter(const TopoDS_Vertex& V,
							     const TopoDS_Edge& E,
							     Standard_Real& P,
							     Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewParameter (V, E, P, Tol);
  Tol = (*((Handle(BRep_TVertex)*)&V.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

