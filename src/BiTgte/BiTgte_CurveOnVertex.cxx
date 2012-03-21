// Created on: 1997-03-21
// Created by: Bruno DUMORTIER
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



#include <BiTgte_CurveOnVertex.ixx>

#include <BiTgte_HCurveOnVertex.hxx>
#include <BRep_Tool.hxx>
#include <Standard_NotImplemented.hxx>


//=======================================================================
//function : BiTgte_CurveOnVertex
//purpose  : 
//======================================================================

BiTgte_CurveOnVertex::BiTgte_CurveOnVertex()
{
}


//=======================================================================
//function : BiTgte_CurveOnVertex
//purpose  : 
//=======================================================================

BiTgte_CurveOnVertex::BiTgte_CurveOnVertex(const TopoDS_Edge&   EonF,
					   const TopoDS_Vertex& V    )
{
  Init(EonF,V);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::Init(const TopoDS_Edge&   EonF,
				const TopoDS_Vertex& V   )
{
  BRep_Tool::Range(EonF,myFirst,myLast);
  myPnt = BRep_Tool::Pnt(V);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::FirstParameter() const
{
  return myFirst;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::LastParameter() const
{
  return myLast;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

GeomAbs_Shape BiTgte_CurveOnVertex::Continuity() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return GeomAbs_C0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbIntervals(const GeomAbs_Shape S) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::Intervals(TColStd_Array1OfReal& T,
				     const GeomAbs_Shape S) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) BiTgte_CurveOnVertex::Trim(const Standard_Real First,
						  const Standard_Real Last,
						  const Standard_Real Tol)
const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  Handle(BiTgte_HCurveOnVertex) HC;
  return HC;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsClosed() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsPeriodic() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::Period() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0.;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Pnt BiTgte_CurveOnVertex::Value(const Standard_Real U) const
{
  return myPnt;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D0(const Standard_Real U,gp_Pnt& P) const
{
  P = myPnt;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D1(const Standard_Real U,gp_Pnt& P,gp_Vec& V) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D2(const Standard_Real U,gp_Pnt& P,
			      gp_Vec& V1,gp_Vec& V2) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D3(const Standard_Real U,gp_Pnt& P,
			      gp_Vec& V1,gp_Vec& V2,gp_Vec& V3) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Vec BiTgte_CurveOnVertex::DN(const Standard_Real U,
				const Standard_Integer N) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return gp_Vec();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::Resolution(const Standard_Real R3d) const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0.;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

GeomAbs_CurveType BiTgte_CurveOnVertex::GetType() const
{
  return GeomAbs_OtherCurve;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Lin BiTgte_CurveOnVertex::Line() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return gp_Lin();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Circ BiTgte_CurveOnVertex::Circle() const
{
  Standard_NoSuchObject::Raise("BiTgte_CurveOnVertex::Circle");
  return gp_Circ();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Elips BiTgte_CurveOnVertex::Ellipse() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return gp_Elips();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Hypr BiTgte_CurveOnVertex::Hyperbola() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return gp_Hypr();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Parab BiTgte_CurveOnVertex::Parabola() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return gp_Parab();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::Degree() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsRational() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbPoles() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbKnots() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) BiTgte_CurveOnVertex::Bezier() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  Handle(Geom_BezierCurve) B;
  return B;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) BiTgte_CurveOnVertex::BSpline() const
{
  Standard_NotImplemented::Raise("BiTgte_CurveOnVertex");
  Handle(Geom_BSplineCurve) B;
  return B;
}


