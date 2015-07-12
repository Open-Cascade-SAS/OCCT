// Created on: 1992-08-27
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge2d.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <HLRBRep.hxx>
#include <HLRBRep_Curve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================
TopoDS_Edge HLRBRep::MakeEdge (const HLRBRep_Curve& ec,
                               const Standard_Real U1,
                               const Standard_Real U2)
{
  TopoDS_Edge Edg;
  const Standard_Real sta = ec.Parameter2d(U1);
  const Standard_Real end = ec.Parameter2d(U2);

  switch (ec.GetType())
  {
  case GeomAbs_Line:
    Edg = BRepLib_MakeEdge2d(ec.Line(),sta,end);
    break;
    
  case GeomAbs_Circle:
    Edg = BRepLib_MakeEdge2d(ec.Circle(),sta,end);
    break;
    
  case GeomAbs_Ellipse:
    Edg = BRepLib_MakeEdge2d(ec.Ellipse(),sta,end);
    break;
    
  case GeomAbs_Hyperbola:
    Edg = BRepLib_MakeEdge2d(ec.Hyperbola(),sta,end);
    break;
    
  case GeomAbs_Parabola:
    Edg = BRepLib_MakeEdge2d(ec.Parabola(),sta,end);
    break;
    
  case GeomAbs_BezierCurve: {
    TColgp_Array1OfPnt2d Poles(1,ec.NbPoles());
    Handle(Geom2d_BezierCurve) ec2d;
    if (ec.IsRational()) {
      TColStd_Array1OfReal Weights(1,ec.NbPoles());
      ec.PolesAndWeights(Poles,Weights);
      ec2d = new Geom2d_BezierCurve(Poles,Weights);
    }
    else {
      ec.Poles(Poles);
      ec2d = new Geom2d_BezierCurve(Poles);
    }
    BRepLib_MakeEdge2d mke2d(ec2d,sta,end);
    if (mke2d.IsDone())
      Edg = mke2d.Edge();
    break;
  }
    
  case GeomAbs_BSplineCurve: {
    Handle(Geom2d_BSplineCurve) ec2d;
    GeomAdaptor_Curve GAcurve = ec.GetCurve().Curve();
    TopoDS_Edge anEdge = ec.GetCurve().Edge();
    Standard_Real fpar, lpar;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
    if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
      aCurve = (Handle(Geom_TrimmedCurve)::DownCast(aCurve))->BasisCurve();
    Handle(Geom_BSplineCurve) BSplCurve (Handle(Geom_BSplineCurve)::DownCast(aCurve));
    Handle(Geom_BSplineCurve) theCurve = Handle(Geom_BSplineCurve)::DownCast(BSplCurve->Copy());
    if (theCurve->IsPeriodic() && !GAcurve.IsClosed())
    {
      theCurve->Segment(sta, end);
      TColgp_Array1OfPnt2d    Poles(1, theCurve->NbPoles());
      TColStd_Array1OfReal    knots(1, theCurve->NbKnots());
      TColStd_Array1OfInteger mults(1, theCurve->NbKnots());
      //-- ec.KnotsAndMultiplicities(knots,mults);
      theCurve->Knots(knots);
      theCurve->Multiplicities(mults);
      if (theCurve->IsRational()) {
        TColStd_Array1OfReal Weights(1, theCurve->NbPoles());
        ec.PolesAndWeights(theCurve, Poles, Weights);
        ec2d = new Geom2d_BSplineCurve(Poles, Weights, knots, mults,
                                       theCurve->Degree(), theCurve->IsPeriodic());
      }
      else {
        ec.Poles(theCurve, Poles);
        ec2d = new Geom2d_BSplineCurve(Poles, knots, mults,
                                       theCurve->Degree(), theCurve->IsPeriodic());
      }
    }
    else
    {
      TColgp_Array1OfPnt2d    Poles(1,ec.NbPoles());
      TColStd_Array1OfReal    knots(1,ec.NbKnots());
      TColStd_Array1OfInteger mults(1,ec.NbKnots());
      //-- ec.KnotsAndMultiplicities(knots,mults);
      ec.Knots(knots);
      ec.Multiplicities(mults);
      if (ec.IsRational()) {
        TColStd_Array1OfReal Weights(1,ec.NbPoles());
        ec.PolesAndWeights(Poles,Weights);
        ec2d = new Geom2d_BSplineCurve(Poles,Weights,knots,mults,ec.Degree(),ec.IsPeriodic());
      }
      else {
        ec.Poles(Poles);
        ec2d = new Geom2d_BSplineCurve(Poles,knots,mults,ec.Degree(),ec.IsPeriodic());
      }
    }
    BRepLib_MakeEdge2d mke2d(ec2d, sta, end);
    if (mke2d.IsDone())
      Edg = mke2d.Edge();
    break;
  }
  default: {
    const Standard_Integer nbPnt = 15;
    TColgp_Array1OfPnt2d    Poles(1,nbPnt);
    TColStd_Array1OfReal    knots(1,nbPnt);
    TColStd_Array1OfInteger mults(1,nbPnt);
    mults.Init(1);
    mults(1    ) = 2;
    mults(nbPnt) = 2;
    const Standard_Real step = (U2-U1)/(nbPnt-1);
    Standard_Real par3d = U1;
    for (Standard_Integer i = 1; i < nbPnt; i++) {
      Poles(i) = ec.Value(par3d);
      knots(i) = par3d;
      par3d += step;
    }
    Poles(nbPnt) = ec.Value(U2);
    knots(nbPnt) = U2;
    
    Handle(Geom2d_BSplineCurve) ec2d = new Geom2d_BSplineCurve(Poles,knots,mults,1);
    BRepLib_MakeEdge2d mke2d(ec2d,sta,end);
    if (mke2d.IsDone())
      Edg = mke2d.Edge();
  }
  }
  return Edg;
}

//=======================================================================
//function : MakeEdge3d
//purpose  : 
//=======================================================================

TopoDS_Edge HLRBRep::MakeEdge3d(const HLRBRep_Curve& ec,
                                const Standard_Real U1,
                                const Standard_Real U2)
{
  TopoDS_Edge Edg;
  //const Standard_Real sta = ec.Parameter2d(U1);
  //const Standard_Real end = ec.Parameter2d(U2);

  TopoDS_Edge anEdge = ec.GetCurve().Edge();
  Standard_Real fpar, lpar;
  //BRep_Tool::Range(anEdge, fpar, lpar);
  //Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
  BRepAdaptor_Curve BAcurve(anEdge);
  fpar = BAcurve.FirstParameter();
  lpar = BAcurve.LastParameter();
  
  Edg = TopoDS::Edge(anEdge.EmptyCopied());
  Edg.Orientation(TopAbs_FORWARD);
  BRep_Builder BB;
  BB.Range(Edg, U1, U2);

  //Share vertices if possible
  TopoDS_Vertex V1, V2, V1new, V2new;
  TopExp::Vertices(anEdge, V1, V2);

  Standard_Real Tol = Precision::PConfusion();
  if (Abs(fpar - U1) <= Tol)
    V1new = V1;
  else
  {
    gp_Pnt aPnt = BAcurve.Value(U1);
    V1new = BRepLib_MakeVertex(aPnt);
  }
  if (Abs(lpar - U2) <= Tol)
    V2new = V2;
  else
  {
    gp_Pnt aPnt = BAcurve.Value(U2);
    V2new = BRepLib_MakeVertex(aPnt);
  }

  V1new.Orientation(TopAbs_FORWARD);
  V2new.Orientation(TopAbs_REVERSED);
  BB.Add(Edg, V1new);
  BB.Add(Edg, V2new);
  return Edg;
}

//=======================================================================
//function : PolyHLRAngleAndDeflection
//purpose  : 
//=======================================================================

void 
HLRBRep::PolyHLRAngleAndDeflection (const Standard_Real InAngl,
				    Standard_Real& OutAngl,
				    Standard_Real& OutDefl)
{
  static Standard_Real HAngMin =  1*M_PI/180;
  static Standard_Real HAngLim =  5*M_PI/180;
  static Standard_Real HAngMax = 35*M_PI/180;

  OutAngl = InAngl;
  if (OutAngl < HAngMin) OutAngl = HAngMin;
  if (OutAngl > HAngMax) OutAngl = HAngMax;
  OutAngl = HAngLim + sqrt((OutAngl - HAngMin) * (HAngMax - HAngLim) *
			   (HAngMax - HAngLim) / (HAngMax - HAngMin));
  OutDefl = OutAngl * OutAngl * 0.5;
}
