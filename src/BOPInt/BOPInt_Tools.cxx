// Created by: Peter KURNEV
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

#include <BOPInt_Tools.ixx>

#include <gp_Pnt.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>

//=======================================================================
//function : CheckCurve
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Tools::CheckCurve(const Handle (Geom_Curve)& aC3D,
					    const Standard_Real aTolR3D,
					    Bnd_Box& aBox)
{
  Standard_Boolean bRet;  
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dX, dY, dZ;
  Standard_Real dS, aTol;
  GeomAdaptor_Curve aGAC;
  //
  aGAC.Load(aC3D);
  BndLib_Add3dCurve::Add(aGAC, aTolR3D, aBox);
  // 910/B1
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  dX=aXmax-aXmin;
  dY=aYmax-aYmin;
  dZ=aZmax-aZmin;
  dS=1.e-12;
  aTol=2.*aTolR3D+dS;
  bRet=(dX>aTol ||  dY>aTol || dZ>aTol);
  //
  return bRet;
}


//=======================================================================
//function : IsOnPave
//purpose  : 
//=======================================================================
  Standard_Boolean BOPInt_Tools::IsOnPave(const Standard_Real aT1,
					  const IntTools_Range& aRange,
					  const Standard_Real aTolerance)
{
  Standard_Boolean firstisonpave1, firstisonpave2, bIsOnPave;
  //
  firstisonpave1  = (Abs(aRange.First() - aT1) < aTolerance);
  firstisonpave2  = (Abs(aRange.Last()  - aT1) < aTolerance);
  bIsOnPave=(firstisonpave1 || firstisonpave2);
  return bIsOnPave;
}
//=======================================================================
// function: VertexParameters
// purpose: 
//=======================================================================
  void BOPInt_Tools::VertexParameters(const IntTools_CommonPrt& aCPart,
				      Standard_Real& aT1, 
				      Standard_Real& aT2)
{
  const IntTools_Range& aR1=aCPart.Range1();
  aT1=0.5*(aR1.First()+aR1.Last());
  //
  if((aCPart.VertexParameter1() >= aR1.First()) &&
     (aCPart.VertexParameter1() <= aR1.Last())) {
    aT1 = aCPart.VertexParameter1();
  }
  //
  const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
  const IntTools_Range& aR2=aRanges2(1);
  aT2=0.5*(aR2.First()+aR2.Last());
  //
  if((aCPart.VertexParameter2() >= aR2.First()) &&
     (aCPart.VertexParameter2() <= aR2.Last())) {
    aT2 = aCPart.VertexParameter2();
  }
}
//=======================================================================
// function: VertexParameter
// purpose: 
//=======================================================================
  void BOPInt_Tools::VertexParameter(const IntTools_CommonPrt& aCPart,
				     Standard_Real& aT)
{
  const IntTools_Range& aR=aCPart.Range1();
  aT=0.5*(aR.First()+aR.Last());
  if((aCPart.VertexParameter1() >= aR.First()) &&
     (aCPart.VertexParameter1() <= aR.Last())) {
    aT = aCPart.VertexParameter1();
  }
}
//=======================================================================
// function: IsOnPave1
// purpose: 
//=======================================================================
  Standard_Boolean BOPInt_Tools::IsOnPave1(const Standard_Real aTR,
					   const IntTools_Range& aCPRange,
					   const Standard_Real aTolerance)
{
  Standard_Boolean bIsOnPave;
  Standard_Real aT1, aT2, dT1, dT2;
  //
  aT1=aCPRange.First();
  aT2=aCPRange.Last();
  bIsOnPave=(aTR>=aT1 && aTR<=aT1);
  if (bIsOnPave) {
    return bIsOnPave;
  }
  //
  dT1=Abs(aTR-aT1);  
  dT2=Abs(aTR-aT2);
  bIsOnPave=(dT1<=aTolerance || dT2<=aTolerance);
  return bIsOnPave;
}

//=======================================================================
// function: IsInRange
// purpose: 
//=======================================================================
  Standard_Boolean BOPInt_Tools::IsInRange(const IntTools_Range& aRRef,
                                           const IntTools_Range& aR,
                                           const Standard_Real aTolerance)
{
  Standard_Boolean bIsIn;
  Standard_Real aT1, aT2, aTRef1, aTRef2;
  //
  aR.Range(aT1, aT2);
  aRRef.Range(aTRef1, aTRef2);
  //
  aTRef1-=aTolerance;
  aTRef2+=aTolerance;
  //
  bIsIn = (aT1>=aTRef1 && aT1<=aTRef2) ||
          (aT2>=aTRef1 && aT2<=aTRef2);
  //
  return bIsIn;
}

//=======================================================================
//function : SegPln
//purpose  : 
//=======================================================================
  Standard_Integer BOPInt_Tools::SegPln(const gp_Lin& theLin,
					const Standard_Real theTLin1,
					const Standard_Real theTLin2,
					const Standard_Real theTolLin,
					const gp_Pln& thePln,
					const Standard_Real theTolPln,
					gp_Pnt& theP,
					Standard_Real& theTP,
					Standard_Real& theTolP,
					Standard_Real& theTPmin,
					Standard_Real& theTPmax)
{
  Standard_Integer iRet;
  Standard_Real aTol, aA, aB, aC, aD, aE, aH, aTP, aDist1, aDist2;
  gp_Pnt aP1, aP2;
  //
  iRet=0;
  aTol=theTolLin+theTolPln;
  //
  const gp_Ax3& aPosPln=thePln.Position();
  const gp_Dir& aDirPln=aPosPln.Direction();
  const gp_Pnt& aLocPln=aPosPln.Location();
  //
  const gp_Dir& aDirLin=theLin.Direction();
  const gp_Pnt& aLocLin=theLin.Location();
  //
  aP1.SetXYZ(aLocLin.XYZ()+theTLin1*aDirLin.XYZ());
  aDist1=aDirPln.X()*(aP1.X()-aLocPln.X())+
         aDirPln.Y()*(aP1.Y()-aLocPln.Y())+
         aDirPln.Z()*(aP1.Z()-aLocPln.Z());
  //
  aP2.SetXYZ(aLocLin.XYZ()+theTLin2*aDirLin.XYZ());
  aDist2=aDirPln.X()*(aP2.X()-aLocPln.X())+
         aDirPln.Y()*(aP2.Y()-aLocPln.Y())+
         aDirPln.Z()*(aP2.Z()-aLocPln.Z());
  //
  if (aDist1<aTol && aDist2<aTol){
    iRet=1; // common block
    return iRet;
  }
  //
  if (aDist1*aDist2 > 0.) {
    iRet=2; // segment lays on one side to the Plane
    return iRet;
  } 
  //
  thePln.Coefficients(aA, aB, aC, aD);
  aE=aA*aLocLin.X()+aB*aLocLin.Y()+aC*aLocLin.Z()+aD;
  aH=aA*aDirLin.X()+aB*aDirLin.Y()+aC*aDirLin.Z();
  aTP=-aE/aH;
  if (aTP < theTLin1-aTol || aTP > theTLin2+aTol) {
    iRet=3; // no intersections due to range of the Line
    return iRet;
  }
  //
  theTP=aTP;
  theP.SetXYZ(aLocLin.XYZ()+aTP*aDirLin.XYZ());
  theTolP=aTol;
  theTPmin=theTP-theTolPln;
  theTPmax=theTP+theTolPln;
  iRet=0; // intersection point
  return iRet;
}
