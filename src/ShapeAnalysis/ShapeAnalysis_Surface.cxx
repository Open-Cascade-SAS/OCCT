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

// 06.01.99 pdn private method SurfaceNewton PRO17015: fix against hang in Extrema
// 11.01.99 pdn PRO10109 4517: protect against wrong result
//%12 pdn 11.02.99 PRO9234 project degenerated
// 03.03.99 rln S4135: new algorithms for IsClosed (accepts precision), Degenerated (stores precision)
//:p7 abv 10.03.99 PRO18206: adding new method IsDegenerated()
//:p8 abv 11.03.99 PRO7226 #489490: improving ProjectDegenerated() for degenerated edges
//:q1 pdn, abv 15.03.99 PRO7226 #525030: adding maxpreci in NextValueOfUV()
//:q2 abv 16.03.99: PRO7226 #412920: applying Newton algorithm before UVFromIso()
//:q6 abv 19.03.99: ie_soapbox-B.stp #390760: improving projecting point on surface
//#77 rln 15.03.99: S4135: returning singularity which has minimum gap between singular point and input 3D point
//:r3 abv 30.03.99: (by smh) S4163: protect against unexpected signals
//:#4 smh 07.04.99: S4163 Zero divide.  
//#4  szv           S4163: optimizations
//:r9 abv 09.04.99: id_turbine-C.stp #3865: check degenerated 2d point by recomputing to 3d instead of Resolution
//:s5 abv 22.04.99  Adding debug printouts in catch {} blocks

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <ElSLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAbs_SurfaceForm.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_Surface,Standard_Transient)

//S4135
//S4135
//=======================================================================
//function : ShapeAnalysis_Surface
//purpose  :
//=======================================================================
ShapeAnalysis_Surface::ShapeAnalysis_Surface(const Handle(Geom_Surface)& S) :
       mySurf (S),
       myExtOK(Standard_False), //:30
       myNbDeg (-1), 
       myIsos (Standard_False),
       myIsoBoxes (Standard_False),
       myGap (0.), myUDelt (0.01), myVDelt (0.01), myUCloseVal (-1), myVCloseVal (-1)
{
  mySurf->Bounds(myUF,myUL,myVF,myVL);
  myAdSur = new GeomAdaptor_HSurface (mySurf);
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void ShapeAnalysis_Surface::Init(const Handle(Geom_Surface)& S)
{
  if (mySurf == S) return;
  myExtOK = Standard_False; //:30
  mySurf = S;
  myNbDeg = -1;
  myUCloseVal = myVCloseVal = -1;  myGap = 0.;
  mySurf->Bounds (myUF,myUL,myVF,myVL);
  myAdSur = new GeomAdaptor_HSurface (mySurf);
  myIsos = Standard_False;
  myIsoBoxes = Standard_False;
  myIsoUF.Nullify(); myIsoUL.Nullify(); myIsoVF.Nullify(); myIsoVL.Nullify();
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void ShapeAnalysis_Surface::Init(const Handle(ShapeAnalysis_Surface)& other)
{
  Init (other->Surface());
  myAdSur = other->TrueAdaptor3d();
  myNbDeg = other->myNbDeg; //rln S4135 direct transmission (to avoid computation in <other>)
  for (Standard_Integer i = 0; i < myNbDeg; i ++) {
    other->Singularity (i+1, myPreci[i], myP3d[i], myFirstP2d[i], myLastP2d[i], myFirstPar[i], myLastPar[i], myUIsoDeg[i]);
  }
}

//=======================================================================
//function : Adaptor3d
//purpose  :
//=======================================================================

const Handle(GeomAdaptor_HSurface)& ShapeAnalysis_Surface::Adaptor3d()
{
  return myAdSur;
}

//=======================================================================
//function : ComputeSingularities
//purpose  :
//=======================================================================

void ShapeAnalysis_Surface::ComputeSingularities()
{
  //rln S4135
  if (myNbDeg >= 0) return;
//:51 abv 22 Dec 97: allow forcing:  if (myNbDeg >= 0) return;
//CKY 27-FEV-98 : en appel direct on peut forcer. En appel interne on optimise
  if (mySurf.IsNull())  return;
  
  Standard_Real   su1, sv1, su2, sv2;
//  mySurf->Bounds(su1, su2, sv1, sv2);
  Bounds(su1, su2, sv1, sv2);//modified by rln on 12/11/97 mySurf-> is deleted

  myNbDeg = 0; //:r3
  
  if (mySurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    Handle(Geom_ConicalSurface) conicS =
      Handle(Geom_ConicalSurface)::DownCast(mySurf);
    Standard_Real vApex = - conicS->RefRadius() / Sin (conicS->SemiAngle());
    myPreci   [0] = 0;
    myP3d     [0] = conicS->Apex();
    myFirstP2d[0].SetCoord (su1, vApex);
    myLastP2d [0].SetCoord (su2, vApex);
    myFirstPar[0] = su1;
    myLastPar [0] = su2;
    myUIsoDeg [0] = Standard_False;
    myNbDeg = 1;
  }
  else if (mySurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
    Handle(Geom_ToroidalSurface) toroidS =
      Handle(Geom_ToroidalSurface)::DownCast(mySurf);
    Standard_Real minorR = toroidS->MinorRadius();
    Standard_Real majorR = toroidS->MajorRadius();
    //szv#4:S4163:12Mar99 warning - possible div by zero
    Standard_Real Ang = ACos (Min (1., majorR / minorR));
    myPreci   [0] = myPreci[1] = Max (0., majorR - minorR);
    myP3d     [0] = mySurf->Value (0., M_PI-Ang);
    myFirstP2d[0].SetCoord (su1, M_PI-Ang);
    myLastP2d [0].SetCoord (su2, M_PI-Ang);
    myP3d     [1] = mySurf->Value (0., M_PI+Ang);
    myFirstP2d[1].SetCoord (su2, M_PI+Ang);
    myLastP2d [1].SetCoord (su1, M_PI+Ang);
    myFirstPar[0] = myFirstPar[1] = su1;
    myLastPar [0] = myLastPar [1] = su2;
    myUIsoDeg [0] = myUIsoDeg [1] = Standard_False;
    myNbDeg = ( majorR > minorR ? 1 : 2 );
  }
  else if (mySurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    myPreci   [0] = myPreci[1] = 0;
    myP3d     [0] = mySurf->Value ( su1, sv2 ); // Northern pole is first
    myP3d     [1] = mySurf->Value ( su1, sv1 );
    myFirstP2d[0].SetCoord (su2, sv2);
    myLastP2d [0].SetCoord (su1, sv2);
    myFirstP2d[1].SetCoord (su1, sv1);
    myLastP2d [1].SetCoord (su2, sv1);
    myFirstPar[0] = myFirstPar[1] = su1;
    myLastPar [0] = myLastPar [1] = su2;
    myUIsoDeg [0] = myUIsoDeg [1] = Standard_False;
    myNbDeg = 2;
  }
  else if ((mySurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))    ||
	   (mySurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) || //:b2 abv 18 Feb 98
	   (mySurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))) { //rln S4135

    //rln S4135 //:r3
    myP3d     [0] = myAdSur->Value (su1, 0.5 * (sv1 + sv2));
    myFirstP2d[0].SetCoord (su1, sv2); 
    myLastP2d [0].SetCoord (su1, sv1);

    myP3d     [1] = myAdSur->Value (su2, 0.5 * (sv1 + sv2));
    myFirstP2d[1].SetCoord (su2, sv1); 
    myLastP2d [1].SetCoord (su2, sv2);

    myP3d     [2] = myAdSur->Value (0.5 * (su1 + su2), sv1);
    myFirstP2d[2].SetCoord (su1, sv1); 
    myLastP2d [2].SetCoord (su2, sv1);
    
    myP3d     [3] = myAdSur->Value (0.5 * (su1 + su2), sv2);
    myFirstP2d[3].SetCoord (su2, sv2); 
    myLastP2d [3].SetCoord (su1, sv2);

    myFirstPar[0] = myFirstPar[1] = sv1; 
    myLastPar [0] = myLastPar [1] = sv2;
    myUIsoDeg [0] = myUIsoDeg [1] = Standard_True;

    myFirstPar[2] = myFirstPar[3] = su1; 
    myLastPar [2] = myLastPar [3] = su2;
    myUIsoDeg [2] = myUIsoDeg [3] = Standard_False;

    gp_Pnt Corner1 = myAdSur->Value(su1,sv1);
    gp_Pnt Corner2 = myAdSur->Value(su1,sv2);
    gp_Pnt Corner3 = myAdSur->Value(su2,sv1);
    gp_Pnt Corner4 = myAdSur->Value(su2,sv2);

    myPreci[0] = Max (Corner1.Distance(Corner2), Max (myP3d[0].Distance(Corner1), myP3d[0].Distance(Corner2)));
    myPreci[1] = Max (Corner3.Distance(Corner4), Max (myP3d[1].Distance(Corner3), myP3d[1].Distance(Corner4)));
    myPreci[2] = Max (Corner1.Distance(Corner3), Max (myP3d[2].Distance(Corner1), myP3d[2].Distance(Corner3)));
    myPreci[3] = Max (Corner2.Distance(Corner4), Max (myP3d[3].Distance(Corner2), myP3d[3].Distance(Corner4)));

    myNbDeg = 4;
  }
  SortSingularities();
}

//=======================================================================
//function : HasSingularities
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::HasSingularities (const Standard_Real preci)
{
  return NbSingularities(preci) > 0;
}

//=======================================================================
//function : NbSingularities
//purpose  :
//=======================================================================

Standard_Integer ShapeAnalysis_Surface::NbSingularities(const Standard_Real preci)
{
  if (myNbDeg < 0) ComputeSingularities();
  Standard_Integer Nb = 0;
  for (Standard_Integer i = 1; i <= myNbDeg; i++)
    if (myPreci[i-1] <= preci)
      Nb++;
  return Nb;
}

//=======================================================================
//function : Singularity
//purpose  :
//=======================================================================

 Standard_Boolean ShapeAnalysis_Surface::Singularity(const Standard_Integer num,
						     Standard_Real& preci,
						     gp_Pnt& P3d,
						     gp_Pnt2d& firstP2d,
						     gp_Pnt2d& lastP2d,
						     Standard_Real& firstpar,
						     Standard_Real& lastpar,
						     Standard_Boolean& uisodeg)
{
//  ATTENTION, les champs sont des tableaux C, n0s partent de 0. num part de 1
  if (myNbDeg < 0) ComputeSingularities();
  if (num < 1 || num > myNbDeg) return Standard_False;
  P3d      = myP3d      [num-1];
  preci    = myPreci    [num-1];
  firstP2d = myFirstP2d [num-1];
  lastP2d  = myLastP2d  [num-1];
  firstpar = myFirstPar [num-1];
  lastpar  = myLastPar  [num-1];
  uisodeg  = myUIsoDeg  [num-1];
  return Standard_True;
}

//=======================================================================
//function : IsDegenerated
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::IsDegenerated(const gp_Pnt& P3d,const Standard_Real preci)
{
  if (myNbDeg < 0) ComputeSingularities ();
  for (Standard_Integer i = 0; i < myNbDeg && myPreci[i] <= preci; i ++) {
    myGap = myP3d[i].Distance (P3d);
    //rln S4135
    if (myGap <= preci)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : DegeneratedValues
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::DegeneratedValues(const gp_Pnt& P3d,
							  const Standard_Real preci,
							  gp_Pnt2d& firstP2d,
							  gp_Pnt2d& lastP2d,
							  Standard_Real& firstPar,
							  Standard_Real& lastPar,
							  const Standard_Boolean /*forward*/)
{
  if (myNbDeg < 0) ComputeSingularities ();
  //#77 rln S4135: returning singularity which has minimum gap between singular point and input 3D point
  Standard_Integer indMin = -1;
  Standard_Real gapMin = RealLast();
  for (Standard_Integer i = 0; i < myNbDeg && myPreci[i] <= preci; i ++) {
    myGap = myP3d[i].Distance (P3d);
    //rln S4135
    if (myGap <= preci)
      if (gapMin > myGap) {
	gapMin = myGap;
	indMin = i;
      }
  }
  if (indMin >= 0) {
    myGap = gapMin;
    firstP2d = myFirstP2d[indMin];
    lastP2d  = myLastP2d[indMin];
    firstPar = myFirstPar[indMin];
    lastPar  = myLastPar[indMin];
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ProjectDegenerated
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::ProjectDegenerated(const gp_Pnt& P3d,
							   const Standard_Real preci,
							   const gp_Pnt2d& neighbour,
							   gp_Pnt2d& result)
{
  if (myNbDeg < 0) ComputeSingularities ();
  //added by rln on 03/12/97
  //:c1 abv 23 Feb 98: preci (3d) -> Resolution (2d)
  //#77 rln S4135
  Standard_Integer indMin = -1;
  Standard_Real gapMin = RealLast();
  for (Standard_Integer i = 0; i < myNbDeg && myPreci[i] <= preci; i ++) {
    Standard_Real gap2 = myP3d[i].SquareDistance (P3d);
    if ( gap2 > preci*preci ) 
      gap2 = Min ( gap2, myP3d[i].SquareDistance ( Value(result) ) );
    //rln S4135
    if ( gap2 <= preci*preci && gapMin > gap2 ) {
      gapMin = gap2;
      indMin = i;
    }
  }
  if ( indMin < 0 ) return Standard_False;
  myGap = Sqrt ( gapMin );
  if ( ! myUIsoDeg[indMin] ) result.SetX (neighbour.X());
  else                       result.SetY (neighbour.Y());
  return Standard_True;
}

//pdn %12 11.02.99 PRO9234 entity 15402
//=======================================================================
//function : ProjectDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::ProjectDegenerated(const Standard_Integer nbrPnt,
							   const TColgp_SequenceOfPnt& points,
							   TColgp_SequenceOfPnt2d& pnt2d,
							   const Standard_Real preci,
							   const Standard_Boolean direct)
{
  if (myNbDeg < 0) ComputeSingularities ();
  
  Standard_Integer step = (direct ? 1 : -1);
  //#77 rln S4135
  Standard_Integer indMin = -1;
  Standard_Real gapMin = RealLast(), prec2 = preci*preci;
  Standard_Integer j = (direct ? 1 : nbrPnt);
  for (Standard_Integer i = 0; i < myNbDeg && myPreci[i] <= preci; i ++) {
    Standard_Real gap2 = myP3d[i].SquareDistance (points(j));
    if ( gap2 > prec2 ) 
      gap2 = Min ( gap2, myP3d[i].SquareDistance ( Value(pnt2d(j)) ) );
    if ( gap2 <= prec2 && gapMin > gap2 ) {
      gapMin = gap2;
      indMin = i;
    }
  }
  if ( indMin <0 ) return Standard_False;

  myGap = Sqrt ( gapMin );
  gp_Pnt2d pk;
    
  Standard_Integer k; // svv Jan11 2000 : porting on DEC
  for ( k=j+step; k <= nbrPnt && k >= 1; k += step ) {
    pk = pnt2d(k); 
    gp_Pnt P1 = points(k);
    if ( myP3d[indMin].SquareDistance(P1) > prec2 &&
	 myP3d[indMin].SquareDistance(Value(pk)) > prec2 )
      break;
  }
    
  //:p8 abv 11 Mar 99: PRO7226 #489490: if whole pcurve is degenerate, distribute evenly
  if ( k <1 || k > nbrPnt ) { 
    Standard_Real x1 = ( myUIsoDeg[indMin] ? pnt2d(1).Y() : pnt2d(1).X() );
    Standard_Real x2 = ( myUIsoDeg[indMin] ? pnt2d(nbrPnt).Y() : pnt2d(nbrPnt).X() );
    for ( j = 1; j <= nbrPnt; j++ ) {
      //szv#4:S4163:12Mar99 warning - possible div by zero
      Standard_Real x = ( x1 * ( nbrPnt - j ) + x2 * ( j - 1 ) ) / ( nbrPnt - 1 );
      if ( ! myUIsoDeg[indMin] ) pnt2d(j).SetX ( x );
      else                       pnt2d(j).SetY ( x );
    }
    return Standard_True;
  }

  for ( j = k-step; j <= nbrPnt && j >= 1; j -= step) {
    if (!myUIsoDeg[indMin]) pnt2d(j).SetX (pk.X());
    else                    pnt2d(j).SetY (pk.Y());
  }
  return Standard_True;
}

//=======================================================================
//method : IsDegenerated
//purpose:
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::IsDegenerated (const gp_Pnt2d &p2d1, 
						       const gp_Pnt2d &p2d2,
						       const Standard_Real tol, 
						       const Standard_Real ratio)
{
  gp_Pnt p1 = Value ( p2d1 );
  gp_Pnt p2 = Value ( p2d2 );
  gp_Pnt pm = Value ( 0.5 * ( p2d1.XY() + p2d2.XY() ) );
  Standard_Real max3d = Max ( p1.Distance ( p2 ), 
			      Max ( pm.Distance ( p1 ), pm.Distance ( p2 ) ) );
  if ( max3d > tol ) return Standard_False;

  GeomAdaptor_Surface& SA = Adaptor3d()->ChangeSurface();
  Standard_Real RU = SA.UResolution ( 1. );
  Standard_Real RV = SA.VResolution ( 1. );

  if ( RU < Precision::PConfusion() || RV < Precision::PConfusion() ) return 0;
  Standard_Real du = Abs ( p2d1.X() - p2d2.X() ) / RU; 
  Standard_Real dv = Abs ( p2d1.Y() - p2d2.Y() ) / RV; 
  max3d *= ratio;
  return du * du + dv * dv > max3d * max3d;
}

//=======================================================================
//static : ComputeIso
//purpose  :
//=======================================================================

static Handle(Geom_Curve) ComputeIso
  (const Handle(Geom_Surface)& surf,
   const Standard_Boolean utype, const Standard_Real par)
{
  Handle(Geom_Curve) iso;
  try {
    OCC_CATCH_SIGNALS
    if (utype) iso = surf->UIso (par);
    else       iso = surf->VIso (par);
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
//:s5
    cout << "\nWarning: ShapeAnalysis_Surface, ComputeIso(): Exception in UVIso(): "; 
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    iso.Nullify();
  }
  return iso;
}

//=======================================================================
//function : ComputeBoundIsos
//purpose  :
//=======================================================================

void ShapeAnalysis_Surface::ComputeBoundIsos()
{
  if (myIsos) return;
  myIsos  = Standard_True;
  myIsoUF = ComputeIso (mySurf,Standard_True, myUF);
  myIsoUL = ComputeIso (mySurf,Standard_True, myUL);
  myIsoVF = ComputeIso (mySurf,Standard_False,myVF);
  myIsoVL = ComputeIso (mySurf,Standard_False,myVL);
}

//=======================================================================
//function : UIso
//purpose  :
//=======================================================================

Handle(Geom_Curve) ShapeAnalysis_Surface::UIso(const Standard_Real U)
{
  if (U == myUF)  {  ComputeBoundIsos();  return myIsoUF;  }
  if (U == myUL)  {  ComputeBoundIsos();  return myIsoUL;  }
  return ComputeIso (mySurf,Standard_True, U);
}

//=======================================================================
//function : VIso
//purpose  :
//=======================================================================

Handle(Geom_Curve) ShapeAnalysis_Surface::VIso(const Standard_Real V)
{
  if (V == myVF)  {  ComputeBoundIsos();  return myIsoVF;  }
  if (V == myVL)  {  ComputeBoundIsos();  return myIsoVL;  }
  return ComputeIso (mySurf,Standard_False,V);
}

//=======================================================================
//function : IsUClosed
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::IsUClosed(const Standard_Real preci)
{
  Standard_Real prec = Max (preci, Precision::Confusion());
  if (myUCloseVal < 0) {
//    Faut calculer : calculs minimaux
    Standard_Real uf,ul,vf,vl;
    Bounds (uf,ul,vf,vl);//modified by rln on 12/11/97 mySurf-> is deleted
//    mySurf->Bounds (uf,ul,vf,vl);
    if (Precision::IsInfinite (uf) || Precision::IsInfinite (ul)) myUDelt = 0.;
    else myUDelt = Abs (ul-uf) / 20;//modified by rln 11/11/97 instead of 10
                                     //because of the example when 10 was not enough
    if (mySurf->IsUClosed())  { myUCloseVal = 0.; myUDelt = 0.; myGap = 0.; return Standard_True; }

//    Calculs adaptes
    //#67 rln S4135
    GeomAdaptor_Surface& SurfAdapt = Adaptor3d()->ChangeSurface();
    GeomAbs_SurfaceType surftype = SurfAdapt.GetType();
    if (mySurf->IsKind (STANDARD_TYPE (Geom_RectangularTrimmedSurface)))
      surftype = GeomAbs_OtherSurface;

    switch (surftype) {
    case GeomAbs_Plane: {
      myUCloseVal = RealLast();
      break;
    }
    case GeomAbs_SurfaceOfExtrusion: { //:c8 abv 03 Mar 98: UKI60094 #753: process Geom_SurfaceOfLinearExtrusion
      Handle(Geom_SurfaceOfLinearExtrusion) extr =
	Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurf);
      Handle(Geom_Curve) crv = extr->BasisCurve();
      Standard_Real f = crv->FirstParameter();
      Standard_Real l = crv->LastParameter();
      //:r3 abv (smh) 30 Mar 99: protect against unexpected signals
      if ( ! Precision::IsInfinite ( f ) && ! Precision::IsInfinite ( l ) ) {
	gp_Pnt p1 = crv->Value ( f );
	gp_Pnt p2 = crv->Value ( l );
	myUCloseVal = p1.Distance ( p2 );
      }
      else myUCloseVal = RealLast();
      break;
    }
    case GeomAbs_BSplineSurface: {
      Handle(Geom_BSplineSurface) bs = Handle(Geom_BSplineSurface)::DownCast(mySurf);
      Standard_Integer nbup = bs->NbUPoles();
      Standard_Real distmin = RealLast();
      if (bs->IsUPeriodic()) {
	myUCloseVal = 0;
	myUDelt = 0;
      }
      else if (nbup < 3) {//modified by rln on 12/11/97
	myUCloseVal = RealLast();
      }
      else if (bs->IsURational() ||
	       //#6 rln 20/02/98 ProSTEP ug_exhaust-A.stp entity #18360 (Uclosed BSpline,
	       //but multiplicity of boundary knots != degree + 1)
	       bs->UMultiplicity(1)              != bs->UDegree()+1 ||  //#6 //:h4: #6 moved
	       bs->UMultiplicity(bs->NbUKnots()) != bs->UDegree()+1 ) { //#6 //:h4
	Standard_Integer nbvk = bs->NbVKnots();
	for (Standard_Integer i = 1; i <= nbvk; i ++) {
	  Standard_Real v = bs->VKnot(i);
	  gp_Pnt p1 = SurfAdapt.Value (uf, v);
	  gp_Pnt p2 = SurfAdapt.Value (ul, v);
	  myUCloseVal = Max (myUCloseVal, p1.SquareDistance (p2));
	  distmin     = Min (distmin,     p1.SquareDistance (p2));
	  if (i > 1) {
	    v = 0.5 * (bs->VKnot(i-1) + bs->VKnot(i));
	    p1 = bs->Value (uf, v);
	    p2 = bs->Value (ul, v);
	    myUCloseVal = Max (myUCloseVal, p1.SquareDistance (p2));
	    distmin     = Min (distmin,     p1.SquareDistance (p2));
	  }
	}
	myUCloseVal = Sqrt (myUCloseVal);
	distmin     = Sqrt (distmin);
	myUDelt     = Min (myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); //#4 smh
      }
      else {
	Standard_Integer nbvp = bs->NbVPoles();
	for (Standard_Integer i = 1; i <= nbvp; i ++) {
	  myUCloseVal = Max (myUCloseVal, bs->Pole(1,i).SquareDistance(bs->Pole(nbup,i)));
	  distmin     = Min (distmin,     bs->Pole(1,i).SquareDistance(bs->Pole(nbup,i)));
	}
	myUCloseVal = Sqrt (myUCloseVal);
	distmin     = Sqrt (distmin);
	myUDelt     = Min (myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); //#4 smh
      }
      break;
    }
    case GeomAbs_BezierSurface: {
      Handle(Geom_BezierSurface) bz = Handle(Geom_BezierSurface)::DownCast(mySurf);
      Standard_Integer nbup = bz->NbUPoles();
      Standard_Real distmin = RealLast();
      if (nbup < 3)
	myUCloseVal = RealLast();
      else {
	Standard_Integer nbvp = bz->NbVPoles();
	for (Standard_Integer i = 1; i <= nbvp; i ++) {
	  myUCloseVal = Max (myUCloseVal, bz->Pole(1,i).SquareDistance(bz->Pole(nbup,i)));
	  distmin     = Min (distmin,     bz->Pole(1,i).SquareDistance(bz->Pole(nbup,i)));
	}
	myUCloseVal = Sqrt (myUCloseVal);
	distmin     = Sqrt (distmin);
	myUDelt     = Min (myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); //#4 smh
      }
      break;
    }
    default: { //Geom_RectangularTrimmedSurface and Geom_OffsetSurface
      Standard_Real distmin = RealLast();
      Standard_Integer nbpoints = 101; //can be revised
      for (Standard_Integer i = 0; i < nbpoints; i++) {
	gp_Pnt p1 = SurfAdapt.Value (uf, vf + (vl - vf ) * i / (nbpoints - 1));
	gp_Pnt p2 = SurfAdapt.Value (ul, vf + (vl - vf ) * i / (nbpoints - 1));
	myUCloseVal = Max (myUCloseVal, p1.SquareDistance (p2));
	distmin     = Min (distmin,     p1.SquareDistance (p2));
      }
      myUCloseVal = Sqrt (myUCloseVal);
      distmin     = Sqrt (distmin);
      myUDelt     = Min (myUDelt, 0.5 * SurfAdapt.UResolution(distmin)); //#4 smh
      break;
    }
    } //switch
    myGap = myUCloseVal;
  }
  return (myUCloseVal <= prec);
}

//=======================================================================
//function : IsVClosed
//purpose  :
//=======================================================================

Standard_Boolean ShapeAnalysis_Surface::IsVClosed(const Standard_Real preci)
{
  Standard_Real prec = Max (preci, Precision::Confusion());
  if (myVCloseVal < 0) {
//    Faut calculer : calculs minimaux
    Standard_Real uf,ul,vf,vl;
    Bounds (uf,ul,vf,vl);//modified by rln on 12/11/97 mySurf-> is deleted
//    mySurf->Bounds (uf,ul,vf,vl);
    if (Precision::IsInfinite (vf) || Precision::IsInfinite (vl)) myVDelt = 0.;
    else myVDelt = Abs (vl-vf) / 20;// 2; rln S4135
                                    //because of the example when 10 was not enough
    if (mySurf->IsVClosed())  { myVCloseVal = 0.; myVDelt = 0.; myGap = 0.; return Standard_True; }

//    Calculs adaptes
    //#67 rln S4135
    GeomAdaptor_Surface& SurfAdapt = Adaptor3d()->ChangeSurface();
    GeomAbs_SurfaceType surftype = SurfAdapt.GetType();
    if (mySurf->IsKind (STANDARD_TYPE (Geom_RectangularTrimmedSurface)))
      surftype = GeomAbs_OtherSurface;

    switch (surftype) {
    case GeomAbs_Plane:
    case GeomAbs_Cone:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_SurfaceOfExtrusion: {
      myVCloseVal = RealLast();
      break;
    }
    case GeomAbs_SurfaceOfRevolution: {
      Handle(Geom_SurfaceOfRevolution) revol =
	Handle(Geom_SurfaceOfRevolution)::DownCast(mySurf);
      Handle(Geom_Curve) crv = revol->BasisCurve();
      gp_Pnt p1 = crv->Value ( crv->FirstParameter() );
      gp_Pnt p2 = crv->Value ( crv->LastParameter() );
      myVCloseVal = p1.Distance ( p2 );
      break;
    }
    case GeomAbs_BSplineSurface: {
      Handle(Geom_BSplineSurface) bs = Handle(Geom_BSplineSurface)::DownCast(mySurf);
      Standard_Integer nbvp = bs->NbVPoles();
      Standard_Real distmin = RealLast();
      if (bs->IsVPeriodic()) {
	myVCloseVal = 0;
	myVDelt = 0;
      }
      else if (nbvp < 3) {//modified by rln on 12/11/97
	myVCloseVal = RealLast();
      }
      else if (bs->IsVRational() ||
	       bs->VMultiplicity(1)              != bs->VDegree()+1 ||  //#6 //:h4
	       bs->VMultiplicity(bs->NbVKnots()) != bs->VDegree()+1 ) { //#6 //:h4
	Standard_Integer nbuk = bs->NbUKnots();
	for (Standard_Integer i = 1; i <= nbuk; i ++) {
	  Standard_Real u = bs->UKnot(i);
	  gp_Pnt p1 = SurfAdapt.Value (u, vf);
	  gp_Pnt p2 = SurfAdapt.Value (u, vl);
	  myVCloseVal = Max (myVCloseVal, p1.SquareDistance (p2));
	  distmin     = Min (distmin,     p1.SquareDistance (p2));
	  if (i > 1) {
	    u = 0.5 * (bs->UKnot(i-1) + bs->UKnot(i));
	    p1 = SurfAdapt.Value (u, vf);
	    p2 = SurfAdapt.Value (u, vl);
	    myVCloseVal = Max (myVCloseVal, p1.SquareDistance (p2));
	    distmin     = Min (distmin,     p1.SquareDistance (p2));
	  }
	}
	myVCloseVal = Sqrt (myVCloseVal);
	distmin     = Sqrt (distmin);
	myVDelt     = Min (myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); //#4 smh
      }
      else {
	Standard_Integer nbup = bs->NbUPoles();
	for (Standard_Integer i = 1; i <= nbup; i ++) {
	  myVCloseVal = Max (myVCloseVal, bs->Pole(i,1).SquareDistance(bs->Pole(i,nbvp)));
	  distmin     = Min (distmin,     bs->Pole(i,1).SquareDistance(bs->Pole(i,nbvp)));
	}
	myVCloseVal = Sqrt (myVCloseVal);
	distmin     = Sqrt (distmin);
	myVDelt     = Min (myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); //#4 smh
      }
      break;
    }
    case GeomAbs_BezierSurface: {
      Handle(Geom_BezierSurface) bz = Handle(Geom_BezierSurface)::DownCast(mySurf);
      Standard_Integer nbvp = bz->NbVPoles();
      Standard_Real distmin = RealLast();
      if (nbvp < 3)
	myVCloseVal = RealLast();
      else {
	Standard_Integer nbup = bz->NbUPoles();
	for (Standard_Integer i = 1; i <= nbup; i ++) {
	  myVCloseVal = Max (myVCloseVal, bz->Pole(i,1).SquareDistance(bz->Pole(i,nbvp)));
	  distmin     = Min (distmin,     bz->Pole(i,1).SquareDistance(bz->Pole(i,nbvp)));
	}
	myVCloseVal = Sqrt (myVCloseVal);
	distmin     = Sqrt (distmin);
	myVDelt     = Min (myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); //#4 smh
      }
      break;
    }
    default: { //Geom_RectangularTrimmedSurface and Geom_OffsetSurface
      Standard_Real distmin = RealLast();
      Standard_Integer nbpoints = 101; //can be revised
      for (Standard_Integer i = 0; i < nbpoints; i++) {
	gp_Pnt p1 = SurfAdapt.Value (uf + (ul - uf ) * i / (nbpoints - 1), vf);
	gp_Pnt p2 = SurfAdapt.Value (uf + (ul - uf ) * i / (nbpoints - 1), vl);
	myVCloseVal = Max (myVCloseVal, p1.SquareDistance (p2));
	distmin     = Min (distmin,     p1.SquareDistance (p2));
      }
      myVCloseVal = Sqrt (myVCloseVal);
      distmin     = Sqrt (distmin);
      myVDelt     = Min (myVDelt, 0.5 * SurfAdapt.VResolution(distmin)); //#4 smh
      break;
    }
    } //switch
    myGap = myVCloseVal;
  }
  return (myVCloseVal <= prec);
}

//=======================================================================
//function : SurfaceNewton
//purpose  : Newton algo (S4030)
//=======================================================================
Standard_Integer ShapeAnalysis_Surface::SurfaceNewton(const gp_Pnt2d &p2dPrev,
                                                      const gp_Pnt& P3D,
                                                      const Standard_Real preci,
                                                      gp_Pnt2d &sol)
{
  GeomAdaptor_Surface& SurfAdapt = Adaptor3d()->ChangeSurface();
  Standard_Real uf, ul, vf, vl;
  Bounds(uf, ul, vf, vl);
  Standard_Real du = SurfAdapt.UResolution (preci);
  Standard_Real dv = SurfAdapt.VResolution (preci);
  Standard_Real UF = uf - du, UL = ul + du;
  Standard_Real VF = vf - dv, VL = vl + dv;

//Standard_Integer fail = 0;
  Standard_Real Tol = Precision::Confusion();
  Standard_Real Tol2 = Tol * Tol;//, rs2p=1e10;
  Standard_Real U = p2dPrev.X(), V = p2dPrev.Y();
  gp_Vec rsfirst = P3D.XYZ() - Value ( U, V ).XYZ(); //pdn
  for ( Standard_Integer i=0; i < 25; i++ ) {
    gp_Vec ru, rv, ruu, rvv, ruv;
    gp_Pnt pnt;
    SurfAdapt.D2 ( U, V, pnt, ru, rv, ruu, rvv, ruv );

    // normal
    Standard_Real ru2 = ru * ru, rv2 = rv * rv;
    gp_Vec n = ru ^ rv;
    Standard_Real nrm2 = n.SquareMagnitude();
    if ( nrm2 < 1e-10 || Precision::IsPositiveInfinite(nrm2)) break; // n == 0, use standard

    // descriminant
    gp_Vec rs = P3D.XYZ() - Value ( U, V ).XYZ();
    Standard_Real rSuu = ( rs * ruu );
    Standard_Real rSvv = ( rs * rvv );
    Standard_Real rSuv = ( rs * ruv );
    Standard_Real D = -nrm2 + rv2 * rSuu + ru2 * rSvv - 
                      2 * rSuv * (ru*rv) + rSuv*rSuv - rSuu*rSvv;
    if ( fabs ( D ) < 1e-10 ) break; // bad case; use standard

    // compute step
    Standard_Real fract = 1. / D;
    du = ( rs * ( ( n ^ rv ) + ru * rSvv - rv * rSuv ) ) * fract;
    dv = ( rs * ( ( ru ^ n ) + rv * rSuu - ru * rSuv ) ) * fract;
    U += du;
    V += dv;
    if ( U < UF || U > UL || V < VF || V > VL ) break;
    // check that iterations do not diverge
//pdn    Standard_Real rs2 = rs.SquareMagnitude();
//    if ( rs2 > 4.*rs2p ) break;
//    rs2p = rs2;

    // test the step by uv and deviation from the solution
    Standard_Real aResolution = Max(1e-12,(U+V)*10e-16);
    if ( fabs ( du ) + fabs ( dv ) > aResolution ) continue; //Precision::PConfusion()  continue;

    //if ( U < UF || U > UL || V < VF || V > VL ) break;

    //pdn PRO10109 4517: protect against wrong result
    Standard_Real rs2 = rs.SquareMagnitude();
    if ( rs2 > rsfirst.SquareMagnitude() ) break;

    Standard_Real rsn = rs * n;
    if ( rs2 - rsn * rsn / nrm2 > Tol2 ) break;

//  if ( rs2 > 100 * preci * preci ) { fail = 6; break; }

    // OK, return the result
//	cout << "Newton: solution found in " << i+1 << " iterations" << endl;
    sol.SetCoord( U, V );
    
    return ( nrm2 < 0.01 * ru2 * rv2 ? 2 : 1 ); //:q6
  }
//      cout << "Newton: failed after " << i+1 << " iterations (fail " << fail << " )" << endl;
  return Standard_False;
}

//=======================================================================
//function : NextValueOfUV
//purpose  : optimizing projection by Newton algo (S4030)
//=======================================================================

gp_Pnt2d ShapeAnalysis_Surface::NextValueOfUV(const gp_Pnt2d &p2dPrev,
                                              const gp_Pnt& P3D,
                                              const Standard_Real preci,
                                              const Standard_Real maxpreci)
{
  GeomAdaptor_Surface& SurfAdapt = Adaptor3d()->ChangeSurface();
  GeomAbs_SurfaceType surftype = SurfAdapt.GetType();

  switch (surftype){
  case GeomAbs_BezierSurface :
  case GeomAbs_BSplineSurface :
  case GeomAbs_SurfaceOfExtrusion :
  case GeomAbs_SurfaceOfRevolution :
  case GeomAbs_OffsetSurface :

    {
      if (surftype == GeomAbs_BSplineSurface)
      {
        Handle(Geom_BSplineSurface) aBSpline =  SurfAdapt.BSpline();

        //Check near to knot position ~ near to C0 points on U isoline.
        if ( SurfAdapt.UContinuity() == GeomAbs_C0 )
        {
          Standard_Integer aMinIndex = aBSpline->FirstUKnotIndex();
          Standard_Integer aMaxIndex = aBSpline->LastUKnotIndex();
          for (Standard_Integer anIdx = aMinIndex; anIdx <= aMaxIndex; ++anIdx)
          {
            Standard_Real aKnot = aBSpline->UKnot(anIdx);
            if (Abs (aKnot - p2dPrev.X()) < Precision::Confusion())
              return ValueOfUV ( P3D, preci );
          }
        }

        //Check near to knot position ~ near to C0 points on U isoline.
        if ( SurfAdapt.VContinuity() == GeomAbs_C0 )
        {
          Standard_Integer aMinIndex = aBSpline->FirstVKnotIndex();
          Standard_Integer aMaxIndex = aBSpline->LastVKnotIndex();
          for (Standard_Integer anIdx = aMinIndex; anIdx <= aMaxIndex; ++anIdx)
          {
            Standard_Real aKnot = aBSpline->VKnot(anIdx);
            if (Abs (aKnot - p2dPrev.Y()) < Precision::Confusion())
              return ValueOfUV ( P3D, preci );
          }
        }
      }

      gp_Pnt2d sol;
      Standard_Integer res = SurfaceNewton(p2dPrev,P3D,preci,sol);
      if (res != 0)
      {
        Standard_Real gap = P3D.Distance ( Value(sol) );
        if ( res == 2 || //:q6 abv 19 Mar 99: protect against strange attractors
          ( maxpreci > 0. && gap - maxpreci > Precision::Confusion()) ) 
        { //:q1: check with maxpreci
          Standard_Real U = sol.X(), V = sol.Y();
          myGap = UVFromIso ( P3D, preci, U, V );
          //	  gp_Pnt2d p = ValueOfUV ( P3D, preci );
          if ( gap >= myGap ) return gp_Pnt2d ( U, V );
        }
        myGap = gap;
        return sol;
      }
    }
    break;
  default:
    break;
  }
  return ValueOfUV ( P3D, preci );
}

//=======================================================================
//function : ValueOfUV
//purpose  :
//=======================================================================

gp_Pnt2d ShapeAnalysis_Surface::ValueOfUV(const gp_Pnt& P3D,const Standard_Real preci)
{
  GeomAdaptor_Surface& SurfAdapt = Adaptor3d()->ChangeSurface();
  Standard_Real S = 0., T = 0.;
  myGap = -1.;    // devra etre calcule
  Standard_Boolean computed = Standard_True;  // a priori
  
  Standard_Real uf, ul, vf, vl;
  Bounds(uf, ul, vf, vl);//modified by rln on 12/11/97 mySurf-> is deleted
  
  { //:c9 abv 3 Mar 98: UKI60107-1 #350: to prevent 'catch' from catching exception raising below it
  try {   // ajout CKY 30-DEC-1997 (cf ProStep TR6 r_89-ug)
    OCC_CATCH_SIGNALS
  GeomAbs_SurfaceType surftype = SurfAdapt.GetType();
  switch (surftype){

  case GeomAbs_Plane :
    {
      gp_Pln Plane = SurfAdapt.Plane();
      ElSLib::Parameters( Plane, P3D, S, T);
      break;
    }
  case GeomAbs_Cylinder :
    {
      gp_Cylinder Cylinder = SurfAdapt.Cylinder();
      ElSLib::Parameters( Cylinder, P3D, S, T);
      S += ShapeAnalysis::AdjustByPeriod(S,0.5*(uf+ul),2*M_PI);
      break;
    }
  case GeomAbs_Cone :
    {
      gp_Cone Cone = SurfAdapt.Cone();
      ElSLib::Parameters( Cone, P3D, S, T);
      S += ShapeAnalysis::AdjustByPeriod(S,0.5*(uf+ul),2*M_PI);
      break;
    }
  case GeomAbs_Sphere :
    {
      gp_Sphere Sphere = SurfAdapt.Sphere();
      ElSLib::Parameters( Sphere, P3D, S, T);
      S += ShapeAnalysis::AdjustByPeriod(S,0.5*(uf+ul),2*M_PI);
      break;
    }
  case GeomAbs_Torus :
    {
      gp_Torus Torus = SurfAdapt.Torus();
      ElSLib::Parameters( Torus, P3D, S, T);
      S += ShapeAnalysis::AdjustByPeriod(S,0.5*(uf+ul),2*M_PI);
      T += ShapeAnalysis::AdjustByPeriod(T,0.5*(vf+vl),2*M_PI);
      break;
    }
  case GeomAbs_BezierSurface :
  case GeomAbs_BSplineSurface :
  case GeomAbs_SurfaceOfExtrusion :
  case GeomAbs_SurfaceOfRevolution :
  case GeomAbs_OffsetSurface : //:d0 abv 3 Mar 98: UKI60107-1 #350
    {
      S = (uf+ul)/2;  T = (vf+vl)/2;  // yaura aumoins qqchose
      //pdn to fix hangs PRO17015
      if ((surftype==GeomAbs_SurfaceOfExtrusion)&&Precision::IsInfinite(uf)&&Precision::IsInfinite(ul)) {
	//conic case
	gp_Pnt2d prev(S,T);
	gp_Pnt2d solution;
	if (SurfaceNewton(prev,P3D,preci,solution) != 0)
  {
#ifdef OCCT_DEBUG
	  cout <<"Newton found point on conic extrusion"<<endl;
#endif
	  return solution;
	}
#ifdef OCCT_DEBUG
	cout <<"Newton failed point on conic extrusion"<<endl;
#endif
	uf = -500;
	ul = 500;
      }

      if (Precision::IsInfinite(uf)) uf = -1000;
      if (Precision::IsInfinite(ul)) ul = 1000;
      if (Precision::IsInfinite(vf)) vf = -1000;
      if (Precision::IsInfinite(vl)) vl = 1000;

      //:30 by abv 2.12.97: speed optimization
      // code is taken from GeomAPI_ProjectPointOnSurf
      if ( ! myExtOK ) {
//      Standard_Real du = Abs(ul-uf)/100;  Standard_Real dv = Abs(vl-vf)/100;
//      if (IsUClosed()) du = 0;  if (IsVClosed()) dv = 0;
//  Forcer appel a IsU-VClosed
	if (myUCloseVal < 0) IsUClosed();
	if (myVCloseVal < 0) IsVClosed();
    Standard_Real du = 0., dv = 0.;
    //extension of the surface range is limited to non-offset surfaces as the latter
    //can throw exception (e.g. Geom_UndefinedValue) when computing value - see id23943
    if (!mySurf->IsKind (STANDARD_TYPE (Geom_OffsetSurface))) {
      //modified by rln during fixing CSR # BUC60035 entity #D231
      du = Min (myUDelt, SurfAdapt.UResolution (preci));
      dv = Min (myVDelt, SurfAdapt.VResolution (preci));
    }
	Standard_Real Tol = Precision::PConfusion();
        myExtPS.SetFlag (Extrema_ExtFlag_MIN);
	myExtPS.Initialize (SurfAdapt, uf-du, ul+du, vf-dv, vl+dv, Tol, Tol );
	myExtOK = Standard_True;
      }
      myExtPS.Perform ( P3D );
      Standard_Integer nPSurf = ( myExtPS.IsDone() ? myExtPS.NbExt() : 0 );

      if ( nPSurf > 0 ) {
	Standard_Real dist2Min = myExtPS.SquareDistance( 1 );
	Standard_Integer indMin=1;
	for (Standard_Integer sol = 2; sol <= nPSurf ; sol++) {
	  Standard_Real dist2 = myExtPS.SquareDistance(sol);
	  if ( dist2Min > dist2 ) {
	    dist2Min = dist2;
	    indMin = sol;
	  }
	}
        myExtPS.Point(indMin).Parameter ( S, T );
        // PTV 26.06.2002 WORKAROUND protect OCC486. Remove after fix bug.
        // file CEA_cuve-V5.igs Entityes 244, 259, 847, 925
        // if project point3D on SurfaceOfRevolution Extreme recompute 2d point, but
        // returns an old distance from 3d to solution :-(
        gp_Pnt aCheckPnt = SurfAdapt.Value( S, T );
        dist2Min = P3D.SquareDistance(aCheckPnt);
        // end of WORKAROUND
	Standard_Real disSurf = sqrt (dist2Min);//, disCurv =1.e10;

	// Test de projection merdeuse sur les bords :
	Standard_Real UU = S, VV = T, DistMinOnIso = RealLast();  // myGap;
//	ForgetNewton(P3D, mySurf, preci, UU, VV, DistMinOnIso);

	//test added by rln on 08/12/97
//	DistMinOnIso = UVFromIso (P3D, preci, UU, VV);
	Standard_Boolean possLockal = Standard_False; //:study S4030 (optimizing)
	if (disSurf > preci) {
	  gp_Pnt2d pp(UU,VV);
	  if (SurfaceNewton (pp, P3D, preci, pp) != 0)
    { //:q2 abv 16 Mar 99: PRO7226 #412920
	    Standard_Real dist = P3D.Distance ( Value(pp) );
	    if ( dist < disSurf ) {
	      disSurf = dist;
	      S = UU = pp.X();
	      T = VV = pp.Y();
	    }
	  }
	  if ( disSurf < 10*preci)
	    if (mySurf->Continuity() != GeomAbs_C0){
	      Standard_Real Tol = Precision::Confusion();
	      gp_Vec D1U, D1V;
	      gp_Pnt pnt;
	      SurfAdapt.D1(UU, VV, pnt, D1U, D1V);
	      gp_Vec b = D1U.Crossed(D1V);
	      gp_Vec a (pnt, P3D);
	      Standard_Real ab = a.Dot(b);
	      Standard_Real nrm2 = b.SquareMagnitude();
	      if ( nrm2 > 1e-10 ) {
		Standard_Real dist = a.SquareMagnitude() - (ab*ab)/nrm2;
		possLockal = ( dist < Tol*Tol );
	      }
	    }
	  if (!possLockal) {
	    DistMinOnIso = UVFromIso (P3D, preci, UU, VV);
	  }
	}

	if (disSurf > DistMinOnIso) {
	  // On prend les parametres UU et VV;
	  S = UU;
	  T = VV;
	  myGap = DistMinOnIso;
	}
	else {
	  myGap = disSurf;
	}

	// On essaie Intersection Droite Passant par P3D / Surface
//	if ((myGap > preci)&&(!possLockal) ) {
//	  Standard_Real SS, TT;
//	  disCurv = FindUV(P3D, mySurf, S, T, SS, TT);
//	  if (disCurv < preci || disCurv < myGap) {
//	    S = SS;
//	    T = TT;
//	  }
//	}

      }
      else {
#ifdef OCCT_DEBUG
	cout << "Warning: ShapeAnalysis_Surface::ValueOfUV(): Extrema failed, doing Newton" << endl;
#endif
	// on essai sur les bords
	Standard_Real UU = S, VV = T;//, DistMinOnIso;
//	ForgetNewton(P3D, mySurf, preci, UU, VV, DistMinOnIso);
	myGap = UVFromIso (P3D, preci, UU, VV);
//	if (DistMinOnIso > preci) {
//	  Standard_Real SS, TT;
//	  Standard_Real disCurv = FindUV(P3D, mySurf, UU, VV, SS, TT);
//	  if (disCurv < preci) {
//	    S = SS;
//	    T = TT;
//	  }
//	}
//	else {
	  S = UU;
	  T = VV;
//	}
      }
    }
    break;

    default :
      computed = Standard_False;
    break;
  }

  }  // end Try ValueOfUV (CKY 30-DEC-1997)

  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
//   Pas de raison mais qui sait. Mieux vaut retourner un truc faux que stopper
//   L ideal serait d avoir un status ... mais qui va l interroger ?
//   Avec ce status, on saurait que ce point est a sauter et voila tout
//   En attendant, on met une valeur "pas idiote" mais surement fausse ...
//szv#4:S4163:12Mar99 optimized
//:s5
    cout << "\nWarning: ShapeAnalysis_Surface::ValueOfUV(): Exception: "; 
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    S = (Precision::IsInfinite(uf))? 0 : (uf+ul) / 2.;
    T = (Precision::IsInfinite(vf))? 0 : (vf+vl) / 2.;
  }
  } //:c9
  //szv#4:S4163:12Mar99 waste raise
  //if (!computed) throw Standard_NoSuchObject("PCurveLib_ProjectPointOnSurf::ValueOfUV untreated surface type");
  if (computed) { if (myGap <= 0) myGap = P3D.Distance (SurfAdapt.Value (S,T)); }
  else { myGap = -1.; S = 0.; T = 0.; }
  return gp_Pnt2d( S, T);
}

//=======================================================================
//function : UVFromIso
//purpose  :
//=======================================================================

Standard_Real ShapeAnalysis_Surface::UVFromIso(const gp_Pnt& P3d,const Standard_Real preci,Standard_Real& U,Standard_Real& V)
{
//  Projection qui considere les isos ... comme suit :
//  Les 4 bords, plus les isos en U et en V
//  En effet, souvent, un des deux est bon ...
  Standard_Real theMin = RealLast();
  
  gp_Pnt pntres;
  Standard_Real Cf, Cl, UU,VV;

  //  Initialisation des recherches : point deja trouve (?)
  UU = U; VV = V;
  gp_Pnt depart = myAdSur->Value (U,V);
  theMin = depart.Distance(P3d);
    
  if (theMin < preci/10) return theMin;  // c etait deja OK
  ComputeBoxes();
  if(myIsoUF.IsNull() || myIsoUL.IsNull() || myIsoVF.IsNull() || myIsoVL.IsNull()) {
    // no isolines
    // no more precise computation
    return theMin;
  }
  try {    // RAJOUT    
    OCC_CATCH_SIGNALS
    //pdn Create BndBox containing point;
    Bnd_Box aPBox;
    aPBox.Set(P3d);

    //cout<<"Adaptor3d()->Surface().GetType() = "<<Adaptor3d()->Surface().GetType()<<endl;

    //modified by rln on 04/12/97 in order to use theese variables later
    Standard_Boolean UV = Standard_True;
    Standard_Real par=0., other=0., dist=0.;
    Handle(Geom_Curve) iso;
    Adaptor3d_IsoCurve anIsoCurve(Adaptor3d());
    for (Standard_Integer num = 0; num < 6; num ++) {

      UV = (num < 3);  // 0-1-2 : iso-U  3-4-5 : iso-V
      if( !(Adaptor3d()->Surface().GetType()==GeomAbs_OffsetSurface) ) {
	const Bnd_Box *anIsoBox = 0;
        switch (num) {
        case 0 : par = myUF; iso = myIsoUF;  anIsoBox = &myBndUF; break;
        case 1 : par = myUL; iso = myIsoUL;  anIsoBox = &myBndUL; break;
        case 2 : par = U;    iso = UIso (U); break;
        case 3 : par = myVF; iso = myIsoVF;  anIsoBox = &myBndVF; break;
        case 4 : par = myVL; iso = myIsoVL;  anIsoBox = &myBndVL; break;
        case 5 : par = V;    iso = VIso (V); break;
        default: break;
        }
        
        //    On y va la-dessus
        if (!Precision::IsInfinite(par) && !iso.IsNull()) {
          if( anIsoBox && anIsoBox->Distance(aPBox) > theMin)
            continue;
          
          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();
          
          if (Precision::IsInfinite(Cf))  Cf = -1000;
          if (Precision::IsInfinite(Cl))  Cl = +1000;
          dist = ShapeAnalysis_Curve().Project (iso,P3d,preci,pntres,other,Cf,Cl, Standard_False);
          if (dist < theMin) {
            theMin = dist;
            //:q6	if (UV) VV = other;  else UU = other;
            //  Selon une isoU, on calcule le meilleur V; et lycee de Versailles
            UU = (UV ? par : other);  VV = (UV ? other : par); //:q6: uncommented
          }
        }
      }

      else {
        Adaptor3d_Curve *anAdaptor = NULL;
        GeomAdaptor_Curve aGeomCurve;
        
	const Bnd_Box *anIsoBox = 0;
        switch (num) {
        case 0 : par = myUF; aGeomCurve.Load(myIsoUF); anAdaptor=&aGeomCurve; anIsoBox = &myBndUF; break;
        case 1 : par = myUL; aGeomCurve.Load(myIsoUL); anAdaptor=&aGeomCurve; anIsoBox = &myBndUL;break;
        case 2 : par = U;    anIsoCurve.Load(GeomAbs_IsoU,U); anAdaptor=&anIsoCurve; break;
        case 3 : par = myVF; aGeomCurve.Load(myIsoVF); anAdaptor=&aGeomCurve; anIsoBox = &myBndVF; break;
        case 4 : par = myVL; aGeomCurve.Load(myIsoVL); anAdaptor=&aGeomCurve; anIsoBox = &myBndVL;break;
        case 5 : par = V;    anIsoCurve.Load(GeomAbs_IsoV,V); anAdaptor=&anIsoCurve; break;
          default : break;
        }
        if( anIsoBox && anIsoBox->Distance(aPBox) > theMin)
          continue;
        dist = ShapeAnalysis_Curve().Project(*anAdaptor,P3d,preci,pntres,other);
        if (dist < theMin) {
          theMin = dist;
          UU = (UV ? par : other);  VV = (UV ? other : par); //:q6: uncommented
        }
      }
    }
    
    //added by rln on 04/12/97 iterational process
    Standard_Real PrevU = U, PrevV = V;
    Standard_Integer MaxIters = 5, Iters = 0;
    if( !(Adaptor3d()->Surface().GetType()==GeomAbs_OffsetSurface) ) {
      while (((PrevU != UU) || (PrevV != VV)) && (Iters < MaxIters) && (theMin > preci)) {
        PrevU = UU; PrevV = VV;
        if (UV) {par = UU; iso = UIso(UU);}
        else    {par = VV; iso = VIso(VV);}
        if(!iso.IsNull()) {
          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();
          if (Precision::IsInfinite(Cf))  Cf = -1000;
          if (Precision::IsInfinite(Cl))  Cl = +1000;
          dist = ShapeAnalysis_Curve().Project (iso, P3d, preci, pntres, other, Cf, Cl, Standard_False);
          if (dist < theMin) {
            theMin = dist;
            if (UV) VV = other;  else UU = other;
          }
        }
        UV = !UV;
        if (UV) {par = UU; iso = UIso(UU);}
        else    {par = VV; iso = VIso(VV);}
        if(!iso.IsNull()) {
          Cf = iso->FirstParameter();
          Cl = iso->LastParameter();
          if (Precision::IsInfinite(Cf))  Cf = -1000;
          if (Precision::IsInfinite(Cl))  Cl = +1000;
          dist = ShapeAnalysis_Curve().Project (iso, P3d, preci, pntres, other, Cf, Cl, Standard_False);
          if (dist < theMin) {
            theMin = dist;
            if (UV) VV = other;  else UU = other;
          }
        }
        UV = !UV;
        Iters++;
      }
    }

    else {
      while (((PrevU != UU) || (PrevV != VV)) && (Iters < MaxIters) && (theMin > preci)) {
        PrevU = UU; PrevV = VV;
        if (UV) {
          par = UU;
          anIsoCurve.Load(GeomAbs_IsoU,UU);
        }
        else {
          par = VV;
          anIsoCurve.Load(GeomAbs_IsoV,VV);
        }
        Cf = anIsoCurve.FirstParameter();
        Cl = anIsoCurve.LastParameter();
        if (Precision::IsInfinite(Cf))  Cf = -1000;
        if (Precision::IsInfinite(Cl))  Cl = +1000;
        dist = ShapeAnalysis_Curve().Project(anIsoCurve, P3d, preci, pntres, other);
        if (dist < theMin) {
          theMin = dist;
          if (UV) VV = other;  else UU = other;
        }
        UV = !UV;
      if (UV) {
        par = UU;
        anIsoCurve.Load(GeomAbs_IsoU,UU);
      }
      else    {
        par = VV;
        anIsoCurve.Load(GeomAbs_IsoV,VV);
      }
        Cf = anIsoCurve.FirstParameter();
        Cl = anIsoCurve.LastParameter();
        if (Precision::IsInfinite(Cf))  Cf = -1000;
        if (Precision::IsInfinite(Cl))  Cl = +1000;
        dist = ShapeAnalysis_Curve().ProjectAct (anIsoCurve, P3d, preci, pntres, other);
        if (dist < theMin) {
          theMin = dist;
          if (UV) VV = other;  else UU = other;
        }
        UV = !UV;
        Iters++;
      }
    }

    U = UU;  V = VV;

  }  // fin try RAJOUT
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
//:s5
    cout << "\nWarning: ShapeAnalysis_Curve::UVFromIso(): Exception: "; 
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    theMin = RealLast();    // theMin de depart
  }
  return theMin;
}


//=======================================================================
//function : SortSingularities
//purpose  :
//=======================================================================

void ShapeAnalysis_Surface::SortSingularities()
{
  for (Standard_Integer i = 0; i < myNbDeg - 1; i++) {
    Standard_Real minPreci = myPreci[i];
    Standard_Integer minIndex = i;
    for (Standard_Integer j = i + 1; j < myNbDeg; j++)
      if (minPreci > myPreci[j]) { minPreci = myPreci[j]; minIndex = j; }
    if (minIndex != i) {
      myPreci[minIndex] = myPreci[i]; myPreci[i] = minPreci;
      gp_Pnt tmpP3d = myP3d[minIndex];
      myP3d[minIndex] = myP3d[i]; myP3d[i] = tmpP3d;
      gp_Pnt2d tmpP2d = myFirstP2d[minIndex];
      myFirstP2d[minIndex] = myFirstP2d[i]; myFirstP2d[i] = tmpP2d;
      tmpP2d = myLastP2d[minIndex]; myLastP2d[minIndex] = myLastP2d[i]; myLastP2d[i] = tmpP2d;
      Standard_Real tmpPar = myFirstPar[minIndex];
      myFirstPar[minIndex] = myFirstPar[i]; myFirstPar[i] = tmpPar;
      tmpPar = myLastPar[minIndex]; myLastPar[minIndex] = myLastPar[i]; myLastPar[i] = tmpPar;
      Standard_Boolean tmpUIsoDeg = myUIsoDeg[minIndex];
      myUIsoDeg[minIndex] = myUIsoDeg[i]; myUIsoDeg[i] = tmpUIsoDeg; 
    }
  }      
}


//=======================================================================
//function : SetDomain
//purpose  : 
//=======================================================================

void ShapeAnalysis_Surface::SetDomain(const Standard_Real U1,
                                      const Standard_Real U2,
                                      const Standard_Real V1,
                                      const Standard_Real V2)
{
  myUF = U1;
  myUL = U2;
  myVF = V1;
  myVL = V2;
}


void ShapeAnalysis_Surface::ComputeBoxes()
{
  if(myIsoBoxes) return;
  myIsoBoxes = Standard_True;
  ComputeBoundIsos();
  if(!myIsoUF.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoUF),Precision::Confusion(),myBndUF);
  if(!myIsoUL.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoUL),Precision::Confusion(),myBndUL);
  if(!myIsoVF.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoVF),Precision::Confusion(),myBndVF);
  if(!myIsoVL.IsNull())
    BndLib_Add3dCurve::Add(GeomAdaptor_Curve(myIsoVL),Precision::Confusion(),myBndVL);            
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxUF() 
{
  ComputeBoxes();
  return myBndUF;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxUL() 
{
  ComputeBoxes();
  return myBndUL;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxVF() 
{
  ComputeBoxes();
  return myBndVF;
}

const Bnd_Box& ShapeAnalysis_Surface::GetBoxVL() 
{
  ComputeBoxes();
  return myBndVL;
}
