// Created on: 1994-11-14
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepFill_MultiLine.hxx>

#include <BRepIntCurveSurface_Inter.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom2d_Line.hxx>
#include <GeomProjLib.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Standard_ConstructionError.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <ElCLib.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>

#ifdef DRAW
#include <DrawTrSurf.hxx>
static Standard_Boolean AffichCurve = Standard_False;
static Standard_Integer NbProj = 1;
#endif

//POP pour NT
#include <stdio.h>

//=======================================================================
//function : isIsoU
//purpose  : 
//=======================================================================

static Standard_Boolean isIsoU(const TopoDS_Face& Face,
  const TopoDS_Edge& Edge ) 
{
  Handle(Geom2d_Curve) C;
  Handle(Geom2d_Line) Li;
  Standard_Real f,l;

  C = BRep_Tool::CurveOnSurface(Edge,Face, f, l);
  if ( C.IsNull()) {
    throw Standard_ConstructionError("BRepFill_MultiLine : Edge without PCurve");
  }

  gp_Dir2d D = C->DN(f,1);

  if (Abs(D.Dot(gp::DX2d())) < Abs(D.Dot(gp::DY2d()))) 
    return Standard_True;
  else            
    return Standard_False;
}


//=======================================================================
//function : BRepFill_MultiLine
//purpose  : 
//=======================================================================

BRepFill_MultiLine::BRepFill_MultiLine()
{
  myNbPnt2d = 2;
  myNbPnt = 1;
}


//=======================================================================
//function : BRepFill_MultiLine
//purpose  : 
//=======================================================================

BRepFill_MultiLine::BRepFill_MultiLine(const TopoDS_Face&     Face1, 
  const TopoDS_Face&     Face2,
  const TopoDS_Edge&     Edge1,
  const TopoDS_Edge&     Edge2,
  const Standard_Boolean Inv1,
  const Standard_Boolean Inv2,
  const Handle(Geom2d_Curve)& Bissec)
  : myFace1(Face1 ),
  myFace2(Face2 ),
  myBis  (Bissec),
  myKPart(0)
{
  //
  const Standard_Real mult = 5.;
  const Standard_Real eps = mult * Precision::Confusion();
  //
  myNbPnt2d = 2;
  myNbPnt = 1;

  // eval if myedges are IsoU or not
  myIsoU1 = isIsoU(Face1, Edge1);
  myIsoU2 = isIsoU(Face2, Edge2);

  // eval myU1, myV1, myU2, myV2;
  Handle(Geom_Plane) RefPlane;
  Handle(Geom_Plane) BasisPlane = new Geom_Plane(0.,0.,1.,0.);
  TopLoc_Location L;

  TopExp_Explorer Exp;
  Standard_Real Umin = 0.,Vmin = 0.,Umax = 0.,Vmax = 0.,U,V;
  gp_Pnt2d P1,P2;
  gp_Vec DZ;
  gp_Pnt P;

  // Result on Face1
  Standard_Boolean First = Standard_True;
  for (Exp.Init(myFace1,TopAbs_EDGE);Exp.More(); Exp.Next()) {
    TopoDS_Edge CurEdge = TopoDS::Edge(Exp.Current());
    BRep_Tool::UVPoints(CurEdge,myFace1,P1,P2);
    if ( First) {
      First = Standard_False;
      Umin = Min(P1.X(),P2.X());
      Umax = Max(P1.X(),P2.X());

      Vmin = Min(P1.Y(),P2.Y());
      Vmax = Max(P1.Y(),P2.Y());
    }
    else {
      U    = Min(P1.X(),P2.X());
      Umin = Min(Umin,U);
      U    = Max(P1.X(),P2.X());
      Umax = Max(Umax,U);

      V    = Min(P1.Y(),P2.Y());
      Vmin = Min(Vmin,V);
      V    = Max(P1.Y(),P2.Y());
      Vmax = Max(Vmax,V);
    }
  }

  // return isos in their domain of restriction.
  Handle(Geom_Curve) UU1, UU2, VV1, VV2;
  Handle(Geom_Surface) S;
  S = BRep_Tool::Surface(myFace1,L);  
  if (!L.IsIdentity())
    S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

  if ( myIsoU1) {
    if (!BRep_Tool::Degenerated(Edge1) && !Inv1) {
      UU1 = S->UIso(Umin);
      GeomAdaptor_Curve Dummy(UU1);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU1 = S->UIso(Umax);
      }
    }
    else {
      UU1 = S->UIso(Umax);
      GeomAdaptor_Curve Dummy(UU1);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU1 = S->UIso(Umin);
      }
    }
    VV1 = S->VIso(Vmin);
  }
  else {
    if (!BRep_Tool::Degenerated(Edge1) && !Inv1) {
      UU1 = S->VIso(Vmin);    
      GeomAdaptor_Curve Dummy(UU1);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU1 = S->VIso(Vmax);
      }
    }
    else {
      UU1 = S->VIso(Vmax);
      GeomAdaptor_Curve Dummy(UU1);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU1 = S->VIso(Vmin);
      }
    }
    VV1 = S->UIso(Umin);
  }

  if ( myIsoU1) {
    Standard_Real dummyUmin = Umin, dummyUmax = Umax;
    Umin = Vmin; 
    Umax = Vmax;
    Vmin = dummyUmin;
    Vmax = dummyUmax;
  }

  // try duplication
  GeomAdaptor_Surface GAS1(S);
  GeomAbs_SurfaceType Type1 = GAS1.GetType();

  if ( UU1->IsPeriodic()) {
    ElCLib::AdjustPeriodic(UU1->FirstParameter(),
      UU1->LastParameter(),
      Precision::PConfusion(),
      Umin, Umax);
  }
  if ( VV1->IsPeriodic()) {
    ElCLib::AdjustPeriodic(VV1->FirstParameter(),
      VV1->LastParameter(),
      Precision::PConfusion(),
      Vmin, Vmax);
  }
  if (GAS1.GetType() == GeomAbs_Sphere) {
    if (myIsoU1)
      ElCLib::AdjustPeriodic(-M_PI/2.,M_PI/2.,
      Precision::PConfusion(),
      Umin, Umax);
    else
      ElCLib::AdjustPeriodic(-M_PI/2.,M_PI/2.,
      Precision::PConfusion(),
      Vmin, Vmax);
  }
  // end try duplication

  myU1 = Geom2dAdaptor_Curve(GeomProjLib::Curve2d(UU1, BasisPlane),
    Umin, Umax);

  UU1->D1(Umin, P, DZ);
  RefPlane = new Geom_Plane(gp_Ax3(P,DZ,gp::DZ()));

  myV1 = Geom2dAdaptor_Curve(GeomProjLib::Curve2d(VV1, RefPlane),
    Vmin, Vmax);


  First = Standard_True;
  for (Exp.Init(myFace2,TopAbs_EDGE);Exp.More(); Exp.Next()) {
    TopoDS_Edge CurEdge = TopoDS::Edge(Exp.Current());
    BRep_Tool::UVPoints(CurEdge,myFace2,P1,P2);
    if ( First) {
      First = Standard_False;
      Umin = Min(P1.X(),P2.X());
      Umax = Max(P1.X(),P2.X());

      Vmin = Min(P1.Y(),P2.Y());
      Vmax = Max(P1.Y(),P2.Y());
    }
    else {
      U    = Min(P1.X(),P2.X());
      Umin = Min(Umin,U);
      U    = Max(P1.X(),P2.X());
      Umax = Max(Umax,U);

      V    = Min(P1.Y(),P2.Y());
      Vmin = Min(Vmin,V);
      V    = Max(P1.Y(),P2.Y());
      Vmax = Max(Vmax,V);
    }
  }

  // return isos in their domain of restriction.
  S = BRep_Tool::Surface(myFace2,L);

  if (!L.IsIdentity())
    S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

  if ( myIsoU2) {
    if (!BRep_Tool::Degenerated(Edge2) && !Inv2) {
      UU2 = S->UIso(Umin);    
      GeomAdaptor_Curve Dummy(UU2);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU2 = S->UIso(Umax);
      }
    }
    else {
      UU2 = S->UIso(Umax);
      GeomAdaptor_Curve Dummy(UU2);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU2 = S->UIso(Umin);
      }
    }
    VV2 = S->VIso(Vmin);
  }
  else {
    if (!BRep_Tool::Degenerated(Edge2) && !Inv2) {
      UU2 = S->VIso(Vmin);
      GeomAdaptor_Curve Dummy(UU2);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU2 = S->VIso(Vmax);
      }
    }
    else {
      UU2 = S->VIso(Vmax);
      GeomAdaptor_Curve Dummy(UU2);
      if (Dummy.GetType() == GeomAbs_Circle && 
        Dummy.Circle().Radius() < eps) {
          UU2 = S->VIso(Vmin);
      }
    }
    VV2 = S->UIso(Umin);
  }

  if ( myIsoU2) {
    Standard_Real dummyUmin = Umin, dummyUmax = Umax;
    Umin = Vmin; 
    Umax = Vmax;
    Vmin = dummyUmin;
    Vmax = dummyUmax;
  }

  // try duplication
  GeomAdaptor_Surface GAS2(S);
  GeomAbs_SurfaceType Type2 = GAS2.GetType();

  if ( UU2->IsPeriodic()) {
    ElCLib::AdjustPeriodic(UU2->FirstParameter(),
      UU2->LastParameter(),
      Precision::PConfusion(),
      Umin, Umax);
  }
  if ( VV2->IsPeriodic()) {
    ElCLib::AdjustPeriodic(VV2->FirstParameter(),
      VV2->LastParameter(),
      Precision::PConfusion(),
      Vmin, Vmax);
  }
  if (GAS2.GetType() == GeomAbs_Sphere) {
    if (myIsoU2)
      ElCLib::AdjustPeriodic(-M_PI/2.,M_PI/2.,
      Precision::PConfusion(),
      Umin, Umax);
    else
      ElCLib::AdjustPeriodic(-M_PI/2.,M_PI/2.,
      Precision::PConfusion(),
      Vmin, Vmax);
  }
  // end try duplication

  myU2 = Geom2dAdaptor_Curve(GeomProjLib::Curve2d(UU2, BasisPlane),
    Umin, Umax);

  UU2->D1(Umin, P, DZ);
  RefPlane = new Geom_Plane(gp_Ax3(P,DZ,gp::DZ()));
  myV2 = Geom2dAdaptor_Curve(GeomProjLib::Curve2d(VV2, RefPlane),
    Vmin, Vmax);

  // eval if in a particular case.
  // Particular case if :
  //     1) - Straight Bissectrice
  //        - Bissectrice orthogonal to the base element.
  //        ==> Iso on 2 faces.
  //     2) - Straight Bissectrice
  //        - 2 surfaces are planes.
  myCont  = GeomAbs_C0;

  if ( myBis.GetType() == GeomAbs_Line) {
    Standard_Real DeltaU = myBis.LastParameter()  - myBis.FirstParameter();
    gp_Pnt2d aPnt1 = ValueOnF1(myBis.FirstParameter() + 0.1*DeltaU);
    gp_Pnt2d aPnt2 = ValueOnF1(myBis.FirstParameter() + 0.9*DeltaU);
    if ( myIsoU1) {
      if ( Abs(aPnt1.Y() - aPnt2.Y()) < eps)
        myKPart = 1;
    }
    else {
      if ( Abs(aPnt1.X() - aPnt2.X()) < eps)
        myKPart = 1;
    }

    if ( myKPart == 1)
      myCont = GeomAbs_G1;

    if ( (Type1 == GeomAbs_Plane) && (Type2 == GeomAbs_Plane)) {
      myKPart = 2;
    }
  }
}


//=======================================================================
//function : IsParticularCase
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLine::IsParticularCase() const 
{
  return ( myKPart != 0);
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void BRepFill_MultiLine::Curves(Handle(Geom_Curve)& Curve,
  Handle(Geom2d_Curve)& PCurve1,
  Handle(Geom2d_Curve)& PCurve2) const 
{
  if ( myKPart == 1) {
    gp_Pnt2d      P1,P2,PMil;
    Standard_Real f,l;

    P1   = ValueOnF1(myBis.FirstParameter());
    P2   = ValueOnF1(myBis.LastParameter());

    // find value of the with medium point 
    // the ends can be degenerated points.

    PMil = ValueOnF1(0.5*(myBis.FirstParameter() + myBis.LastParameter()));

    TopLoc_Location L;
    Handle(Geom_Surface) S = BRep_Tool::Surface(myFace1,L);
    if (!L.IsIdentity())
      S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

    Standard_Boolean Sens;
    if ( !myIsoU1) {
      Curve   = S->UIso(PMil.X());
      Sens    = P1.Y() < P2.Y();
      if ( Sens) 
        Curve   = new Geom_TrimmedCurve(Curve, P1.Y(), P2.Y(), Sens);
      else
        Curve   = new Geom_TrimmedCurve(Curve, P2.Y(), P1.Y(), Sens);

      f = Curve->FirstParameter();
      l = Curve->LastParameter();
      if ( Sens) 
        PCurve1 = new Geom2d_Line(gp_Pnt2d(PMil.X(),P1.Y() - f), gp::DY2d());
      else
        PCurve1 = new Geom2d_Line(gp_Pnt2d(PMil.X(),P1.Y() + f),-gp::DY2d());
      PCurve1 = new Geom2d_TrimmedCurve( PCurve1 ,f ,l);
    }
    else {
      Curve   = S->VIso(PMil.Y());
      Sens = P1.X() < P2.X();
      if (Sens)
        Curve   = new Geom_TrimmedCurve(Curve, P1.X(), P2.X(), Sens);
      else
        Curve   = new Geom_TrimmedCurve(Curve, P2.X(), P1.X(), Sens);

      f = Curve->FirstParameter();
      l = Curve->LastParameter();
      if ( Sens) 
        PCurve1 = new Geom2d_Line(gp_Pnt2d(P1.X() - f,PMil.Y()), gp::DX2d());
      else
        PCurve1 = new Geom2d_Line(gp_Pnt2d(P1.X() + f,PMil.Y()), -gp::DX2d());
      PCurve1 = new Geom2d_TrimmedCurve( PCurve1 ,f ,l);
    }

    P1   = ValueOnF2(myBis.FirstParameter());
    P2   = ValueOnF2(myBis.LastParameter());
    PMil = ValueOnF2(0.5*(myBis.FirstParameter() + myBis.LastParameter()));

    if (!myIsoU2) {
      Sens = P1.Y() < P2.Y();
      if ( Sens) 
        PCurve2 = new Geom2d_Line(gp_Pnt2d(PMil.X(),(P1.Y() - f)), gp::DY2d());
      else
        PCurve2 = new Geom2d_Line(gp_Pnt2d(PMil.X(),(P1.Y() + f)), -gp::DY2d());
    }
    else {
      Sens = P1.X() < P2.X();
      if ( Sens) 
        PCurve2 = new Geom2d_Line(gp_Pnt2d(P1.X() - f ,PMil.Y()), gp::DX2d());
      else
        PCurve2 = new Geom2d_Line(gp_Pnt2d(P1.X() + f ,PMil.Y()), -gp::DX2d());
    }
    PCurve2 = new Geom2d_TrimmedCurve( PCurve2 ,f ,l);
  }
  else if ( myKPart == 2) {
    TopLoc_Location L;

    Handle(Geom_Surface) S = BRep_Tool::Surface(myFace1,L);
    if (!L.IsIdentity())
      S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

    if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) 
      S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();

    Handle(Geom_Plane) Plane = Handle(Geom_Plane)::DownCast(S);
    // eval the 3d curve corresponding to the bissectrice.
    gp_Pnt2d P = myBis.Line().Location();
    gp_Dir2d D = myBis.Line().Direction();
    Handle(Geom_Line) Line = new Geom_Line(gp_Pnt(P.X(),P.Y(),0.),
      gp_Dir(D.X(),D.Y(),0.) );
    Handle(Geom_TrimmedCurve) TLine = 
      new Geom_TrimmedCurve(Line, myBis.FirstParameter(),
      myBis.LastParameter());
    Curve = GeomProjLib::ProjectOnPlane(TLine, Plane, 
      gp::DZ(), Standard_False);

#ifdef DRAW
    if ( AffichCurve) {
      char name[100];
      sprintf(name,"C2_%d",NbProj);
      DrawTrSurf::Set(name,TLine);
      sprintf(name,"C3_%d",NbProj);
      DrawTrSurf::Set(name,Curve);
      sprintf(name,"SS_%d",NbProj);
      DrawTrSurf::Set(name,Plane);
      NbProj++;
    }
#endif

    // eval PCurve1
    PCurve1 = GeomProjLib::Curve2d(Curve,Plane);

    // eval PCurve2
    S = BRep_Tool::Surface(myFace2,L);
    if (!L.IsIdentity())
      S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));
    if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) 
      S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
    Plane = Handle(Geom_Plane)::DownCast(S);
    PCurve2 = GeomProjLib::Curve2d(Curve,Plane);
  }
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real BRepFill_MultiLine::FirstParameter() const 
{
  return myBis.FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real BRepFill_MultiLine::LastParameter() const 
{
  return myBis.LastParameter();
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt BRepFill_MultiLine::Value(const Standard_Real U) const 
{
  Handle(Geom_Surface) S;
  TopLoc_Location L;

  S = BRep_Tool::Surface(myFace1,L);  

  gp_Pnt2d P2d = ValueOnF1(U);

  gp_Pnt P3d = S->Value(P2d.X(), P2d.Y());
  P3d.Transform(L.Transformation());

  return P3d;
}


//=======================================================================
//function : ValueOnFace
//purpose  : 
//=======================================================================

static gp_Pnt2d ValueOnFace(const Standard_Real        U,
  const Geom2dAdaptor_Curve& TheBis,
  const Geom2dAdaptor_Curve& TheU,
  const Geom2dAdaptor_Curve& TheV,
  const Standard_Boolean     IsIsoU)
{
  gp_Pnt2d P = TheBis.Value(U);

  Geom2dAPI_ProjectPointOnCurve Ext(P,TheU.Curve(),
    TheU.FirstParameter(),
    TheU.LastParameter());
#ifdef DRAW
  if (AffichCurve) {
    char* TheUname = "TheU";
    char* PP1name = "PP1";
    DrawTrSurf::Set(TheUname,TheU.Curve());
    DrawTrSurf::Set(PP1name,P);
    //    DrawTrSurf::Set("TheU",TheU.Curve());
    //    DrawTrSurf::Set("PP1",P);
  }
#endif
  //
  const Standard_Real mult = 5.;
  const Standard_Real eps = mult * Precision::Confusion();
  //
  Standard_Real UU =0., Dist = Precision::Infinite(), D1, D2;

  if ( Ext.NbPoints() != 0 ) {
    UU = Ext.LowerDistanceParameter();
    Dist = Ext.LowerDistance();
  }
  // Control with `ends` 
  D1 = P.Distance(TheU.Value(TheU.FirstParameter()));
  D2 = P.Distance(TheU.Value(TheU.LastParameter()));

  if (D1 < Dist || D2 < Dist || Abs(D1 - Dist) < eps || Abs(D2 - Dist) < eps) {
    if ( Abs( D1 - D2) < eps) {
      if ( TheU.GetType() == GeomAbs_Circle) {
        gp_Vec2d Axis = TheU.Circle().XAxis().Direction();
        gp_Vec2d D12d = TheBis.DN(TheBis.FirstParameter(),1);
        Standard_Real Ang = Axis.Angle(D12d);
        if ( !TheU.Circle().IsDirect()) Ang = -Ang;
        UU = ElCLib::InPeriod( Ang, TheU.FirstParameter(), 
          TheU.FirstParameter() + 2*M_PI);
        Dist = TheU.Circle().Radius();
      }
      else {
#ifdef OCCT_DEBUG
        cout << "MultiLine : D1 = D2 and the Curve is not a circle" << endl;
        cout << "  ---> ValueOnFace failed at parameter U = " << U << endl;
#endif
        throw Standard_ConstructionError("BRepFill_MultiLine: ValueOnFace");
      }
    }
    else if ( D1 < D2) {
      Dist = D1;
      UU   = TheU.FirstParameter();
    }
    else {
      Dist = D2;
      UU   = TheU.LastParameter();
    }
  }

  const Standard_Real Tol = Precision::Confusion();
  Standard_Real VV;

  gp_Pnt2d PF = TheV.Value(TheV.FirstParameter());
  gp_Pnt2d PL = TheV.Value(TheV.LastParameter());  

  if (Abs(Dist - Abs(PF.Y())) < Tol) {
    VV = TheV.FirstParameter();
  }
  else if (Abs(Dist - Abs(PL.Y())) < Tol) {
    VV = TheV.LastParameter();
  }  
  else {  
    // test if the curve is at the side `negative Y`.
    if ( Min( PF.Y(),PL.Y()) < -Tol)  Dist = -Dist;

    Handle(Geom2d_Line) Line 
      = new Geom2d_Line(gp_Pnt2d(0., Dist), gp::DX2d());

#ifdef DRAW
    if (AffichCurve) {
      static Standard_CString aTheV = "TheV" ;
      DrawTrSurf::Set(aTheV,TheV.Curve());
      static Standard_CString aLINF1 = "LINF1" ;
      DrawTrSurf::Set(aLINF1,Line);
    }
#endif    

    Geom2dAdaptor_Curve Cu1 = TheV;
    Geom2dAdaptor_Curve Cu2( Line);

    Standard_Real TolConf = 0.;

    Geom2dInt_GInter Intersector(Cu1,Cu2,TolConf,Tol);

    if ( !Intersector.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Intersector not done" << endl;
      cout << "  ---> ValueonFace failed at parameter U = " << U << endl;
#endif
      return gp_Pnt2d(0.,0.);
    }
    else {
      if ( Intersector.NbPoints() > 0) {
        VV = Intersector.Point(1).ParamOnFirst();
      }
      else if ( Intersector.NbSegments() > 0) {
        IntRes2d_IntersectionSegment Seg = Intersector.Segment(1);
        Standard_Real VS1 = Seg.FirstPoint().ParamOnFirst();
        Standard_Real VS2 = Seg.LastPoint().ParamOnFirst();
        gp_Pnt2d      PS1 = TheV.Value(VS1);
        gp_Pnt2d      PS2 = TheV.Value(VS2);
        Standard_Real Alp = (Dist - PS1.Y())/(PS2.Y() - PS1.Y());
        VV = Alp*(VS2 - VS1) + VS1;
      }
      else {
#ifdef OCCT_DEBUG
        cout << "Intersector done, but no points found" << endl;
        cout << "  ---> ValueonFace failed at parameter U = " << U << endl;
#endif
        if (Abs(Dist - PL.Y()) < Abs(Dist - PF.Y()))
          VV = TheV.LastParameter();
        else 
          VV = TheV.FirstParameter();
      }
    }
  }

  if ( IsIsoU) 
    return gp_Pnt2d(VV,UU);
  else
    return gp_Pnt2d(UU,VV);
}

//=======================================================================
//function : ValueOnF1
//purpose  : 
//=======================================================================

gp_Pnt2d BRepFill_MultiLine::ValueOnF1(const Standard_Real U) const 
{
  return ValueOnFace(U,myBis,myU1,myV1,myIsoU1);
}


//=======================================================================
//function : ValueOnF2
//purpose  : 
//=======================================================================

gp_Pnt2d BRepFill_MultiLine::ValueOnF2(const Standard_Real U) const 
{
  return ValueOnFace(U,myBis,myU2,myV2,myIsoU2);
}

//=======================================================================
//function : Value3dOnF1OnF2
//purpose  : 
//=======================================================================

void BRepFill_MultiLine::Value3dOnF1OnF2(const Standard_Real U,
  gp_Pnt&       P3d,
  gp_Pnt2d&     PF1,
  gp_Pnt2d&     PF2)
  const
{
  PF1 = ValueOnFace(U,myBis,myU1,myV1,myIsoU1);
  PF2 = ValueOnFace(U,myBis,myU2,myV2,myIsoU2);

  Handle(Geom_Surface) S;
  TopLoc_Location L;

  S = BRep_Tool::Surface(myFace1,L);  
  P3d = S->Value(PF1.X(), PF1.Y());
  P3d.Transform(L.Transformation());
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepFill_MultiLine::Continuity() const
{
  return myCont;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLine::Value(const Standard_Real   theT,
  NCollection_Array1<gp_Pnt2d>& thePnt2d,
  NCollection_Array1<gp_Pnt>&   thePnt) const
{
  thePnt(1)   = Value(theT);
  thePnt2d(1) = ValueOnF1(theT);
  thePnt2d(2) = ValueOnF2(theT);
  return Standard_True;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLine::D1(const Standard_Real   /*theT*/,
  NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
  NCollection_Array1<gp_Vec>&   /*theVec*/) const
{
  return Standard_False;
}
