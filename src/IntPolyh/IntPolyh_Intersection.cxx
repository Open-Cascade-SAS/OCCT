// Created on: 1999-03-03
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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


#include <IntPolyh_Intersection.hxx>

#include <Adaptor3d_HSurface.hxx>

#include <IntPolyh_Couple.hxx>
#include <IntPolyh_CoupleMapHasher.hxx>
#include <IntPolyh_MaillageAffinage.hxx>
#include <IntPolyh_SectionLine.hxx>
#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Tools.hxx>
#include <IntPolyh_Triangle.hxx>

#include <NCollection_Map.hxx>

static Standard_Boolean IsAdvRequired(IntPolyh_PMaillageAffinage& theMaillage);

static Standard_Integer ComputeIntersection(IntPolyh_PMaillageAffinage& theMaillage);

static Standard_Boolean AnalyzeIntersection(IntPolyh_PMaillageAffinage& theMaillage);

//=======================================================================
//function : IntPolyh_Intersection
//purpose  : 
//=======================================================================
IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& theS1,
                                             const Handle(Adaptor3d_HSurface)& theS2)
{
  mySurf1 = theS1;
  mySurf2 = theS2;
  myNbSU1 = 10;
  myNbSV1 = 10;
  myNbSU2 = 10;
  myNbSV2 = 10;
  myIsDone = Standard_False;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform();
}

//=======================================================================
//function : IntPolyh_Intersection
//purpose  : 
//=======================================================================
IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& theS1,
                                             const Standard_Integer            theNbSU1,
                                             const Standard_Integer            theNbSV1,
                                             const Handle(Adaptor3d_HSurface)& theS2,
                                             const Standard_Integer            theNbSU2,
                                             const Standard_Integer            theNbSV2)
{
  mySurf1 = theS1;
  mySurf2 = theS2;
  myNbSU1 = theNbSU1;
  myNbSV1 = theNbSV1;
  myNbSU2 = theNbSU2;
  myNbSV2 = theNbSV2;
  myIsDone = Standard_False;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform();
}

//=======================================================================
//function : IntPolyh_Intersection
//purpose  : 
//=======================================================================
IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& theS1,
                                             const TColStd_Array1OfReal&       theUPars1,
                                             const TColStd_Array1OfReal&       theVPars1,
                                             const Handle(Adaptor3d_HSurface)& theS2,
                                             const TColStd_Array1OfReal&       theUPars2,
                                             const TColStd_Array1OfReal&       theVPars2)
{
  mySurf1 = theS1;
  mySurf2 = theS2;
  myNbSU1 = theUPars1.Length();
  myNbSV1 = theVPars1.Length();
  myNbSU2 = theUPars2.Length();
  myNbSV2 = theVPars2.Length();
  myIsDone = Standard_False;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform(theUPars1, theVPars1, theUPars2, theVPars2);
}

//=======================================================================
//function : GetLinePoint
//purpose  : 
//=======================================================================
void IntPolyh_Intersection::GetLinePoint(const Standard_Integer Indexl,
                                         const Standard_Integer Indexp,
                                         Standard_Real &x,
                                         Standard_Real &y,
                                         Standard_Real &z,
                                         Standard_Real &u1,
                                         Standard_Real &v1,
                                         Standard_Real &u2,
                                         Standard_Real &v2,
                                         Standard_Real &incidence) const
{
  const IntPolyh_SectionLine  &msl = mySectionLines[Indexl - 1];
  const IntPolyh_StartPoint   &sp = msl[Indexp - 1];
  x = sp.X();
  y = sp.Y();
  z = sp.Z();
  u1 = sp.U1();
  v1 = sp.V1();
  u2 = sp.U2();
  v2 = sp.V2();
  incidence = sp.GetAngle();
}

//=======================================================================
//function : GetTangentZonePoint
//purpose  : 
//=======================================================================
void IntPolyh_Intersection::GetTangentZonePoint(const Standard_Integer Indexz,
                                                const Standard_Integer /*Indexp*/,
                                                Standard_Real &x,
                                                Standard_Real &y,
                                                Standard_Real &z,
                                                Standard_Real &u1,
                                                Standard_Real &v1,
                                                Standard_Real &u2,
                                                Standard_Real &v2) const
{
  const IntPolyh_StartPoint   &sp = myTangentZones[Indexz - 1];
  x = sp.X();
  y = sp.Y();
  z = sp.Z();
  u1 = sp.U1();
  v1 = sp.V1();
  u2 = sp.U2();
  v2 = sp.V2();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntPolyh_Intersection::Perform()
{
  // Prepare the sampling of the surfaces - UV parameters of the triangulation nodes
  TColStd_Array1OfReal UPars1, VPars1, UPars2, VPars2;
  IntPolyh_Tools::MakeSampling(mySurf1, myNbSU1, myNbSV1, Standard_False, UPars1, VPars1);
  IntPolyh_Tools::MakeSampling(mySurf2, myNbSU2, myNbSV2, Standard_False, UPars2, VPars2);

  // Perform intersection
  Perform(UPars1, VPars1, UPars2, VPars2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntPolyh_Intersection::Perform(const TColStd_Array1OfReal& theUPars1,
                                    const TColStd_Array1OfReal& theVPars1,
                                    const TColStd_Array1OfReal& theUPars2,
                                    const TColStd_Array1OfReal& theVPars2)
{
  myIsDone = Standard_True;

  // Compute the deflection of the given sampling if it is not set
  Standard_Real aDeflTol1 = IntPolyh_Tools::ComputeDeflection(mySurf1, theUPars1, theVPars1);
  Standard_Real aDeflTol2 = IntPolyh_Tools::ComputeDeflection(mySurf2, theUPars2, theVPars2);

  // Perform standard intersection
  IntPolyh_PMaillageAffinage pMaillageStd = 0;
  Standard_Integer           nbCouplesStd = 0;
  Standard_Boolean isStdDone = PerformStd(theUPars1, theVPars1,
                                          theUPars2, theVPars2,
                                          aDeflTol1, aDeflTol2,
                                          pMaillageStd, nbCouplesStd);

  if (!isStdDone)
  {
    // Intersection not done
    myIsDone = Standard_False;
    if (pMaillageStd) delete pMaillageStd;
    return;
  }

  if (!IsAdvRequired(pMaillageStd))
  {
    // Default interference done well, use it
    pMaillageStd->StartPointsChain(mySectionLines, myTangentZones);
  }
  else
  {
    // Default intersection is done, but too few interferences found.
    // Perform advanced intersection - perform intersection four times with different shifts.
    IntPolyh_PMaillageAffinage pMaillageFF = 0;
    IntPolyh_PMaillageAffinage pMaillageFR = 0;
    IntPolyh_PMaillageAffinage pMaillageRF = 0;
    IntPolyh_PMaillageAffinage pMaillageRR = 0;
    Standard_Integer           nbCouplesAdv = 0;

    Standard_Boolean isAdvDone = PerformAdv(theUPars1, theVPars1,
                                              theUPars2, theVPars2,
                                              aDeflTol1, aDeflTol2,
                                              pMaillageFF,
                                              pMaillageFR,
                                              pMaillageRF,
                                              pMaillageRR,
                                              nbCouplesAdv);

    if (isAdvDone && nbCouplesAdv > 0)
    {
      // Advanced interference found
      pMaillageFF->StartPointsChain(mySectionLines, myTangentZones);
      pMaillageFR->StartPointsChain(mySectionLines, myTangentZones);
      pMaillageRF->StartPointsChain(mySectionLines, myTangentZones);
      pMaillageRR->StartPointsChain(mySectionLines, myTangentZones);
    }
    else
    {
      // Advanced intersection not done or no intersection is found -> use standard intersection
      if (nbCouplesStd > 0)
        pMaillageStd->StartPointsChain(mySectionLines, myTangentZones);
    }

    // Clean up
    if (pMaillageFF) delete pMaillageFF;
    if (pMaillageFR) delete pMaillageFR;
    if (pMaillageRF) delete pMaillageRF;
    if (pMaillageRR) delete pMaillageRR;
  }

  // clean up
  if (pMaillageStd) delete pMaillageStd;
}

//=======================================================================
//function : PerformStd
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformStd(const TColStd_Array1OfReal& theUPars1,
                                                   const TColStd_Array1OfReal& theVPars1,
                                                   const TColStd_Array1OfReal& theUPars2,
                                                   const TColStd_Array1OfReal& theVPars2,
                                                   const Standard_Real         theDeflTol1,
                                                   const Standard_Real         theDeflTol2,
                                                   IntPolyh_PMaillageAffinage& theMaillageS,
                                                   Standard_Integer&           theNbCouples)
{
  Standard_Boolean isDone = PerformMaillage(theUPars1, theVPars1,
                                            theUPars2, theVPars2,
                                            theDeflTol1, theDeflTol2,
                                            theMaillageS);
  theNbCouples = (isDone) ? (theMaillageS->GetCouples().Extent()) : 0;
  return isDone;
}

//=======================================================================
//function : PerformAdv
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformAdv(const TColStd_Array1OfReal& theUPars1,
                                                   const TColStd_Array1OfReal& theVPars1,
                                                   const TColStd_Array1OfReal& theUPars2,
                                                   const TColStd_Array1OfReal& theVPars2,
                                                   const Standard_Real         theDeflTol1,
                                                   const Standard_Real         theDeflTol2,
                                                   IntPolyh_PMaillageAffinage& theMaillageFF,
                                                   IntPolyh_PMaillageAffinage& theMaillageFR,
                                                   IntPolyh_PMaillageAffinage& theMaillageRF,
                                                   IntPolyh_PMaillageAffinage& theMaillageRR,
                                                   Standard_Integer&           theNbCouples)
{
  // Compute the points on the surface and normal directions in these points
  IntPolyh_ArrayOfPointNormal aPoints1, aPoints2;
  IntPolyh_Tools::FillArrayOfPointNormal(mySurf1, theUPars1, theVPars1, aPoints1);
  IntPolyh_Tools::FillArrayOfPointNormal(mySurf2, theUPars2, theVPars2, aPoints2);

  // Perform intersection with the different shifts of the triangles
  Standard_Boolean isDone =
    PerformMaillage(theUPars1, theVPars1, theUPars2, theVPars2, // sampling
                    theDeflTol1, theDeflTol2,                   // deflection tolerance
                    aPoints1, aPoints2,                         // points and normals
                    Standard_True , Standard_False,             // shift
                    theMaillageFR)
                    &&
    PerformMaillage(theUPars1, theVPars1, theUPars2, theVPars2, // sampling
                    theDeflTol1, theDeflTol2,                   // deflection tolerance
                    aPoints1, aPoints2,                         // points and normals
                    Standard_False, Standard_True,              // shift
                    theMaillageRF)
                    &&
    PerformMaillage(theUPars1, theVPars1, theUPars2, theVPars2, // sampling
                    theDeflTol1, theDeflTol2,                   // deflection tolerance
                    aPoints1, aPoints2,                         // points and normals
                    Standard_True, Standard_True,               // shift
                    theMaillageFF)
                    &&
    PerformMaillage(theUPars1, theVPars1, theUPars2, theVPars2, // sampling
                    theDeflTol1, theDeflTol2,                   // deflection tolerance
                    aPoints1, aPoints2,                         // points and normals
                    Standard_False, Standard_False,             // shift
                    theMaillageRR);

  if (isDone)
  {
    theNbCouples = theMaillageFF->GetCouples().Extent() +
                   theMaillageFR->GetCouples().Extent() +
                   theMaillageRF->GetCouples().Extent() +
                   theMaillageRR->GetCouples().Extent();

    // Merge couples
    if(theNbCouples > 0)
      MergeCouples(theMaillageFF->GetCouples(),
                   theMaillageFR->GetCouples(),
                   theMaillageRF->GetCouples(),
                   theMaillageRR->GetCouples());
  }

  return isDone;
}

//=======================================================================
//function : PerformMaillage
//purpose  : Computes standard MaillageAffinage (without shift)
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage(const TColStd_Array1OfReal& theUPars1,
                                                        const TColStd_Array1OfReal& theVPars1,
                                                        const TColStd_Array1OfReal& theUPars2,
                                                        const TColStd_Array1OfReal& theVPars2,
                                                        const Standard_Real         theDeflTol1,
                                                        const Standard_Real         theDeflTol2,
                                                        IntPolyh_PMaillageAffinage& theMaillage)
{
  theMaillage =
    new IntPolyh_MaillageAffinage(mySurf1, theUPars1.Length(), theVPars1.Length(),
                                  mySurf2, theUPars2.Length(), theVPars2.Length(),
                                  0);

  theMaillage->FillArrayOfPnt(1, theUPars1, theVPars1, &theDeflTol1);
  theMaillage->FillArrayOfPnt(2, theUPars2, theVPars2, &theDeflTol2);

  Standard_Integer FinTTC = ComputeIntersection(theMaillage);

  // If no intersecting triangles are found, try enlarged surfaces
  if (FinTTC == 0)
  {
    // Check if enlarge for the surfaces is possible
    Standard_Boolean isEnlargeU1, isEnlargeV1, isEnlargeU2, isEnlargeV2;
    IntPolyh_Tools::IsEnlargePossible(mySurf1, isEnlargeU1, isEnlargeV1);
    IntPolyh_Tools::IsEnlargePossible(mySurf2, isEnlargeU2, isEnlargeV2);

    if (isEnlargeU1 || isEnlargeV1 || isEnlargeU2 || isEnlargeV2)
    {
      theMaillage->SetEnlargeZone(Standard_True);
      // Make new points on the enlarged surface
      theMaillage->FillArrayOfPnt(1);
      theMaillage->FillArrayOfPnt(2);
      // Compute intersection
      ComputeIntersection(theMaillage);
      theMaillage->SetEnlargeZone(Standard_False);
    }
  }

  // if too many intersections, consider surfaces parallel
  return AnalyzeIntersection(theMaillage);
}

//=======================================================================
//function : PerformMaillage
//purpose  : Computes MaillageAffinage
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage(const TColStd_Array1OfReal& theUPars1,
                                                        const TColStd_Array1OfReal& theVPars1,
                                                        const TColStd_Array1OfReal& theUPars2,
                                                        const TColStd_Array1OfReal& theVPars2,
                                                        const Standard_Real         theDeflTol1,
                                                        const Standard_Real         theDeflTol2,
                                                        const IntPolyh_ArrayOfPointNormal& thePoints1,
                                                        const IntPolyh_ArrayOfPointNormal& thePoints2,
                                                        const Standard_Boolean      theIsFirstFwd,
                                                        const Standard_Boolean      theIsSecondFwd,
                                                        IntPolyh_PMaillageAffinage& theMaillage)
{
  theMaillage =
    new IntPolyh_MaillageAffinage(mySurf1, theUPars1.Length(), theVPars1.Length(),
                                  mySurf2, theUPars2.Length(), theVPars2.Length(),
                                  0);

  theMaillage->FillArrayOfPnt(1, theIsFirstFwd , thePoints1, theUPars1, theVPars1, theDeflTol1);
  theMaillage->FillArrayOfPnt(2, theIsSecondFwd, thePoints2, theUPars2, theVPars2, theDeflTol2);

  ComputeIntersection(theMaillage);

  return AnalyzeIntersection(theMaillage);
}

//=======================================================================
//function : MergeCouples
//purpose  : This method analyzes the lists to find same couples.
//           If some are detected it leaves the couple in only one list
//           deleting from others.
//=======================================================================
void IntPolyh_Intersection::MergeCouples(IntPolyh_ListOfCouples &anArrayFF,
                                         IntPolyh_ListOfCouples &anArrayFR,
                                         IntPolyh_ListOfCouples &anArrayRF,
                                         IntPolyh_ListOfCouples &anArrayRR) const
{
  // Fence map to remove from the lists the duplicating elements.
  NCollection_Map<IntPolyh_Couple, IntPolyh_CoupleMapHasher> aFenceMap;
  //
  IntPolyh_ListOfCouples* pLists[4] = {&anArrayFF, &anArrayFR, &anArrayRF, &anArrayRR};
  for (Standard_Integer i = 0; i < 4; ++i) {
    IntPolyh_ListIteratorOfListOfCouples aIt(*pLists[i]);
    for (; aIt.More();) {
      if (!aFenceMap.Add(aIt.Value())) {
        pLists[i]->Remove(aIt);
        continue;
      }
      aIt.Next();
    }
  }
}

//=======================================================================
//function : IsAdvRequired
//purpose  : Analyzes the standard intersection on the angles between triangles.
//           If the angle between some of the interfering triangles is
//           too small (less than 5 deg), the advanced intersection is required.
//           Otherwise, the standard intersection is considered satisfactory.
//=======================================================================
Standard_Boolean IsAdvRequired(IntPolyh_PMaillageAffinage& theMaillage)
{
  if (!theMaillage)
    return Standard_True;

  // Interfering triangles
  IntPolyh_ListOfCouples& Couples = theMaillage->GetCouples();
  // Number of interfering pairs
  Standard_Integer aNbCouples = Couples.Extent();
  // Flag to define whether advanced intersection is required or not
  Standard_Boolean isAdvReq = (aNbCouples == 0);
  if (isAdvReq)
    // No interfering triangles are found -> perform advanced intersection
    return isAdvReq;

  if (aNbCouples > 10)
    // Enough interfering triangles are found -> no need to perform advanced intersection
    return isAdvReq;

  const Standard_Real anEps = .996; //~ cos of 5 deg
  IntPolyh_ListIteratorOfListOfCouples aIt(Couples);
  for(; aIt.More(); aIt.Next())
  {
    if (Abs(aIt.Value().Angle()) > anEps)
    {
      // The angle between interfering triangles is small -> perform advanced
      // intersection to make intersection more precise
      isAdvReq = Standard_True;
      break;
    }
  }

  return isAdvReq;
}

//=======================================================================
//function : ComputeIntersection
//purpose  : Computes the intersection of the triangles
//=======================================================================
Standard_Integer ComputeIntersection(IntPolyh_PMaillageAffinage& theMaillage)
{
  if (!theMaillage)
    return 0;

  // Compute common box and mark the points inside that box
  theMaillage->CommonBox();

  // Make triangles
  theMaillage->FillArrayOfTriangles(1);
  theMaillage->FillArrayOfTriangles(2);

  // Make edges
  theMaillage->FillArrayOfEdges(1);
  theMaillage->FillArrayOfEdges(2);

  // Deflection refinement
  theMaillage->TrianglesDeflectionsRefinementBSB();

  return theMaillage->TriangleCompare();
}

//=======================================================================
//function : AnalyzeIntersection
//purpose  : Analyzes the intersection on the number of interfering triangles
//=======================================================================
Standard_Boolean AnalyzeIntersection(IntPolyh_PMaillageAffinage& theMaillage)
{
  if (!theMaillage)
    return Standard_False;

  IntPolyh_ListOfCouples& Couples = theMaillage->GetCouples();
  Standard_Integer FinTTC = Couples.Extent();
  if(FinTTC > 200)
  {
    const Standard_Real eps = .996; //~ cos of 5deg.
    Standard_Integer npara = 0;
    IntPolyh_ListIteratorOfListOfCouples aIt(Couples);
    for(; aIt.More(); aIt.Next())
    {
      Standard_Real cosa = Abs(aIt.Value().Angle());
      if(cosa > eps) ++npara;
    }

    if (npara >= theMaillage->GetArrayOfTriangles(1).NbItems() ||
        npara >= theMaillage->GetArrayOfTriangles(2).NbItems())
    {
      return Standard_False;
    }
  }
  return Standard_True;
}
