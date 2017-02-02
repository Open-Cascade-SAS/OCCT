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

// modif du 04/03/96 mjm
// modif dans IGESConvGeom_GeomBuilder
//#53 rln 24.12.98 CCI60005
//#59 rln 29.12.98 PRO17015
//%11 pdn 12.01.99 CTS22023: writing offset curves and detecting planar curves
//%12 pdn 13.01.99: CTS22023: cutting bspline curves (for Euclid3)
//:l7 abv 13.01.99: CTS22021: using QuasiAngular parametrisation for circles (more precise)
//:l5 abv 14.01.99: CTS22022-1: protection against exceptions in Segment()
//:q3 abv 17.03.99: PRO17828: using GeomConvert_ApproxCurve for converting circle to bspline

#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomEntity.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESConvGeom_GeomBuilder.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Precision.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

// Pour toutes les courbes infinies soit 
// Udeb <= -Precision::Infinite() et/ou Ufin >= Precision::Infinite()
// on choisit arbitrairement de les construire entre 
// Udeb = -Precision::Infinite() et Ufin = Precision::Infinite()
//=============================================================================
// GeomToIGES_GeomCurve
//=============================================================================
GeomToIGES_GeomCurve::GeomToIGES_GeomCurve()
:GeomToIGES_GeomEntity()
{
}


//=============================================================================
// GeomToIGES_GeomCurve
//=============================================================================

GeomToIGES_GeomCurve::GeomToIGES_GeomCurve
(const GeomToIGES_GeomEntity& GE)
:GeomToIGES_GeomEntity(GE)
{
}


//=============================================================================
// Transfer des Entites Curve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Curve)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
    DeclareAndCast(Geom_BoundedCurve, Bounded, start);
    res = TransferCurve(Bounded, Udeb, Ufin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_Conic))) {
    DeclareAndCast(Geom_Conic, Conic, start);
    res = TransferCurve(Conic, Udeb, Ufin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    DeclareAndCast(Geom_OffsetCurve, OffsetC, start);
    res = TransferCurve(OffsetC, Udeb, Ufin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_Line))) {
    DeclareAndCast(Geom_Line, Line, start);
    res = TransferCurve(Line, Udeb, Ufin);
  }  
  return res;
}
 


//=============================================================================
// Transfer des Entites BoundedCurve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_BoundedCurve)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    DeclareAndCast(Geom_BSplineCurve, Bspline, start);
    res = TransferCurve(Bspline, Udeb, Ufin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    DeclareAndCast(Geom_BezierCurve, Bezier, start);
    res = TransferCurve(Bezier, Udeb, Ufin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    DeclareAndCast(Geom_TrimmedCurve, Trimmed, start);
    res = TransferCurve(Trimmed, Udeb, Ufin);
  }
  
  return res;
}
 
//%11 pdn 12.01.98
//=============================================================================
// Detects if curve lies in some plane and returns normal
// IsPlanar
//=============================================================================
static gp_XYZ GetAnyNormal ( gp_XYZ orig )
{
  gp_XYZ Norm;
  if ( Abs ( orig.Z() ) < Precision::Confusion() )
    Norm.SetCoord ( 0, 0, 1 );
  else {
    Norm.SetCoord ( orig.Z(), 0, -orig.X() );
    Standard_Real nrm = Norm.Modulus();
    if ( nrm < Precision::Confusion() ) Norm.SetCoord ( 0, 0, 1 );
    else Norm = Norm / nrm;
  }
  return Norm;
}

//%11 pdn 12.01.98
//=============================================================================
// Detects if curve lies in some plane and returns normal
// IsPlanar
//=============================================================================
static Standard_Boolean ArePolesPlanar (const TColgp_Array1OfPnt& Poles,
				        gp_XYZ& Normal)
{
  if(Poles.Length()<3) {
    Normal = GetAnyNormal(Poles(1).XYZ()-Poles(2).XYZ());
    return Standard_True;
  }
  
  Normal = Poles(Poles.Length()).XYZ() ^ Poles(1).XYZ();
  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for ( i = 1; i < Poles.Length(); i++) 
    Normal += Poles ( i ).XYZ() ^ Poles ( i + 1 ).XYZ();

  Standard_Real tol = Precision::Confusion();
  Standard_Real nrm = Normal.Modulus();
  if ( nrm < tol ) {
    Normal.SetCoord ( 0, 0, 1 );
    return Standard_False;
  }
  Normal = Normal / nrm;

  Standard_Real scl = Poles(1).XYZ() * Normal;
  for ( i = 2; i <= Poles.Length(); i++ )
    if ( Abs ( Poles(i).XYZ() * Normal - scl ) > tol ) return Standard_False;
  return Standard_True;
}

//%11 pdn 12.01.98
//=============================================================================
// Detects if curve lies in some plane and returns normal
// IsPlanar
//=============================================================================
static Standard_Boolean IsPlanar(const Handle(Geom_Curve)& curve,
				 gp_XYZ& Normal)
{
  Normal.SetCoord(0,0,0);
  if ( curve->IsKind(STANDARD_TYPE(Geom_Line))) {
    DeclareAndCast(Geom_Line, Line, curve);
    Normal = GetAnyNormal(Line->Position().Direction().XYZ());
    return Standard_True;
  }
  if ( curve->IsKind(STANDARD_TYPE(Geom_Conic))) {
    DeclareAndCast(Geom_Conic, Conic, curve);
    Normal = Conic->Axis().Direction().XYZ();
    return Standard_True;
  }
  if ( curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    DeclareAndCast(Geom_TrimmedCurve, Trimmed, curve);
    return IsPlanar(Trimmed->BasisCurve(),Normal);
  }
  if ( curve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    DeclareAndCast(Geom_OffsetCurve, OffsetC, curve);
    return IsPlanar(OffsetC->BasisCurve(),Normal);
  }
  if ( curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    DeclareAndCast(Geom_BSplineCurve, BSpline, curve);
    TColgp_Array1OfPnt Poles(1,BSpline->NbPoles());
    BSpline->Poles(Poles);
    return ArePolesPlanar(Poles,Normal);
  }
  if ( curve->IsKind(STANDARD_TYPE(Geom_BezierCurve)))  {
    DeclareAndCast(Geom_BezierCurve, Bezier, curve);
    TColgp_Array1OfPnt Poles(1,Bezier->NbPoles());
    Bezier->Poles(Poles);
    return ArePolesPlanar(Poles,Normal);
  }
  return Standard_False;
}

//=============================================================================
// Transfer des Entites BSplineCurve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_BSplineCurve)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(Geom_BSplineCurve) mycurve;
  Standard_Boolean IPlan  = Standard_False;
  gp_XYZ Norm = gp_XYZ(0.,0.,1.);

// Si la courbe est periodique, on passe par une fonction pour recuperer tous
// les parametres necessaires a l`ecriture IGES.

  Standard_Boolean IPerio  = start->IsPeriodic();
 
  if (IPerio) {
    mycurve = Handle(Geom_BSplineCurve)::DownCast(start->Copy());
    mycurve->SetNotPeriodic();
  }
  else {
    mycurve = start;
  }

  Standard_Real Umin = Udeb;
  Standard_Real Umax = Ufin;
  if (Precision::IsNegativeInfinite(Udeb)) Umin = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Ufin)) Umax = Precision::Infinite();

  //%12 pdn: cut curve for E3
  Standard_Real First = mycurve->FirstParameter();
  Standard_Real Last = mycurve->LastParameter();
  //:l5 abv 14 Jan 99: protect against exceptions in Segment()
  if ( Umin - First < Precision::PConfusion() ) Umin = First;
  if ( Last - Umax  < Precision::PConfusion() ) Umax = Last;
  if ( Umin - First > Precision::PConfusion() || Last - Umax > Precision::PConfusion() ) {
    try {
      OCC_CATCH_SIGNALS
      Handle(Geom_BSplineCurve) bspl = Handle(Geom_BSplineCurve)::DownCast ( mycurve->Copy() ); 
      if ( ! bspl.IsNull() ) {
        if (Abs(Umax-Umin) > Precision::PConfusion())
          bspl->Segment ( Umin, Umax );
	mycurve = bspl;
      }
    }
    catch ( Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: GeomToIGES_GeomCurve: can't trim bspline" << endl;
      cout << "Warning: Exception in Segment(): " ;
      anException.Print(cout);
#endif
      (void)anException;
    }
  }

  Standard_Boolean IClos   = mycurve->IsClosed();
//  Standard_Boolean IRatio  = mycurve->IsRational();
  Standard_Boolean IPolyn  = !(mycurve->IsRational());
  //Standard_Boolean IPerio  = mycurve->IsPeriodic();
  Standard_Integer Deg     = mycurve->Degree();
  Standard_Integer Nbpoles = mycurve->NbPoles();
//  Standard_Integer Nbknots = mycurve->NbKnots();
  Standard_Integer Index = Nbpoles -1;


// Sequence des Knots de [-Deg, Index+1] dans IGESGeom.
// et de [1, Nbpoles+Deg+1] dans Geom
  Standard_Integer Knotindex;
  Standard_Real rtampon;
  TColStd_Array1OfReal K(1, Nbpoles+Deg+1);
  mycurve->KnotSequence(K);
  Standard_Integer itampon = -Deg;
  Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(-Deg,Index+1);
  for ( Knotindex=K.Lower(); Knotindex<=K.Upper(); Knotindex++) { 
    rtampon = K.Value(Knotindex);
    Knots->SetValue(itampon, rtampon);
    itampon++;
  }


// Tableau Weights de [0,Index]
  TColStd_Array1OfReal W(1, Nbpoles);
  mycurve->Weights(W);
  itampon = 0;
  Handle(TColStd_HArray1OfReal) Weights = new TColStd_HArray1OfReal(0,Index);
  for ( Knotindex=W.Lower(); Knotindex<=W.Upper(); Knotindex++) { 
    rtampon = W.Value(Knotindex);
    Weights->SetValue(itampon, rtampon);
    itampon++;
  }


// Tableau Poles de [0,Index]
  TColgp_Array1OfPnt P(1, Nbpoles);
  mycurve->Poles(P);
  Standard_Integer Poleindex;
  itampon = 0;
  Standard_Real Xpt, Ypt, Zpt;
  Handle(TColgp_HArray1OfXYZ) Poles = new TColgp_HArray1OfXYZ(0,Index);
  for ( Poleindex=P.Lower(); Poleindex<=P.Upper(); Poleindex++) { 
    gp_Pnt ptampon = P.Value(Poleindex);
    ptampon.Coord(Xpt, Ypt, Zpt);
    gp_XYZ xyztampon = gp_XYZ (Xpt/GetUnit(), Ypt/GetUnit(), Zpt/GetUnit());
    Poles->SetValue(itampon, xyztampon);
    itampon++;
  }

  // modif mjm du 9/10/97 : mise en place d'une protection.
//%12  Standard_Real First = mycurve->FirstParameter();
//%12  Standard_Real Last = mycurve->LastParameter();
//:l5  if (First >  Umin) Umin = First;
//:l5  if (Last  <  Umax) Umax = Last;
  Handle(IGESGeom_BSplineCurve) BSplineC = new IGESGeom_BSplineCurve; 
  //%11 pdn 13.01.98 computing planar flag and normal
  IPlan = IsPlanar(start,Norm);
  if ( Norm.Z() <0 ) Norm.Reverse();
  BSplineC->Init
    (Index, Deg, IPlan, IClos, IPolyn, IPerio, Knots, Weights, Poles, 
     Umin, Umax, Norm);
  
  res = BSplineC;
  return res;
}
 

//=============================================================================
// Transfer des Entites BezierCurve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_BezierCurve)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(Geom_TrimmedCurve) mycurve3d = 
	  new Geom_TrimmedCurve(start, Udeb, Ufin);
  Handle(Geom_BSplineCurve) Bspline = GeomConvert::CurveToBSplineCurve(mycurve3d,
    Convert_RationalC1);//#28 rln 19.10.98 UKI60155
  Standard_Real First = Bspline->FirstParameter();
  Standard_Real Last  = Bspline->LastParameter();
  res = TransferCurve(Bspline, First, Last);
  return res;
}
 

//=============================================================================
// Transfer des Entites TrimmedCurve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_TrimmedCurve)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(Geom_Curve) st = start->BasisCurve();
  if (st->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) { 
    DeclareAndCast(Geom_TrimmedCurve, Trimmed, st);
    Handle(Geom_Curve) st1 = Trimmed->BasisCurve();
    res = TransferCurve(st1, Udeb, Ufin);
  }
  
  res = TransferCurve(st, Udeb, Ufin);
  return res;
}
 


//=============================================================================
// Transfer des Entites Conic de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Conic)& start, const Standard_Real Udeb,
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  // conic curve : Ellipse, Circle, Hyperbola, Parabola.
  // All the conics are planar curves. An axis placement (two axis)
  // defines the local coordinate system of the conic.
  // The Location point, the XDirection and the YDirection of this
  // axis placement define the plane of the conic. 
  // The XDirection defines the origin of the curve's parametrization.
  // The Direction (main direction) of the axis placement is normal
  // to the plane of the conic.

  if (start->IsKind(STANDARD_TYPE(Geom_Circle))) {
    DeclareAndCast(Geom_Circle, Circle, start);
    res = TransferCurve(Circle, Udeb, Ufin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
    DeclareAndCast(Geom_Ellipse, Ellipse, start);
    res = TransferCurve(Ellipse, Udeb, Ufin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_Hyperbola))) {
    DeclareAndCast(Geom_Hyperbola, Hyperbola, start);
    res = TransferCurve(Hyperbola, Udeb, Ufin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_Parabola))) {
    DeclareAndCast(Geom_Parabola, Parabola, start);
    res = TransferCurve(Parabola, Udeb, Ufin);
  }
  
  return res;
}
 

//=============================================================================
// Transfer des Entites Circle de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Circle)& start , const Standard_Real Udeb, 
 const Standard_Real Ufin)
{

  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }
  Handle(IGESGeom_CircularArc) Circle = new IGESGeom_CircularArc;;
  IGESConvGeom_GeomBuilder Build;
  
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  if (Abs(Udeb) <= gp::Resolution()) U1 = 0.0;
  
// creation du "CircularArc" (#100)
// --------------------------------
  Standard_Real xloc,yloc,zloc;
  start->Circ().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc,yloc,zloc);
  gp_Ax3 Pos = gp_Ax3(start->Circ().Position());
// unusable  Standard_Boolean IsDirect = Pos.Direct();
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);

  Standard_Real Xc, Yc, Zc;
  Standard_Real Xs, Ys, Zs;
  Standard_Real Xe, Ye, Ze;
  //gka BUG 6542 1.09.04 BSpline curve was written in the IGES instead circle.
  gp_Pnt pfirst,plast;
  start->D0(U1,pfirst);
  if(Abs (Ufin - Udeb - 2 * M_PI) <= Precision::PConfusion())
    plast = pfirst;
  else
    start->D0(U2,plast);
  //
  Build.EvalXYZ(((start->Circ()).Location()).XYZ(), Xc, Yc, Zc);
  Build.EvalXYZ(pfirst.XYZ(), Xs, Ys, Zs);
  Build.EvalXYZ(plast.XYZ(), Xe, Ye, Ze);
  Circle->Init (Zc/GetUnit(), 
		gp_XY( Xc/GetUnit(), Yc/GetUnit()), 
		gp_XY( Xs/GetUnit(), Ys/GetUnit()), 
		gp_XY( Xe/GetUnit(), Ye/GetUnit()));
  
  // creation de la Trsf (#124)
  // il faut tenir compte de l`unite pour la matrice de transformation
  // (partie translation).

  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;  
    TMat = Build.MakeTransformation(GetUnit());
    Circle->InitTransf(TMat);
  }
  
  res = Circle;
  return res;
}
 

//=============================================================================
// Transfer des Entites Ellipse de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Ellipse)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{

  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  //#35 rln 22.10.98 BUC60391 face 9
  //Closed Conic Arc is incorrectly oriented when reading back to CAS.CADE
  if (Abs (Ufin - Udeb - 2 * M_PI) <= Precision::PConfusion()) {
    //#53 rln 24.12.98 CCI60005
    //Trimmed ellipse. To avoid huge weights in B-Spline first rotate it and then convert
    Handle(Geom_Ellipse) copystart = Handle(Geom_Ellipse)::DownCast (start->Copy());
    gp_Ax2 pos = copystart->Position();
    copystart->SetPosition (pos.Rotated (pos.Axis(), gp_Ax3 (pos).Direct() ? Udeb : 2 * M_PI - Udeb));
    Handle(Geom_BSplineCurve) Bspline;
    //:q3 abv 17 Mar 99: use GeomConvert_ApproxCurve for precise conversion
    const Handle(Geom_Curve)& aCopy = copystart; // to avoid ambiguity
    GeomConvert_ApproxCurve approx (aCopy, Precision::Approximation(), 
				    GeomAbs_C1, 100, 6 );
    if ( approx.HasResult() ) Bspline = approx.Curve();
    if ( Bspline.IsNull() ) 
      GeomConvert::CurveToBSplineCurve (copystart, Convert_QuasiAngular);
    TColStd_Array1OfReal Knots(1, Bspline->NbKnots());
    Bspline->Knots (Knots);
    BSplCLib::Reparametrize (Udeb, Udeb + 2 * M_PI, Knots);
    Bspline->SetKnots (Knots);
    return TransferCurve (Bspline, Udeb, Ufin);
  }
 
  Handle(IGESGeom_ConicArc) Conic = new IGESGeom_ConicArc;
  IGESConvGeom_GeomBuilder Build;
  Standard_Real U1 = Udeb; 
  Standard_Real U2 = Ufin;
  if (Abs(Udeb) <= gp::Resolution()) U1 = 0.0;

// creation du "ConicArc" (#104)
// -----------------------------

  Standard_Real xloc,yloc,zloc;
  start->Elips().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc, yloc, zloc);
  gp_Ax3 Pos = gp_Ax3(start->Elips().Position());
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);

  Standard_Real Xs, Ys, Zs;
  Standard_Real Xe, Ye, Ze;
  Build.EvalXYZ((start->Value(U1)).XYZ(), Xs, Ys, Zs);
  Build.EvalXYZ((start->Value(U2)).XYZ(), Xe, Ye, Ze);
  gp_Elips2d E2d = gp_Elips2d(gp_Ax22d(gp::Origin2d(), gp::DX2d(), gp::DY2d()),
			      (start->MajorRadius() / GetUnit()), 
			      (start->MinorRadius() / GetUnit()));
  Standard_Real A, B, C, D, E, F;
  E2d.Coefficients(A, C, B, D, E, F);//#59 rln 29.12.98 PRO17015 face 67
                                     //gp_Elips2d returns 0.5*K not K 

  Conic->Init (A, 2 * B, C, 2 * D, 2 * E, F, 0., //#59 rln
	       gp_XY( Xs/GetUnit(), Ys/GetUnit()), 
	       gp_XY( Xe/GetUnit(), Ye/GetUnit()));


  // creation de la Trsf (#124)
  // il faut tenir compte de l'unite pour la matrice de transformation
  // (partie translation).

  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;  
    TMat = Build.MakeTransformation(GetUnit());
    Conic->InitTransf(TMat);
  }
  res = Conic;
  return res;
}
 

//=============================================================================
// Transfer des Entites Hyperbola de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Hyperbola)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_ConicArc) Conic = new IGESGeom_ConicArc;
  IGESConvGeom_GeomBuilder Build;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  if (Precision::IsNegativeInfinite(Udeb)) U1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Ufin)) U2 = Precision::Infinite();

// creation du "ConicArc" (#104)
// -----------------------------
  Standard_Real xloc,yloc,zloc;
  start->Hypr().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc, yloc, zloc);
  gp_Ax3 Pos = gp_Ax3(start->Hypr().Position());
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);

  Standard_Real Xs, Ys, Zs;
  Standard_Real Xe, Ye, Ze;
  Build.EvalXYZ((start->Value(U1)).XYZ(), Xs, Ys, Zs);
  Build.EvalXYZ((start->Value(U2)).XYZ(), Xe, Ye, Ze);
  gp_Hypr2d H2d = gp_Hypr2d (gp_Ax22d(gp::Origin2d(), gp::DX2d(), gp::DY2d()),
			     (start->MajorRadius() / GetUnit()), 
			     (start->MinorRadius() / GetUnit()));
  Standard_Real A, B, C, D, E, F;
  H2d.Coefficients(A, C, B, D, E, F);

  Conic->Init (A, B, C, D, E, F, 0., 
	       gp_XY( Xs/GetUnit(), Ys/GetUnit()), 
	       gp_XY( Xe/GetUnit(), Ye/GetUnit()));


  // creation de la Trsf (#124)
  // il faut tenir compte de l'unite pour la matrice de transformation
  // (partie translation).

  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;  
    TMat = Build.MakeTransformation(GetUnit());
    Conic->InitTransf(TMat);
  }
  res = Conic;
  return res;
}
 

//=============================================================================
// Transfer des Entites Parabola de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Parabola)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_ConicArc) Conic = new IGESGeom_ConicArc;
  IGESConvGeom_GeomBuilder Build;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  if (Precision::IsNegativeInfinite(Udeb)) U1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Ufin)) U2 = Precision::Infinite();

// creation du "ConicArc" (#104)
// -----------------------------
  Standard_Real xloc,yloc,zloc;
  start->Parab().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc, yloc, zloc);
  gp_Ax3 Pos = gp_Ax3(start->Parab().Position());
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);

  Standard_Real Xs, Ys, Zs;
  Standard_Real Xe, Ye, Ze;
  Build.EvalXYZ((start->Value(U1)).XYZ(), Xs, Ys, Zs);
  Build.EvalXYZ((start->Value(U2)).XYZ(), Xe, Ye, Ze);
  gp_Parab2d P2d = gp_Parab2d (gp_Ax22d(gp::Origin2d(), gp::DX2d(), gp::DY2d()),
			       (start->Focal()*2.));
  Standard_Real A, B, C, D, E, F;
  P2d.Coefficients(A, C, B, D, E, F);

  Conic->Init (A, B, C, D, E, F, 0., 
	       gp_XY( Xs/GetUnit(), Ys/GetUnit()), 
	       gp_XY( Xe/GetUnit(), Ye/GetUnit()));


  // creation de la Trsf (#124)
  // il faut tenir compte de l'unite pour la matrice de transformation
  // (partie translation).

  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;  
    TMat = Build.MakeTransformation(GetUnit());
    Conic->InitTransf(TMat);
  }
  res = Conic;
  return res;
}
 

//=============================================================================
// Transfer des Entites Line de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_Line)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_Line) Line = new IGESGeom_Line;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  if (Precision::IsNegativeInfinite(Udeb)) U1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Ufin)) U2 = Precision::Infinite();

// creation du "Line" (#110)
// -------------------------

  Standard_Real X1,Y1,Z1,X2,Y2,Z2;
  start->Value(U1).Coord(X1,Y1,Z1);
  start->Value(U2).Coord(X2,Y2,Z2);

  Line->Init((gp_XYZ(X1/GetUnit(),Y1/GetUnit(),Z1/GetUnit())),
	     (gp_XYZ(X2/GetUnit(),Y2/GetUnit(),Z2/GetUnit())));
  res = Line;
  return res;
}
 

//=============================================================================
// Transfer des Entites OffsetCurve de Geom vers IGES
// TransferCurve
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomCurve::TransferCurve
(const Handle(Geom_OffsetCurve)& start, const Standard_Real Udeb,
 const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_OffsetCurve) OffsetC = new IGESGeom_OffsetCurve;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  if (Precision::IsNegativeInfinite(Udeb)) U1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Ufin)) U2 = Precision::Infinite();

  if (Interface_Static::IVal("write.iges.offset.mode") == 0)
  {
    res = TransferCurve(GeomConvert::CurveToBSplineCurve(start),U1,U2);
    return res;
  }

  Handle(Geom_Curve) Curve = start->BasisCurve();
  Standard_Real Deb = Curve->FirstParameter();
  Standard_Real Fin = Curve->LastParameter();
  //%11 pdn 12.01.98 offset curve should be planar
  gp_XYZ Normal;
  if (!IsPlanar(Curve,Normal)) {
    //%11 pdn 12.01.98 protection against exceptions
    try{
      OCC_CATCH_SIGNALS
      res = TransferCurve(GeomConvert::CurveToBSplineCurve(start),U1,U2);
      return res;
    }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "writing non-planar offset curve."<<endl;
      cout << "Warning: GeomConvert::CurveToBSplineCurve raised an exception: ";
      anException.Print(cout);
#endif
      (void)anException;
    }
  }

  Handle(IGESData_IGESEntity) BaseCurve = TransferCurve(Curve, Deb, Fin);
  Handle(IGESData_IGESEntity) voident;
  
  OffsetC->Init( BaseCurve, 1,voident, 0, 0, (start->Offset()/GetUnit()), 0.,
		(start->Offset()/GetUnit()), 0.,
		start->Direction().XYZ().Reversed(), U1, U2);  //%11 pdn 12.01.99 // valeur (0,0,1) pour l'instant   
  
  res = OffsetC;  
  return res;
}
