// Created on: 1995-05-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : ChFiDS_ElSpine
//purpose  : 
//=======================================================================
ChFiDS_ElSpine::ChFiDS_ElSpine():periodic(0)
{
  pfirstsav = Precision::Infinite();
  plastsav  = Precision::Infinite();
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
//function : GetSavedFirstParameter
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::GetSavedFirstParameter() const
{
  return pfirstsav;
}

//=======================================================================
//function : GetSavedLastParameter
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_ElSpine::GetSavedLastParameter() const
{
  return plastsav;
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

Standard_Integer ChFiDS_ElSpine::NbIntervals(const GeomAbs_Shape S) const
{
  return curve.NbIntervals(S);
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) const
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
  if(!periodic) throw Standard_Failure("ElSpine non periodique");
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
//function : SaveFirstParameter
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SaveFirstParameter()
{
  pfirstsav = pfirst;
}

//=======================================================================
//function : SaveLastParameter
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SaveLastParameter()
{
  plastsav = plast;
}


//=======================================================================
//function : SetOrigin
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::SetOrigin(const Standard_Real O)
{
  if(!periodic) throw Standard_Failure("Elspine non periodique");
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
//function : AddVertexWithTangent
//purpose  : 
//=======================================================================

void ChFiDS_ElSpine::AddVertexWithTangent(const gp_Ax1& anAx1)
{
  VerticesWithTangents.Append(anAx1);
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
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_ElSpine::NbVertices() const
{
  return VerticesWithTangents.Length();
}

//=======================================================================
//function : VertexWithTangent
//purpose  : 
//=======================================================================

const gp_Ax1& ChFiDS_ElSpine::VertexWithTangent(const Standard_Integer Index) const
{
  return VerticesWithTangents(Index);
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
