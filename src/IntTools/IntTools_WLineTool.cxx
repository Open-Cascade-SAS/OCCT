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

#include <IntTools_WLineTool.hxx>

#include <BRep_Tool.hxx>
#include <Extrema_ExtCC.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Surface.hxx>
#include <gp_Circ.hxx>
#include <IntTools_Context.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

/////////////////////// NotUseSurfacesForApprox /////////////////////////

// The block is dedicated to determine whether WLine [ifprm, ilprm]
// crosses the degenerated zone on each given surface or not.
// If Yes -> We will not use info about surfaces during approximation
// because inside degenerated zone of the surface the approx. algo.
// uses wrong values of normal, etc., and resulting curve will have
// oscillations that we would not like to have. 

//=======================================================================
//function : IsDegeneratedZone
//purpose  : static subfunction in IsDegeneratedZone
//=======================================================================
static
Standard_Boolean IsDegeneratedZone(const gp_Pnt2d& aP2d,
                                   const Handle(Geom_Surface)& aS,
                                   const Standard_Integer iDir)
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US1, US2, VS1, VS2, dY, dX, d1, d2, dD;
  Standard_Real aXm, aYm, aXb, aYb, aXe, aYe;
  aS->Bounds(US1, US2, VS1, VS2); 

  gp_Pnt aPm, aPb, aPe;
  
  aXm=aP2d.X();
  aYm=aP2d.Y();
  
  aS->D0(aXm, aYm, aPm); 
  
  dX=1.e-5;
  dY=1.e-5;
  dD=1.e-12;

  if (iDir==1) {
    aXb=aXm;
    aXe=aXm;
    aYb=aYm-dY;
    if (aYb < VS1) {
      aYb=VS1;
    }
    aYe=aYm+dY;
    if (aYe > VS2) {
      aYe=VS2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  //
  else if (iDir==2) {
    aYb=aYm;
    aYe=aYm;
    aXb=aXm-dX;
    if (aXb < US1) {
      aXb=US1;
    }
    aXe=aXm+dX;
    if (aXe > US2) {
      aXe=US2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  return !bFlag;
}

//=======================================================================
//function : IsPointInDegeneratedZone
//purpose  : static subfunction in NotUseSurfacesForApprox
//=======================================================================
static
Standard_Boolean IsPointInDegeneratedZone(const IntSurf_PntOn2S& aP2S,
                                          const TopoDS_Face& aF1,
                                          const TopoDS_Face& aF2)
                                          
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US11, US12, VS11, VS12, US21, US22, VS21, VS22;
  Standard_Real U1, V1, U2, V2, aDelta, aD;
  gp_Pnt2d aP2d;

  Handle(Geom_Surface)aS1 = BRep_Tool::Surface(aF1);
  aS1->Bounds(US11, US12, VS11, VS12);
  GeomAdaptor_Surface aGAS1(aS1);

  Handle(Geom_Surface)aS2 = BRep_Tool::Surface(aF2);
  aS1->Bounds(US21, US22, VS21, VS22);
  GeomAdaptor_Surface aGAS2(aS2);
  //
  //const gp_Pnt& aP=aP2S.Value();
  aP2S.Parameters(U1, V1, U2, V2);
  //
  aDelta=1.e-7;
  // Check on Surf 1
  aD=aGAS1.UResolution(aDelta);
  aP2d.SetCoord(U1, V1);
  if (fabs(U1-US11) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U1-US12) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS1.VResolution(aDelta);
  if (fabs(V1-VS11) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(V1-VS12) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  // Check on Surf 2
  aD=aGAS2.UResolution(aDelta);
  aP2d.SetCoord(U2, V2);
  if (fabs(U2-US21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U2-US22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS2.VResolution(aDelta);
  if (fabs(V2-VS21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {  
      return bFlag;
    }
  }
  if (fabs(V2-VS22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  return !bFlag;
}

//=======================================================================
//function :  NotUseSurfacesForApprox
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_WLineTool::NotUseSurfacesForApprox(const TopoDS_Face& aF1,
                                                             const TopoDS_Face& aF2,
                                                             const Handle(IntPatch_WLine)& WL,
                                                             const Standard_Integer ifprm,
                                                             const Standard_Integer ilprm)
{
  Standard_Boolean bPInDZ;

  Handle(IntSurf_LineOn2S) aLineOn2S=WL->Curve();
  
  const IntSurf_PntOn2S& aP2Sfprm=aLineOn2S->Value(ifprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Sfprm, aF1, aF2);
  if (bPInDZ) {
    return bPInDZ;
  }

  const IntSurf_PntOn2S& aP2Slprm=aLineOn2S->Value(ilprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Slprm, aF1, aF2);
  
  return bPInDZ;
}

/////////////////////// end of NotUseSurfacesForApprox //////////////////

/////////////////////// DecompositionOfWLine ////////////////////////////

//=======================================================================
//function : CheckTangentZonesExist
//purpose  : static subfunction in ComputeTangentZones
//=======================================================================
static
Standard_Boolean CheckTangentZonesExist(const Handle(GeomAdaptor_HSurface)& theSurface1,
                                        const Handle(GeomAdaptor_HSurface)& theSurface2)
{
  if ( ( theSurface1->GetType() != GeomAbs_Torus ) ||
       ( theSurface2->GetType() != GeomAbs_Torus ) )
    return Standard_False;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  if ( aTor1.Location().Distance( aTor2.Location() ) > Precision::Confusion() )
    return Standard_False;

  if ( ( fabs( aTor1.MajorRadius() - aTor2.MajorRadius() ) > Precision::Confusion() ) ||
       ( fabs( aTor1.MinorRadius() - aTor2.MinorRadius() ) > Precision::Confusion() ) )
    return Standard_False;

  if ( ( aTor1.MajorRadius() < aTor1.MinorRadius() ) ||
       ( aTor2.MajorRadius() < aTor2.MinorRadius() ) )
    return Standard_False;

  return Standard_True;
}


//=======================================================================
//function : ComputeTangentZones
//purpose  : static subfunction in DecompositionOfWLine
//=======================================================================
static
Standard_Integer ComputeTangentZones( const Handle(GeomAdaptor_HSurface)& theSurface1,
                                     const Handle(GeomAdaptor_HSurface)&  theSurface2,
                                     const TopoDS_Face&                   theFace1,
                                     const TopoDS_Face&                   theFace2,
                                     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS1,
                                     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS2,
                                     Handle(TColStd_HArray1OfReal)&       theResultRadius,
                                     const Handle(IntTools_Context)& aContext)
{
  Standard_Integer aResult = 0;
  if ( !CheckTangentZonesExist( theSurface1, theSurface2 ) )
    return aResult;


  TColgp_SequenceOfPnt2d aSeqResultS1, aSeqResultS2;
  TColStd_SequenceOfReal aSeqResultRad;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  gp_Ax2 anax1( aTor1.Location(), aTor1.Axis().Direction() );
  gp_Ax2 anax2( aTor2.Location(), aTor2.Axis().Direction() );
  Standard_Integer j = 0;

  for ( j = 0; j < 2; j++ ) {
    Standard_Real aCoef = ( j == 0 ) ? -1 : 1;
    Standard_Real aRadius1 = fabs(aTor1.MajorRadius() + aCoef * aTor1.MinorRadius());
    Standard_Real aRadius2 = fabs(aTor2.MajorRadius() + aCoef * aTor2.MinorRadius());

    gp_Circ aCircle1( anax1, aRadius1 );
    gp_Circ aCircle2( anax2, aRadius2 );

    // roughly compute radius of tangent zone for perpendicular case
    Standard_Real aCriteria = Precision::Confusion() * 0.5;

    Standard_Real aT1 = aCriteria;
    Standard_Real aT2 = aCriteria;
    if ( j == 0 ) {
      // internal tangency
      Standard_Real aR = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      //aT1 = aCriteria * aCriteria + aR * aR - ( aR - aCriteria ) * ( aR - aCriteria );
      aT1 = 2. * aR * aCriteria;
      aT2 = aT1;
    }
    else {
      // external tangency
      Standard_Real aRb = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aRm = ( aRadius1 < aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aDelta = aRb - aCriteria;
      aDelta *= aDelta;
      aDelta -= aRm * aRm;
      aDelta /= 2. * (aRb - aRm);
      aDelta -= 0.5 * (aRb - aRm);
      
      aT1 = 2. * aRm * (aRm - aDelta);
      aT2 = aT1;
    }
    aCriteria = ( aT1 > aT2) ? aT1 : aT2;
    if ( aCriteria > 0 )
      aCriteria = sqrt( aCriteria );

    if ( aCriteria > 0.5 * aTor1.MinorRadius() ) {
      // too big zone -> drop to minimum
      aCriteria = Precision::Confusion();
    }

    GeomAdaptor_Curve aC1( new Geom_Circle(aCircle1) );
    GeomAdaptor_Curve aC2( new Geom_Circle(aCircle2) );
    Extrema_ExtCC anExtrema(aC1, aC2, 0, 2. * M_PI, 0, 2. * M_PI, 
                            Precision::PConfusion(), Precision::PConfusion());
            
    if ( anExtrema.IsDone() ) {

      Standard_Integer i = 0;
      for ( i = 1; i <= anExtrema.NbExt(); i++ ) {
        if ( anExtrema.SquareDistance(i) > aCriteria * aCriteria )
          continue;

        Extrema_POnCurv P1, P2;
        anExtrema.Points( i, P1, P2 );

        Standard_Boolean bFoundResult = Standard_True;
        gp_Pnt2d pr1, pr2;

        Standard_Integer surfit = 0;
        for ( surfit = 0; surfit < 2; surfit++ ) {
          GeomAPI_ProjectPointOnSurf& aProjector = 
            (surfit == 0) ? aContext->ProjPS(theFace1) : aContext->ProjPS(theFace2);

          gp_Pnt aP3d = (surfit == 0) ? P1.Value() : P2.Value();
          aProjector.Perform(aP3d);

          if(!aProjector.IsDone())
            bFoundResult = Standard_False;
          else {
            if(aProjector.LowerDistance() > aCriteria) {
              bFoundResult = Standard_False;
            }
            else {
              Standard_Real foundU = 0, foundV = 0;
              aProjector.LowerDistanceParameters(foundU, foundV);
              if ( surfit == 0 )
                pr1 = gp_Pnt2d( foundU, foundV );
              else
                pr2 = gp_Pnt2d( foundU, foundV );
            }
          }
        }
        if ( bFoundResult ) {
          aSeqResultS1.Append( pr1 );
          aSeqResultS2.Append( pr2 );
          aSeqResultRad.Append( aCriteria );

          // torus is u and v periodic
          const Standard_Real twoPI = M_PI + M_PI;
          Standard_Real arr1tmp[2] = {pr1.X(), pr1.Y()};
          Standard_Real arr2tmp[2] = {pr2.X(), pr2.Y()};

          // iteration on period bounds
          for ( Standard_Integer k1 = 0; k1 < 2; k1++ ) {
            Standard_Real aBound = ( k1 == 0 ) ? 0 : twoPI;
            Standard_Real aShift = ( k1 == 0 ) ? twoPI : -twoPI;

            // iteration on surfaces
            for ( Standard_Integer k2 = 0; k2 < 2; k2++ ) {
              Standard_Real* arr1 = ( k2 == 0 ) ? arr1tmp : arr2tmp;
              Standard_Real* arr2 = ( k2 != 0 ) ? arr1tmp : arr2tmp;
              TColgp_SequenceOfPnt2d& aSeqS1 = ( k2 == 0 ) ? aSeqResultS1 : aSeqResultS2; 
              TColgp_SequenceOfPnt2d& aSeqS2 = ( k2 != 0 ) ? aSeqResultS1 : aSeqResultS2; 

              if (fabs(arr1[0] - aBound) < Precision::PConfusion()) {
                aSeqS1.Append( gp_Pnt2d( arr1[0] + aShift, arr1[1] ) );
                aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
                aSeqResultRad.Append( aCriteria );
              }
              if (fabs(arr1[1] - aBound) < Precision::PConfusion()) {
                aSeqS1.Append( gp_Pnt2d( arr1[0], arr1[1] + aShift) );
                aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
                aSeqResultRad.Append( aCriteria );
              }
            }
          } //
        }
      }
    }
  }
  aResult = aSeqResultRad.Length();

  if ( aResult > 0 ) {
    theResultOnS1 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultOnS2 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultRadius = new TColStd_HArray1OfReal( 1, aResult );

    for ( Standard_Integer i = 1 ; i <= aResult; i++ ) {
      theResultOnS1->SetValue( i, aSeqResultS1.Value(i) );
      theResultOnS2->SetValue( i, aSeqResultS2.Value(i) );
      theResultRadius->SetValue( i, aSeqResultRad.Value(i) );
    }
  }
  return aResult;
}

//=======================================================================
//function : IsPointOnBoundary
//purpose  : static subfunction in DecompositionOfWLine
//=======================================================================
static
Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
                                   const Standard_Real theFirstBoundary,
                                   const Standard_Real theSecondBoundary,
                                   const Standard_Real theResolution,
                                   Standard_Boolean&   IsOnFirstBoundary) 
{
  Standard_Boolean bRet;
  Standard_Integer i;
  Standard_Real adist;
  //
  bRet=Standard_False;
  for(i = 0; i < 2; ++i) {
    IsOnFirstBoundary = (i == 0);
    if (IsOnFirstBoundary) {
      adist = fabs(theParameter - theFirstBoundary);
    }
    else {
      adist = fabs(theParameter - theSecondBoundary);
    }
    if(adist < theResolution) {
      return !bRet;
    }
  }
  return bRet;
}

//=======================================================================
//function : IsInsideTanZone
//purpose  : Check if point is inside a radial tangent zone.
//           static subfunction in DecompositionOfWLine and FindPoint
//=======================================================================
static
Standard_Boolean IsInsideTanZone(const gp_Pnt2d&     thePoint,
                                 const gp_Pnt2d&     theTanZoneCenter,
                                 const Standard_Real theZoneRadius,
                                 Handle(GeomAdaptor_HSurface) theGASurface)
{
  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );
  Standard_Real aRadiusSQR = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  aRadiusSQR *= aRadiusSQR;
  if ( thePoint.SquareDistance( theTanZoneCenter ) <= aRadiusSQR )
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : AdjustByNeighbour
//purpose  : static subfunction in DecompositionOfWLine
//=======================================================================
static
gp_Pnt2d AdjustByNeighbour(const gp_Pnt2d&     theaNeighbourPoint,
                           const gp_Pnt2d&     theOriginalPoint,
                           Handle(GeomAdaptor_HSurface) theGASurface)
{
  gp_Pnt2d ap1 = theaNeighbourPoint;
  gp_Pnt2d ap2 = theOriginalPoint;

  if ( theGASurface->IsUPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->UPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetX( theOriginalPoint.X() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
        ap2 = aPTest;
        aSqDistMin = dd;
      }
    }
  }
  if ( theGASurface->IsVPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->VPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetY( theOriginalPoint.Y() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
        ap2 = aPTest;
        aSqDistMin = dd;
      }
    }
  }
  return ap2;
}

//=======================================================================
//function : RefineVector
//purpose  : static subfunction in FindPoint
//=======================================================================
static
void RefineVector(gp_Vec2d& aV2D)
{
  Standard_Integer k,m;
  Standard_Real aC[2], aEps, aR1, aR2, aNum;
  //
  aEps=RealEpsilon();
  aR1=1.-aEps;
  aR2=1.+aEps;
  //
  aV2D.Coord(aC[0], aC[1]);
  //
  for (k=0; k<2; ++k) {
    m=(k+1)%2;
    aNum=fabs(aC[k]);
    if (aNum>aR1 && aNum<aR2) {
      if (aC[k]<0.) {
        aC[k]=-1.;
      }          
      else {
        aC[k]=1.;
      }
      aC[m]=0.;
      break;
    }
  }
  aV2D.SetCoord(aC[0], aC[1]);
}

//=======================================================================
//function : FindPoint
//purpose  : static subfunction in DecompositionOfWLine
//=======================================================================
static
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                           const gp_Pnt2d&     theLastPoint,
                           const Standard_Real theUmin, 
                           const Standard_Real theUmax,
                           const Standard_Real theVmin,
                           const Standard_Real theVmax,
                           gp_Pnt2d&           theNewPoint)
{
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  Standard_Integer i = 0, j = 0;

  for(i = 0; i < 4; i++) {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;    

    if((i % 2) == 0) {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if(i < 2)
        aprojpoint.SetX(theUmin);
      else
        aprojpoint.SetX(theUmax);
    }
    else {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if(i < 2)
        aprojpoint.SetY(theVmin);
      else
        aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    RefineVector(anormvec);
    Standard_Real adot1 = anormvec.Dot(anOtherVecNormal);

    if(fabs(adot1) < Precision::Angular())
      continue;
    Standard_Real adist = 0.;
    Standard_Boolean bIsOut = Standard_False;

    if((i % 2) == 0) {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
      bIsOut = (i < 2) ? (theLastPoint.X() < theUmin) : (theLastPoint.X() > theUmax);
    }
    else {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
      bIsOut = (i < 2) ? (theLastPoint.Y() < theVmin) : (theLastPoint.Y() > theVmax);
    }
    Standard_Real anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for(j = 0; j < 2; j++) {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY()*anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);
      if ( bIsOut )
        acurvec.Reverse();

      Standard_Real aDotX, anAngleX;
      //
      aDotX = aVec.Dot(acurvec);
      anAngleX = aVec.Angle(acurvec);
      //
      if(aDotX > 0. && fabs(anAngleX) < Precision::PConfusion()) {
        if((i % 2) == 0) {
          if((acurpoint.Y() >= theVmin) &&
             (acurpoint.Y() <= theVmax)) {
            theNewPoint = acurpoint;
            return Standard_True;
          }
        }
        else {
          if((acurpoint.X() >= theUmin) &&
             (acurpoint.X() <= theUmax)) {
            theNewPoint = acurpoint;
            return Standard_True;
          }
        }
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : FindPoint
//purpose  : Find point on the boundary of radial tangent zone
//           static subfunction in DecompositionOfWLine
//=======================================================================
static
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                           const gp_Pnt2d&     theLastPoint,
                           const Standard_Real theUmin, 
                           const Standard_Real theUmax,
                           const Standard_Real theVmin,
                           const Standard_Real theVmax,
                           const gp_Pnt2d&     theTanZoneCenter,
                           const Standard_Real theZoneRadius,
                           Handle(GeomAdaptor_HSurface) theGASurface,
                           gp_Pnt2d&           theNewPoint) {
  theNewPoint = theLastPoint;

  if ( !IsInsideTanZone( theLastPoint, theTanZoneCenter, theZoneRadius, theGASurface) )
    return Standard_False;

  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );

  Standard_Real aRadius = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  gp_Ax22d anAxis( theTanZoneCenter, gp_Dir2d(1, 0), gp_Dir2d(0, 1) );
  gp_Circ2d aCircle( anAxis, aRadius );
  
  //
  gp_Vec2d aDir( theLastPoint.XY() - theFirstPoint.XY() );
  Standard_Real aLength = aDir.Magnitude();
  if ( aLength <= gp::Resolution() )
    return Standard_False;
  gp_Lin2d aLine( theFirstPoint, aDir );

  //
  Handle(Geom2d_Line) aCLine = new Geom2d_Line( aLine );
  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve( aCLine, 0, aLength );
  Handle(Geom2d_Circle) aC2 = new Geom2d_Circle( aCircle );

  Standard_Real aTol = aRadius * 0.001;
  aTol = ( aTol < Precision::PConfusion() ) ? Precision::PConfusion() : aTol;

  Geom2dAPI_InterCurveCurve anIntersector;
  anIntersector.Init( aC1, aC2, aTol );

  if ( anIntersector.NbPoints() == 0 )
    return Standard_False;

  Standard_Boolean aFound = Standard_False;
  Standard_Real aMinDist = aLength * aLength;
  Standard_Integer i = 0;
  for ( i = 1; i <= anIntersector.NbPoints(); i++ ) {
    gp_Pnt2d aPInt = anIntersector.Point( i );
    if ( aPInt.SquareDistance( theFirstPoint ) < aMinDist ) {
      if ( ( aPInt.X() >= theUmin ) && ( aPInt.X() <= theUmax ) &&
           ( aPInt.Y() >= theVmin ) && ( aPInt.Y() <= theVmax ) ) {
        theNewPoint = aPInt;
        aFound = Standard_True;
      }
    }
  }

  return aFound;
}

//=======================================================================
//function : DecompositionOfWLine
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_WLineTool::
  DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                       const Handle(GeomAdaptor_HSurface)&            theSurface1, 
                       const Handle(GeomAdaptor_HSurface)&            theSurface2,
                       const TopoDS_Face&                             theFace1,
                       const TopoDS_Face&                             theFace2,
                       const GeomInt_LineConstructor&                 theLConstructor,
                       const Standard_Boolean                         theAvoidLConstructor,
                       const Standard_Real                            theTol,
                       IntPatch_SequenceOfLine&                       theNewLines,
                       Standard_Real&                                 theReachedTol3d,
                       const Handle(IntTools_Context)& aContext) 
{
  Standard_Boolean bRet, bAvoidLineConstructor;
  Standard_Integer aNbPnts, aNbParts;
  //
  bRet=Standard_False;
  aNbPnts=theWLine->NbPnts();
  bAvoidLineConstructor=theAvoidLConstructor;
  //
  if(!aNbPnts){
    return bRet;
  }
  if (!bAvoidLineConstructor) {
    aNbParts=theLConstructor.NbParts();
    if (!aNbParts) {
      return bRet;
    }
  }
  //
  Standard_Boolean bIsPrevPointOnBoundary, bIsPointOnBoundary, bIsCurrentPointOnBoundary;
  Standard_Integer nblines, pit, i, j;
  Standard_Real aTol;
  TColStd_Array1OfListOfInteger anArrayOfLines(1, aNbPnts); 
  TColStd_Array1OfInteger       anArrayOfLineType(1, aNbPnts);
  TColStd_ListOfInteger aListOfPointIndex;
  
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS1;
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS2;
  Handle(TColStd_HArray1OfReal) aTanZoneRadius;
  Standard_Integer aNbZone = ComputeTangentZones( theSurface1, theSurface2, theFace1, theFace2,
                                                 aTanZoneS1, aTanZoneS2, aTanZoneRadius, aContext);
  
  //
  nblines=0;
  aTol=Precision::Confusion();
  aTol=0.5*aTol;
  bIsPrevPointOnBoundary=Standard_False;
  bIsPointOnBoundary=Standard_False;
  //
  // 1. ...
  //
  // Points
  for(pit = 1; pit <= aNbPnts; ++pit) {
    Standard_Boolean bIsOnFirstBoundary, isperiodic;
    Standard_Real aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
    Standard_Real aParameter, anoffset, anAdjustPar;
    Standard_Real umin, umax, vmin, vmax;
    //
    bIsCurrentPointOnBoundary = Standard_False;
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    //
    // Surface
    for(i = 0; i < 2; ++i) {
      Handle(GeomAdaptor_HSurface) aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
      if(!i) {
        aPoint.ParametersOnS1(U, V);
      }
      else {
        aPoint.ParametersOnS2(U, V);
      }
      // U, V
      for(j = 0; j < 2; j++) {
        isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
        if(!isperiodic){
          continue;
        }
        //
        if (!j) {
          aResolution=aGASurface->UResolution(aTol);
          aPeriod=aGASurface->UPeriod();
          alowerboundary=umin;
          aupperboundary=umax;
          aParameter=U;
        }
        else {
          aResolution=aGASurface->VResolution(aTol);
          aPeriod=aGASurface->VPeriod();
          alowerboundary=vmin;
          aupperboundary=vmax;
          aParameter=V;
        }
        
        GeomInt::AdjustPeriodic(aParameter, 
                                       alowerboundary, 
                                       aupperboundary, 
                                       aPeriod,
                                       anAdjustPar,
                                       anoffset);
        //
        bIsOnFirstBoundary = Standard_True;// ?
        bIsPointOnBoundary=
          IsPointOnBoundary(anAdjustPar, 
                            alowerboundary, 
                            aupperboundary,
                            aResolution, 
                            bIsOnFirstBoundary);
        //
        if(bIsPointOnBoundary) {
          bIsCurrentPointOnBoundary = Standard_True;
          break;
        }
        else {
          // check if a point belong to a tangent zone. Begin
          Standard_Integer zIt = 0;
          for ( zIt = 1; zIt <= aNbZone; zIt++ ) {
            gp_Pnt2d aPZone = (i == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
            Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

            if ( IsInsideTanZone(gp_Pnt2d( U, V ), aPZone, aZoneRadius, aGASurface ) ) {
              // set boundary flag to split the curve by a tangent zone
              bIsPointOnBoundary = Standard_True;
              bIsCurrentPointOnBoundary = Standard_True;
              if ( theReachedTol3d < aZoneRadius ) {
                theReachedTol3d = aZoneRadius;
              }
              break;
            }
          }
        }
      }//for(j = 0; j < 2; j++) {

      if(bIsCurrentPointOnBoundary){
        break;
      }
    }//for(i = 0; i < 2; ++i) {
    //
    if((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary)) {
      if(!aListOfPointIndex.IsEmpty()) {
        nblines++;
        anArrayOfLines.SetValue(nblines, aListOfPointIndex);
        anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
        aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } //for(pit = 1; pit <= aNbPnts; ++pit) {
  //
  if(!aListOfPointIndex.IsEmpty()) {
    nblines++;
    anArrayOfLines.SetValue(nblines, aListOfPointIndex);
    anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
    aListOfPointIndex.Clear();
  }
  //
  if(nblines<=1) {
    return bRet; //Standard_False;
  }
  //
  // 
  // 2. Correct wlines.begin
  TColStd_Array1OfListOfInteger anArrayOfLineEnds(1, nblines);
  Handle(IntSurf_LineOn2S) aSeqOfPntOn2S = new IntSurf_LineOn2S();
  //
  for(i = 1; i <= nblines; i++) {
    if(anArrayOfLineType.Value(i) != 0) {
      continue;
    }
    const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);
    TColStd_ListOfInteger aListOfFLIndex;

    for(j = 0; j < 2; j++) {
      Standard_Integer aneighbourindex = (j == 0) ? (i - 1) : (i + 1);

      if((aneighbourindex < 1) || (aneighbourindex > nblines))
        continue;

      if(anArrayOfLineType.Value(aneighbourindex) == 0)
        continue;
      const TColStd_ListOfInteger& aNeighbour = anArrayOfLines.Value(aneighbourindex);
      Standard_Integer anIndex = (j == 0) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint = theWLine->Point(anIndex);

      IntSurf_PntOn2S aNewP = aPoint;
      if(aListOfIndex.Extent() < 2) {
        aSeqOfPntOn2S->Add(aNewP);
        aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
        continue;
      }
      //
      Standard_Integer iFirst = aListOfIndex.First();
      Standard_Integer iLast  = aListOfIndex.Last();
      //
      for(Standard_Integer surfit = 0; surfit < 2; surfit++) {

        Handle(GeomAdaptor_HSurface) aGASurface = (surfit == 0) ? theSurface1 : theSurface2;
        Standard_Real umin=0., umax=0., vmin=0., vmax=0.;
        aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
        Standard_Real U=0., V=0.;

        if(surfit == 0)
          aNewP.ParametersOnS1(U, V);
        else
          aNewP.ParametersOnS2(U, V);
        Standard_Integer nbboundaries = 0;

        Standard_Boolean bIsNearBoundary = Standard_False;
        Standard_Integer aZoneIndex = 0;
        Standard_Integer bIsUBoundary = Standard_False; // use if nbboundaries == 1
        Standard_Integer bIsFirstBoundary = Standard_False; // use if nbboundaries == 1
        

        for(Standard_Integer parit = 0; parit < 2; parit++) {
          Standard_Boolean isperiodic = (parit == 0) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          Standard_Real aResolution = (parit == 0) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
          Standard_Real alowerboundary = (parit == 0) ? umin : vmin;
          Standard_Real aupperboundary = (parit == 0) ? umax : vmax;

          Standard_Real aParameter = (parit == 0) ? U : V;
          Standard_Boolean bIsOnFirstBoundary = Standard_True;
  
          if(!isperiodic) {
            bIsPointOnBoundary=
              IsPointOnBoundary(aParameter, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
            if(bIsPointOnBoundary) {
              bIsUBoundary = (parit == 0);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
          }
          else {
            Standard_Real aPeriod     = (parit == 0) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            Standard_Real anoffset, anAdjustPar;
            GeomInt::AdjustPeriodic(aParameter, alowerboundary, aupperboundary,
                                           aPeriod, anAdjustPar, anoffset);

            bIsPointOnBoundary=
              IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
            if(bIsPointOnBoundary) {
              bIsUBoundary = (parit == 0);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
            else {
            //check neighbourhood of boundary
            Standard_Real anEpsilon = aResolution * 100.;
            Standard_Real aPart = ( aupperboundary - alowerboundary ) * 0.1;
            anEpsilon = ( anEpsilon > aPart ) ? aPart : anEpsilon;
            
              bIsNearBoundary = IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, 
                                                  anEpsilon, bIsOnFirstBoundary);

            }
          }
        }

        // check if a point belong to a tangent zone. Begin
        for ( Standard_Integer zIt = 1; zIt <= aNbZone; zIt++ ) {
          gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
          Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

          Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
          const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
          Standard_Real nU1, nV1;
            
          if(surfit == 0)
            aNeighbourPoint.ParametersOnS1(nU1, nV1);
          else
            aNeighbourPoint.ParametersOnS2(nU1, nV1);
          gp_Pnt2d ap1(nU1, nV1);
          gp_Pnt2d ap2 = AdjustByNeighbour( ap1, gp_Pnt2d( U, V ), aGASurface );


          if ( IsInsideTanZone( ap2, aPZone, aZoneRadius, aGASurface ) ) {
            aZoneIndex = zIt;
            bIsNearBoundary = Standard_True;
            if ( theReachedTol3d < aZoneRadius ) {
              theReachedTol3d = aZoneRadius;
            }
          }
        }
        // check if a point belong to a tangent zone. End
        Standard_Boolean bComputeLineEnd = Standard_False;

        if(nbboundaries == 2) {
          //xf
          bComputeLineEnd = Standard_True;
          //xt
        }
        else if(nbboundaries == 1) {
          Standard_Boolean isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          if(isperiodic) {
            Standard_Real alowerboundary = (bIsUBoundary) ? umin : vmin;
            Standard_Real aupperboundary = (bIsUBoundary) ? umax : vmax;
            Standard_Real aPeriod     = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            Standard_Real aParameter = (bIsUBoundary) ? U : V;
            Standard_Real anoffset, anAdjustPar;
            GeomInt::AdjustPeriodic(aParameter, alowerboundary, aupperboundary, 
                                           aPeriod, anAdjustPar, anoffset);

            Standard_Real adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary) : fabs(anAdjustPar - aupperboundary);
            Standard_Real anotherPar = (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
            anotherPar += anoffset;
            Standard_Integer aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
            Standard_Real nU1, nV1;

            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            
            Standard_Real adist1 = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
            Standard_Real adist2 = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
            bComputeLineEnd = Standard_True;
            Standard_Boolean bCheckAngle1 = Standard_False;
            Standard_Boolean bCheckAngle2 = Standard_False;
            gp_Vec2d aNewVec;
            Standard_Real anewU = (bIsUBoundary) ? anotherPar : U;
            Standard_Real anewV = (bIsUBoundary) ? V : anotherPar;

            if(((adist1 - adist2) > Precision::PConfusion()) && 
               (adist2 < (aPeriod / 4.))) {
              bCheckAngle1 = Standard_True;
              aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

              if(aNewVec.SquareMagnitude() < gp::Resolution()) {
                aNewP.SetValue((surfit == 0), anewU, anewV);
                bCheckAngle1 = Standard_False;
              }
            }
            else if(adist1 < (aPeriod / 4.)) {
              bCheckAngle2 = Standard_True;
              aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

              if(aNewVec.SquareMagnitude() < gp::Resolution()) {
                bCheckAngle2 = Standard_False;
              }
            }

            if(bCheckAngle1 || bCheckAngle2) {
              // assume there are at least two points in line (see "if" above)
              Standard_Integer anindexother = aneighbourpointindex;

              while((anindexother <= iLast) && (anindexother >= iFirst)) {
                anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
                const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
                Standard_Real nU2, nV2;
                
                if(surfit == 0)
                  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
                else
                  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
                gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

                if(aVecOld.SquareMagnitude() <= gp::Resolution()) {
                  continue;
                }
                else {
                  Standard_Real anAngle = aNewVec.Angle(aVecOld);

                  if((fabs(anAngle) < (M_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.)) {

                    if(bCheckAngle1) {
                      Standard_Real U1, U2, V1, V2;
                      IntSurf_PntOn2S atmppoint = aNewP;
                      atmppoint.SetValue((surfit == 0), anewU, anewV);
                      atmppoint.Parameters(U1, V1, U2, V2);
                      gp_Pnt P1 = theSurface1->Value(U1, V1);
                      gp_Pnt P2 = theSurface2->Value(U2, V2);
                      gp_Pnt P0 = aPoint.Value();

                      if(P0.IsEqual(P1, aTol) &&
                         P0.IsEqual(P2, aTol) &&
                         P1.IsEqual(P2, aTol)) {
                        bComputeLineEnd = Standard_False;
                        aNewP.SetValue((surfit == 0), anewU, anewV);
                      }
                    }

                    if(bCheckAngle2) {
                      bComputeLineEnd = Standard_False;
                    }
                  }
                  break;
                }
              } // end while(anindexother...)
            }
          }
        }
        else if ( bIsNearBoundary ) {
          bComputeLineEnd = Standard_True;
        }

        if(bComputeLineEnd) {

          gp_Pnt2d anewpoint;
          Standard_Boolean found = Standard_False;

          if ( bIsNearBoundary ) {
            // re-compute point near natural boundary or near tangent zone
            Standard_Real u1, v1, u2, v2;
            aNewP.Parameters( u1, v1, u2, v2 );
            if(surfit == 0)
              anewpoint = gp_Pnt2d( u1, v1 );
            else
              anewpoint = gp_Pnt2d( u2, v2 );
            
            Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
            Standard_Real nU1, nV1;
            
            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            gp_Pnt2d ap1(nU1, nV1);
            gp_Pnt2d ap2;


            if ( aZoneIndex ) {
              // exclude point from a tangent zone
              anewpoint = AdjustByNeighbour( ap1, anewpoint, aGASurface );
              gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(aZoneIndex) : aTanZoneS2->Value(aZoneIndex);
              Standard_Real aZoneRadius = aTanZoneRadius->Value(aZoneIndex);

              if ( FindPoint(ap1, anewpoint, umin, umax, vmin, vmax, 
                             aPZone, aZoneRadius, aGASurface, ap2) ) {
                anewpoint = ap2;
                found = Standard_True;
              }
            }
            else if ( aGASurface->IsUPeriodic() || aGASurface->IsVPeriodic() ) {
              // re-compute point near boundary if shifted on a period
              ap2 = AdjustByNeighbour( ap1, anewpoint, aGASurface );

              if ( ( ap2.X() < umin ) || ( ap2.X() > umax ) ||
                  ( ap2.Y() < vmin ) || ( ap2.Y() > vmax ) ) {
                found = FindPoint(ap1, ap2, umin, umax, vmin, vmax, anewpoint);
              }
              else {
                anewpoint = ap2;
                aNewP.SetValue( (surfit == 0), anewpoint.X(), anewpoint.Y() );
              }
            }
          }
          else {

            Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
            Standard_Real nU1, nV1;

            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            gp_Pnt2d ap1(nU1, nV1);
            gp_Pnt2d ap2(nU1, nV1);
            Standard_Integer aneighbourpointindex2 = aneighbourpointindex1;

            while((aneighbourpointindex2 <= iLast) && (aneighbourpointindex2 >= iFirst)) {
              aneighbourpointindex2 = (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
              const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
              Standard_Real nU2, nV2;

              if(surfit == 0)
                aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
              else
                aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
              ap2.SetX(nU2);
              ap2.SetY(nV2);

              if(ap1.SquareDistance(ap2) > gp::Resolution()) {
                break;
              }
            }  
            found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);
          }

          if(found) {
            // check point
            Standard_Real aCriteria = theTol;
            GeomAPI_ProjectPointOnSurf& aProjector = 
              (surfit == 0) ? aContext->ProjPS(theFace2) : aContext->ProjPS(theFace1);
            Handle(GeomAdaptor_HSurface) aSurface = (surfit == 0) ? theSurface1 : theSurface2;

            Handle(GeomAdaptor_HSurface) aSurfaceOther = (surfit == 0) ? theSurface2 : theSurface1;

            gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
            aProjector.Perform(aP3d);

            if(aProjector.IsDone()) {
              if(aProjector.LowerDistance() < aCriteria) {
                Standard_Real foundU = U, foundV = V;
                aProjector.LowerDistanceParameters(foundU, foundV);

                //Correction of projected coordinates. Begin
                //Note, it may be shifted on a period
                Standard_Integer aneindex1 = (j == 0) ? iFirst : iLast;
                const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneindex1);
                Standard_Real nUn, nVn;
                
                if(surfit == 0)
                  aNeighbourPoint.ParametersOnS2(nUn, nVn);
                else
                  aNeighbourPoint.ParametersOnS1(nUn, nVn);
                gp_Pnt2d aNeighbour2d(nUn, nVn);
                gp_Pnt2d anAdjustedPoint = AdjustByNeighbour( aNeighbour2d, gp_Pnt2d(foundU, foundV), aSurfaceOther );
                foundU = anAdjustedPoint.X();
                foundV = anAdjustedPoint.Y();

                if ( ( anAdjustedPoint.X() < umin ) && ( anAdjustedPoint.X() > umax ) &&
                    ( anAdjustedPoint.Y() < vmin ) && ( anAdjustedPoint.Y() > vmax ) ) {
                  // attempt to roughly re-compute point
                  foundU = ( foundU < umin ) ? umin : foundU;
                  foundU = ( foundU > umax ) ? umax : foundU;
                  foundV = ( foundV < vmin ) ? vmin : foundV;
                  foundV = ( foundV > vmax ) ? vmax : foundV;

                  GeomAPI_ProjectPointOnSurf& aProjector2 = 
                    (surfit == 0) ? aContext->ProjPS(theFace1) : aContext->ProjPS(theFace2);

                  aP3d = aSurfaceOther->Value(foundU, foundV);
                  aProjector2.Perform(aP3d);
                  
                  if(aProjector2.IsDone()) {
                    if(aProjector2.LowerDistance() < aCriteria) {
                      Standard_Real foundU2 = anewpoint.X(), foundV2 = anewpoint.Y();
                      aProjector2.LowerDistanceParameters(foundU2, foundV2);
                      anewpoint.SetX(foundU2);
                      anewpoint.SetY(foundV2);
                    }
                  }
                }
                //Correction of projected coordinates. End

                if(surfit == 0)
                  aNewP.SetValue(aP3d, anewpoint.X(), anewpoint.Y(), foundU, foundV);
                else
                  aNewP.SetValue(aP3d, foundU, foundV, anewpoint.X(), anewpoint.Y());
              }
            }
          }
        }
      }
      aSeqOfPntOn2S->Add(aNewP);
      aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
    }
    anArrayOfLineEnds.SetValue(i, aListOfFLIndex);
  }
  // Correct wlines.end

  // Split wlines.begin
  Standard_Integer nbiter;
  //
  nbiter=1;
  if (!bAvoidLineConstructor) {
    nbiter=theLConstructor.NbParts();
  }
  //
  for(j = 1; j <= nbiter; ++j) {
    Standard_Real fprm, lprm;
    Standard_Integer ifprm, ilprm;
    //
    if(bAvoidLineConstructor) {
      ifprm = 1;
      ilprm = theWLine->NbPnts();
    }
    else {
      theLConstructor.Part(j, fprm, lprm);
      ifprm = (Standard_Integer)fprm;
      ilprm = (Standard_Integer)lprm;
    }

    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
    //
    for(i = 1; i <= nblines; i++) {
      if(anArrayOfLineType.Value(i) != 0) {
        continue;
      }
      const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);
      const TColStd_ListOfInteger& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      Standard_Boolean bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      Standard_Boolean bhaslastpoint = (aListOfFLIndex.Extent() == 2);

      if(!bhasfirstpoint && !aListOfFLIndex.IsEmpty()) {
        bhasfirstpoint = (i != 1);
      }

      if(!bhaslastpoint && !aListOfFLIndex.IsEmpty()) {
        bhaslastpoint = (i != nblines);
      }
      
      Standard_Integer iFirst = aListOfIndex.First();
      Standard_Integer iLast  = aListOfIndex.Last();
      Standard_Boolean bIsFirstInside = ((ifprm >= iFirst) && (ifprm <= iLast));
      Standard_Boolean bIsLastInside =  ((ilprm >= iFirst) && (ilprm <= iLast));

      if(!bIsFirstInside && !bIsLastInside) {
        if((ifprm < iFirst) && (ilprm > iLast)) {
          // append whole line, and boundaries if neccesary
          if(bhasfirstpoint) {
            pit = aListOfFLIndex.First();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }

          if(bhaslastpoint) {
            pit = aListOfFLIndex.Last();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }

          // check end of split line (end is almost always)
          Standard_Integer aneighbour = i + 1;
          Standard_Boolean bIsEndOfLine = Standard_True;

          if(aneighbour <= nblines) {
            const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

            if((anArrayOfLineType.Value(aneighbour) != 0) &&
               (aListOfNeighbourIndex.IsEmpty())) {
              bIsEndOfLine = Standard_False;
            }
          }

          if(bIsEndOfLine) {
            if(aLineOn2S->NbPoints() > 1) {
              Handle(IntPatch_WLine) aNewWLine = 
                new IntPatch_WLine(aLineOn2S, Standard_False);
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if(bIsFirstInside && bIsLastInside) {
        // append inside points between ifprm and ilprm
        TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

        for(; anIt.More(); anIt.Next()) {
          pit = anIt.Value();
          if((pit < ifprm) || (pit > ilprm))
            continue;
          const IntSurf_PntOn2S& aP = theWLine->Point(pit);
          aLineOn2S->Add(aP);
        }
      }
      else {

        if(bIsFirstInside) {
          // append points from ifprm to last point + boundary point
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            if(pit < ifprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }

          if(bhaslastpoint) {
            pit = aListOfFLIndex.Last();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          // check end of split line (end is almost always)
          Standard_Integer aneighbour = i + 1;
          Standard_Boolean bIsEndOfLine = Standard_True;

          if(aneighbour <= nblines) {
            const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

            if((anArrayOfLineType.Value(aneighbour) != 0) &&
               (aListOfNeighbourIndex.IsEmpty())) {
              bIsEndOfLine = Standard_False;
            }
          }

          if(bIsEndOfLine) {
            if(aLineOn2S->NbPoints() > 1) {
              Handle(IntPatch_WLine) aNewWLine = 
                new IntPatch_WLine(aLineOn2S, Standard_False);
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        // end if(bIsFirstInside)

        if(bIsLastInside) {
          // append points from first boundary point to ilprm
          if(bhasfirstpoint) {
            pit = aListOfFLIndex.First();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            if(pit > ilprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }
        }
        //end if(bIsLastInside)
      }
    }

    if(aLineOn2S->NbPoints() > 1) {
      Handle(IntPatch_WLine) aNewWLine = 
        new IntPatch_WLine(aLineOn2S, Standard_False);
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end

  return Standard_True;
}

///////////////////// end of DecompositionOfWLine ///////////////////////