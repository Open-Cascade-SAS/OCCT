// Created on: 1995-05-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <ChFiDS_ElSpine.ixx>
#include <Geom_BSplineCurve.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>


//=======================================================================
//function : ChFiDS_ElSpine
//purpose  : 
//=======================================================================

ChFiDS_ElSpine::ChFiDS_ElSpine():periodic(0)
{
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::FirstParameter() const
{
  return pfirst;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::LastParameter() const
{
  return plast;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ChFiDS_ElSpine::Continuity() const
{
  return curve.Continuity();
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_ElSpine::NbIntervals(const GeomAbs_Shape S) 
{
  return curve.NbIntervals(S);
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) 
{
  curve.Intervals(T,S);
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) ChFiDS_ElSpine::Trim(const Standard_Real First,
					    const Standard_Real Last,
					    const Standard_Real Tol) const
{
  return curve.Trim(First,Last,Tol);
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::Resolution(const Standard_Real R3d) const
{
  return curve.Resolution(R3d);
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

GeomAbs_CurveType ChFiDS_ElSpine::GetType() const
{
  return curve.GetType();
}


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_ElSpine::IsPeriodic() const
{
  return periodic;
}


//=======================================================================
//function : SetPeriodic
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetPeriodic(const Standard_Boolean I) 
{
  periodic = I;
  period = plast - pfirst;
}



//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::Period() const
{
  if(!periodic) Standard_Failure::Raise("ElSpine non periodique");
  return period;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt ChFiDS_ElSpine::Value(const Standard_Real AbsC) const
{
  return curve.Value(AbsC);
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::D0(const Standard_Real AbsC, gp_Pnt& P) const
{
  curve.D0(AbsC,P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::D1(const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1)
const
{
  curve.D1(AbsC,P,V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::D2(const Standard_Real AbsC, 
			gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{  
  curve.D2(AbsC,P,V1,V2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::D3(const Standard_Real AbsC, 
			gp_Pnt& P, gp_Vec& V1, gp_Vec& V2,  gp_Vec& V3) const
{  
  curve.D3(AbsC,P,V1,V2,V3);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::FirstParameter(const Standard_Real P)
{
  pfirst = P;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::LastParameter(const Standard_Real P)
{
  plast = P;
}


//=======================================================================
//function : SetOrigin
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetOrigin(const Standard_Real O)
{
  if(!periodic) Standard_Failure::Raise("Elspine non periodique");
  Handle(Geom_BSplineCurve) bs = Handle(Geom_BSplineCurve)::DownCast(curve.Curve());
  if(!bs.IsNull()) {
    bs->SetOrigin(O,Precision::PConfusion());
    curve.Load(bs);
  }
}

//=======================================================================
//function : SetFirstPointAndTgt
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetFirstPointAndTgt(const gp_Pnt& P,
					 const gp_Vec& T)
{
  ptfirst = P;
  tgfirst = T;
}

//=======================================================================
//function : SetLastPointAndTgt
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetLastPointAndTgt(const gp_Pnt& P,
					const gp_Vec& T)
{
  ptlast = P;
  tglast = T;
}

//=======================================================================
//function : FirstPointAndTgt
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::FirstPointAndTgt(gp_Pnt& P,
				      gp_Vec& T) const
{
  P = ptfirst;
  T = tgfirst;
}

//=======================================================================
//function : LastPointAndTgt
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::LastPointAndTgt(gp_Pnt& P,
				     gp_Vec& T) const
{
  P = ptlast;
  T = tglast;
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetCurve(const Handle(Geom_Curve)& C)
{
  curve.Load(C);
}

//=======================================================================
//function : Previous
//purpose  : 
//=======================================================================

const Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::Previous() const 
{
  return previous;
}


//=======================================================================
//function : ChangePrevious
//purpose  : 
//=======================================================================

Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::ChangePrevious()
{
  return previous;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

const Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::Next() const 
{
  return next;
}


//=======================================================================
//function : ChangeNext
//purpose  : 
//=======================================================================

Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::ChangeNext()
{
  return next;
}

//    --
//    --     The following methods must  be called when GetType returned
//    --     the corresponding type.
//    --     

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin ChFiDS_ElSpine::Line() const 
{
 return curve.Line();  
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ  ChFiDS_ElSpine::Circle() const 
{
 return curve.Circle();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips ChFiDS_ElSpine::Ellipse() const 
{
  return curve.Ellipse();
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr ChFiDS_ElSpine::Hyperbola() const 
{
  return curve.Hyperbola();  
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab ChFiDS_ElSpine::Parabola() const 
{
 return curve.Parabola();
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) ChFiDS_ElSpine::Bezier() const 
{
  return curve.Bezier();
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) ChFiDS_ElSpine::BSpline() const 
{
  return curve.BSpline();
}
