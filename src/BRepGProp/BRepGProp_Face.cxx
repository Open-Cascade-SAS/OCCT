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


#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepGProp_Face.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

static const Standard_Real Epsilon1 = Epsilon(1.);

//=======================================================================
//function : UIntegrationOrder
//purpose  : 
//=======================================================================
Standard_Integer BRepGProp_Face::UIntegrationOrder() const {

  Standard_Integer Nu;
  switch (mySurface.GetType())
  {

  case GeomAbs_Plane :
    Nu =4;
    break;

  case GeomAbs_BezierSurface :
    {
      Nu = (*((Handle(Geom_BezierSurface)*)&((mySurface.Surface()).Surface())))->UDegree()+1;
      Nu = Max(4,Nu);
    }
    break;
  case GeomAbs_BSplineSurface :
    {
      Standard_Integer a = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->UDegree()+1;
      Standard_Integer b = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->NbUKnots()-1;
      Nu = Max(4,a*b);
    }
    break;

  default :
    Nu = 9;
    break;
  }
 return Max(8,2*Nu);
}

//=======================================================================
//function : VIntegrationOrder
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::VIntegrationOrder() const
{
 Standard_Integer Nv;
 switch (mySurface.GetType()) {

 case GeomAbs_Plane :
   Nv = 4;
   break;

 case GeomAbs_BezierSurface :
   {
   Nv = (*((Handle(Geom_BezierSurface)*)&((mySurface.Surface()).Surface())))->VDegree()+1;
   Nv = Max(4,Nv);
   }
   break;

 case GeomAbs_BSplineSurface :
   {
   Standard_Integer a = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->VDegree()+1;
   Standard_Integer b = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->NbVKnots()-1;
   Nv = Max(4,a*b);
   }
   break;

   default :
     Nv = 9;
   break;
 }
 return Max(8,2*Nv);
}

//=======================================================================
//function : IntegrationOrder
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::IntegrationOrder() const 
{
  Standard_Integer N;

  switch (myCurve.GetType()) {
    
  case GeomAbs_Line :
    N = 2;
    break;

  case GeomAbs_Circle :
  case GeomAbs_Ellipse :
  case GeomAbs_Hyperbola :
    N = 9;
    break;

  case GeomAbs_Parabola :
    N = 9;
    break;

  case GeomAbs_BezierCurve :
    {
      N = (*((Handle(Geom2d_BezierCurve)*)&(myCurve.Curve())))->Degree() + 1;
    }
    break;

  case GeomAbs_BSplineCurve :
    {
    Standard_Integer a = (*((Handle(Geom2d_BSplineCurve)*)&(myCurve.Curve())))->Degree() + 1;
    Standard_Integer b = (*((Handle(Geom2d_BSplineCurve)*)&(myCurve.Curve())))->NbKnots() - 1;
    N = a * b;
    }
    break;

    default :
      N = 9;
    break;
  }

  return Max(4,2*N);
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void BRepGProp_Face::Bounds(Standard_Real& U1,
			    Standard_Real& U2,
			    Standard_Real& V1,
			    Standard_Real& V2)const 
{
  U1 = mySurface.FirstUParameter();
  U2 = mySurface.LastUParameter();
  V1 = mySurface.FirstVParameter();
  V2 = mySurface.LastVParameter();
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepGProp_Face::Load(const TopoDS_Edge& E) 
{ 
  Standard_Real a,b;
  Handle(Geom2d_Curve) C = 
    BRep_Tool::CurveOnSurface(E, mySurface.Face(), a,b);
  if (E.Orientation() == TopAbs_REVERSED) { 
    Standard_Real x = a;
    a = C->ReversedParameter(b);
    b = C->ReversedParameter(x);
    C = C->Reversed();
  }
  myCurve.Load(C,a,b);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepGProp_Face::Load(const TopoDS_Face& F) 
{ 
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
  mySurface.Initialize(TopoDS::Face(aLocalShape));
//  mySurface.Initialize(TopoDS::Face(F.Oriented(TopAbs_FORWARD)));
  mySReverse = (F.Orientation() == TopAbs_REVERSED);
}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

void BRepGProp_Face::Normal (const Standard_Real  U,
			     const Standard_Real  V,
			           gp_Pnt        &P,
			           gp_Vec        &VNor) const 
{
  gp_Vec D1U,D1V;
  mySurface.D1(U,V,P,D1U,D1V);
  VNor = D1U.Crossed(D1V);
  if (mySReverse) VNor.Reverse();
  
}

//  APO 17.04.2002 (OCC104)
// This is functions that calculate coeff. to optimize "integration order".
//They had been produced experementally for some hard example.
static Standard_Real AS = -0.15, AL = -0.50, B = 1.0, C = 0.75, D = 0.25;
static inline Standard_Real SCoeff(const Standard_Real Eps){
  return Eps < 0.1? AS*(B+Log10(Eps)) + C: C;
}
static inline Standard_Real LCoeff(const Standard_Real Eps){
  return Eps < 0.1? AL*(B+Log10(Eps)) + D: D;
}

//=======================================================================
//function : SIntOrder
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::SIntOrder(const Standard_Real Eps) const
{
  Standard_Integer Nv, Nu;
  switch (mySurface.GetType()) {
  case GeomAbs_Plane:  
    Nu = 1; Nv = 1; break;
  case GeomAbs_Cylinder: 
    Nu = 2; Nv = 1; break;
  case GeomAbs_Cone: 
    Nu = 2; Nv = 1; break;
  case GeomAbs_Sphere: 
    Nu = 2; Nv = 2; break;
  case GeomAbs_Torus:
    Nu = 2; Nv = 2; break;
  case GeomAbs_BezierSurface: 
    Nv = (*((Handle(Geom_BezierSurface)*)&((mySurface.Surface()).Surface())))->VDegree();
    Nu = (*((Handle(Geom_BezierSurface)*)&((mySurface.Surface()).Surface())))->UDegree();
    break;
  case GeomAbs_BSplineSurface: 
    Nv = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->VDegree();
    Nu = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->UDegree();
    break;
  default:  
    Nu = 2; Nv = 2;  break;
  }
  return Min(RealToInt(Ceiling(SCoeff(Eps)*Max((Nu+1),(Nv+1)))), math::GaussPointsMax());
}

//=======================================================================
//function : SUIntSubs
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::SUIntSubs() const
{
  Standard_Integer N;
  switch (mySurface.GetType()) {
  case GeomAbs_Plane:  
    N = 2;  break;
  case GeomAbs_Cylinder: 
    N = 4;  break;
  case GeomAbs_Cone: 
    N = 4;  break;
  case GeomAbs_Sphere: 
    N = 4; break;
  case GeomAbs_Torus:
    N = 4; break;
  case GeomAbs_BezierSurface:  
    N = 2;  break;
  case GeomAbs_BSplineSurface: 
    N = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->NbUKnots();  break;
  default:  
    N = 2;  break;
  }
  return N - 1;
}

//=======================================================================
//function : SVIntSubs
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::SVIntSubs() const
{
  Standard_Integer N;
  switch (mySurface.GetType()) {
  case GeomAbs_Plane:  
    N = 2;  break;
  case GeomAbs_Cylinder: 
    N = 2;  break;
  case GeomAbs_Cone: 
    N = 2;  break;
  case GeomAbs_Sphere: 
    N = 3; break;
  case GeomAbs_Torus:
    N = 4; break;
  case GeomAbs_BezierSurface: 
    N = 2;  break;
  case GeomAbs_BSplineSurface: 
    N = (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->NbVKnots();
    break;
  default:  
    N = 2;  break;
  }
  return N - 1;
}

//=======================================================================
//function : UKnots
//purpose  : 
//=======================================================================

void BRepGProp_Face::UKnots(TColStd_Array1OfReal& Knots) const
{
  switch (mySurface.GetType()) {
  case GeomAbs_Plane:
    Knots(1) = mySurface.FirstUParameter();  Knots(2) = mySurface.LastUParameter();  
    break;
  case GeomAbs_Cylinder: 
  case GeomAbs_Cone: 
  case GeomAbs_Sphere: 
  case GeomAbs_Torus:
    Knots(1) = 0.0;  Knots(2) = M_PI*2.0/3.0;  Knots(3) = M_PI*4.0/3.0;  Knots(4) = M_PI*6.0/3.0;
    break;
  case GeomAbs_BSplineSurface: 
    (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->UKnots(Knots);
    break;
  default: 
    Knots(1) = mySurface.FirstUParameter();  Knots(2) = mySurface.LastUParameter();
    break;
  }  
}

//=======================================================================
//function : VKnots
//purpose  : 
//=======================================================================

void BRepGProp_Face::VKnots(TColStd_Array1OfReal& Knots) const
{
  switch (mySurface.GetType()) {
  case GeomAbs_Plane:
  case GeomAbs_Cylinder: 
  case GeomAbs_Cone: 
    Knots(1) = mySurface.FirstUParameter();  Knots(2) = mySurface.LastUParameter();  
    break;
  case GeomAbs_Sphere: 
    Knots(1) = -M_PI/2.0;  Knots(2) = 0.0;  Knots(3) = +M_PI/2.0;
    break;
  case GeomAbs_Torus:
    Knots(1) = 0.0;  Knots(2) = M_PI*2.0/3.0;  Knots(3) = M_PI*4.0/3.0;  Knots(4) = M_PI*6.0/3.0;
    break;
  case GeomAbs_BSplineSurface: 
    (*((Handle(Geom_BSplineSurface)*)&((mySurface.Surface()).Surface())))->VKnots(Knots);
    break;
  default: 
    Knots(1) = mySurface.FirstUParameter();  Knots(2) = mySurface.LastUParameter();
    break;
  }  
}

//=======================================================================
//function : LIntOrder
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::LIntOrder(const Standard_Real Eps) const
{
  Bnd_Box2d aBox;

  BndLib_Add2dCurve::Add(myCurve, 1.e-7, aBox);
  Standard_Real aXmin, aXmax, aYmin, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  Standard_Real aVmin = mySurface.FirstVParameter();
  Standard_Real aVmax = mySurface.LastVParameter();

  Standard_Real dv = (aVmax-aVmin);
  Standard_Real anR = (dv > Epsilon1 ? Min ((aYmax - aYmin) / dv, 1.) : 1.);

//  Standard_Integer anRInt = Max(RealToInt(Ceiling(SVIntSubs()*anR)), 2);
  Standard_Integer anRInt = RealToInt(Ceiling(SVIntSubs()*anR));
  Standard_Integer aLSubs = LIntSubs();


//  Standard_Real NL, NS = Max(SIntOrder(1.0)*anRInt/LIntSubs(), 1);
  Standard_Real NL, NS = Max(SIntOrder(1.)*anRInt/aLSubs, 1);
  switch (myCurve.GetType()) {
  case GeomAbs_Line:  
    NL = 1;  break;
  case GeomAbs_Circle:
    NL = 2 * 3;  break; //correction for the spans of converted curve
  case GeomAbs_Ellipse:
    NL = 2 * 3;  break; //
  case GeomAbs_Parabola:  
    NL = 2 * 3;  break;
  case GeomAbs_Hyperbola: 
    NL = 3 * 3;  break;
  case GeomAbs_BezierCurve: 
    NL = (*((Handle(Geom2d_BezierCurve)*)&(myCurve.Curve())))->Degree();
    break;
  case GeomAbs_BSplineCurve: 
    NL = (*((Handle(Geom2d_BSplineCurve)*)&(myCurve.Curve())))->Degree();
    break;
  default:  
    NL = 3 * 3;  break;
  }

  NL = Max(NL, NS);

  Standard_Integer nn = 
    RealToInt (aLSubs <= 4 ? Ceiling(LCoeff(Eps)*(NL+1)) : NL+1);

  //return Min(RealToInt(Ceiling(LCoeff(Eps)*(NL+1)*NS)), math::GaussPointsMax());
  return Min(nn, math::GaussPointsMax());
}

//=======================================================================
//function : LIntSubs
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_Face::LIntSubs() const
{
  Standard_Integer N;
  switch (myCurve.GetType()) {
  case GeomAbs_Line:  
    N = 2;  break;
  case GeomAbs_Circle:
  case GeomAbs_Ellipse:
    N = 4;  break;
  case GeomAbs_Parabola:
  case GeomAbs_Hyperbola:
    N = 2;  break;
  case GeomAbs_BSplineCurve: 
    N = (*((Handle(Geom2d_BSplineCurve)*)&(myCurve.Curve())))->NbKnots();
    break;
  default:  
    N = 2;  break;
  }
  return N - 1;
}

//=======================================================================
//function : LKnots
//purpose  : 
//=======================================================================

void BRepGProp_Face::LKnots(TColStd_Array1OfReal& Knots) const
{
  switch (myCurve.GetType()) {
  case GeomAbs_Line:  
    Knots(1) = myCurve.FirstParameter();  Knots(2) = myCurve.LastParameter();
    break;
  case GeomAbs_Circle:
  case GeomAbs_Ellipse:
    Knots(1) = 0.0;  Knots(2) = M_PI*2.0/3.0;  Knots(3) = M_PI*4.0/3.0;  Knots(4) = M_PI*6.0/3.0;
    break;
  case GeomAbs_Parabola:
  case GeomAbs_Hyperbola:
    Knots(1) = myCurve.FirstParameter();  Knots(2) = myCurve.LastParameter();
    break;
  case GeomAbs_BSplineCurve:
    (*((Handle(Geom2d_BSplineCurve)*)&(myCurve.Curve())))->Knots(Knots);
    break;
  default: 
    Knots(1) = myCurve.FirstParameter();  Knots(2) = myCurve.LastParameter();
    break;
  }  
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepGProp_Face::Load(const Standard_Boolean IsFirstParam,
			  const GeomAbs_IsoType  theIsoType) 
{
  Standard_Real aLen;
  Standard_Real aU1;
  Standard_Real aU2;
  Standard_Real aV1;
  Standard_Real aV2;
  gp_Pnt2d      aLoc;
  gp_Dir2d      aDir;

  Bounds(aU1, aU2, aV1, aV2);

  if (theIsoType == GeomAbs_IsoU) {
    aLen = aV2 - aV1;

    if (IsFirstParam) {
      aLoc.SetCoord(aU1, aV2);
      aDir.SetCoord(0., -1.);
    } else {
      aLoc.SetCoord(aU2, aV1);
      aDir.SetCoord(0., 1.);
    }
  } else if (theIsoType == GeomAbs_IsoV) {
    aLen = aU2 - aU1;

    if (IsFirstParam) {
      aLoc.SetCoord(aU1, aV1);
      aDir.SetCoord(1., 0.);
    } else {
      aLoc.SetCoord(aU2, aV2);
      aDir.SetCoord(-1., 0.);
    }
  } else
    return;

  Handle(Geom2d_Curve) aLin = new Geom2d_Line(aLoc, aDir);

  myCurve.Load(aLin, 0., aLen);
}

//=======================================================================
//function : GetRealKnots
//purpose  : 
//=======================================================================

static void GetRealKnots(const Standard_Real                  theMin,
			 const Standard_Real                  theMax,
		         const Handle(TColStd_HArray1OfReal) &theKnots,
		               Handle(TColStd_HArray1OfReal) &theRealKnots)
{
  Standard_Integer i       = theKnots->Lower() - 1;
  Standard_Integer iU      = theKnots->Upper();
  Standard_Integer aStartI = 0;
  Standard_Integer aEndI   = 0;
  Standard_Real    aTol    = Precision::Confusion();

  while (++i < iU) {
    if (aStartI == 0 && theKnots->Value(i) > theMin + aTol)
      aStartI = i;

    if (aEndI == 0 && theKnots->Value(i + 1) > theMax - aTol)
      aEndI = i;

    if (aStartI != 0 && aEndI != 0)
      break;
  }

  if (aStartI == 0)
    aStartI = iU;

  Standard_Integer aNbNode = Max(0, aEndI - aStartI + 1) + 2;
  Standard_Integer j;

  theRealKnots = new TColStd_HArray1OfReal(1, aNbNode);
  theRealKnots->SetValue(1,       theMin);
  theRealKnots->SetValue(aNbNode, theMax);


  for (i = 2, j = aStartI; j <= aEndI; i++, j++)
    theRealKnots->SetValue(i, theKnots->Value(j));
}

//=======================================================================
//function : GetCurveKnots
//purpose  : 
//=======================================================================

static void GetCurveKnots(const Standard_Real                  theMin,
			  const Standard_Real                  theMax,
			  const Geom2dAdaptor_Curve           &theCurve,
		                Handle(TColStd_HArray1OfReal) &theKnots)
{
  Standard_Boolean isSBSpline = theCurve.GetType() == GeomAbs_BSplineCurve;

  if (isSBSpline) {
    Handle(Geom2d_BSplineCurve)   aCrv;
    Standard_Integer              aNbKnots;
    Handle(TColStd_HArray1OfReal) aCrvKnots;

    aCrv     = Handle(Geom2d_BSplineCurve)::DownCast(theCurve.Curve());
    aNbKnots = aCrv->NbKnots();
    aCrvKnots = new TColStd_HArray1OfReal(1, aNbKnots);
    aCrv->Knots(aCrvKnots->ChangeArray1());
    GetRealKnots(theMin, theMax, aCrvKnots, theKnots);
  } else {
    theKnots = new TColStd_HArray1OfReal(1, 2);
    theKnots->SetValue(1, theMin);
    theKnots->SetValue(2, theMax);
  }
}

//=======================================================================
//function : GetUKnots
//purpose  : 
//=======================================================================

void BRepGProp_Face::GetUKnots
                     (const Standard_Real                  theUMin,
		      const Standard_Real                  theUMax,
		            Handle(TColStd_HArray1OfReal) &theUKnots) const
{
  Standard_Boolean isSBSpline = mySurface.GetType() == GeomAbs_BSplineSurface;
  Standard_Boolean isCBSpline = Standard_False;

  if (!isSBSpline) {
    // Check the basis curve of the surface of linear extrusion.
    if (mySurface.GetType() == GeomAbs_SurfaceOfExtrusion) {
      GeomAdaptor_Curve    aCurve;
      Handle(Geom_Surface) aSurf = mySurface.Surface().Surface();

      aCurve.Load(Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (aSurf)->BasisCurve());
      isCBSpline = aCurve.GetType() == GeomAbs_BSplineCurve;
    }
  }

  if (myIsUseSpan && (isSBSpline || isCBSpline)) {
    // Using span decomposition for BSpline.
    Handle(TColStd_HArray1OfReal) aKnots;
    Standard_Integer              aNbKnots;

    if (isSBSpline) {
      // Get U knots of BSpline surface.
      Handle(Geom_Surface)        aSurf = mySurface.Surface().Surface();
      Handle(Geom_BSplineSurface) aBSplSurf;

      aBSplSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
      aNbKnots  = aBSplSurf->NbUKnots();
      aKnots    = new TColStd_HArray1OfReal(1, aNbKnots);
      aBSplSurf->UKnots(aKnots->ChangeArray1());
    } else {
      // Get U knots of BSpline curve - basis curve of
      // the surface of linear extrusion.
      GeomAdaptor_Curve         aCurve;
      Handle(Geom_Surface)      aSurf = mySurface.Surface().Surface();
      Handle(Geom_BSplineCurve) aBSplCurve;

      aCurve.Load(Handle(Geom_SurfaceOfLinearExtrusion)::DownCast (aSurf)->BasisCurve());
      aBSplCurve = aCurve.BSpline();
      aNbKnots   = aBSplCurve->NbKnots();
      aKnots     = new TColStd_HArray1OfReal(1, aNbKnots);
      aBSplCurve->Knots(aKnots->ChangeArray1());
    }

    // Compute number of knots inside theUMin and theUMax.
    GetRealKnots(theUMin, theUMax, aKnots, theUKnots);
  } else {
    // No span decomposition.
    theUKnots = new TColStd_HArray1OfReal(1, 2);
    theUKnots->SetValue(1, theUMin);
    theUKnots->SetValue(2, theUMax);
  }
}

//=======================================================================
//function : GetTKnots
//purpose  : 
//=======================================================================

void BRepGProp_Face::GetTKnots
                     (const Standard_Real                  theTMin,
		      const Standard_Real                  theTMax,
		            Handle(TColStd_HArray1OfReal) &theTKnots) const
{
  Standard_Boolean isBSpline = mySurface.GetType() == GeomAbs_BSplineSurface;

  if (myIsUseSpan && isBSpline) {
    // Using span decomposition for BSpline.
    Handle(TColStd_HArray1OfReal) aSurfKnots;
    Standard_Integer              aNbKnots;

    // Get V knots of BSpline surface.
    Handle(Geom_Surface)        aSurf = mySurface.Surface().Surface();
    Handle(Geom_BSplineSurface) aBSplSurf;

    aBSplSurf  = Handle(Geom_BSplineSurface)::DownCast(aSurf);
    aNbKnots   = aBSplSurf->NbVKnots();
    aSurfKnots = new TColStd_HArray1OfReal(1, aNbKnots);
    aBSplSurf->VKnots(aSurfKnots->ChangeArray1());

//     Handle(TColStd_HArray1OfReal) aCurveKnots;

//     GetCurveKnots(theTMin, theTMax, myCurve, aCurveKnots);
//    GetRealCurveKnots(aCurveKnots, aSurfKnots, myCurve, theTKnots);
    GetCurveKnots(theTMin, theTMax, myCurve, theTKnots);
  } else {
    theTKnots = new TColStd_HArray1OfReal(1, 2);
    theTKnots->SetValue(1, theTMin);
    theTKnots->SetValue(2, theTMax);
  }
}
