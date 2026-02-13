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

#include <Adaptor3d_Surface.hxx>

#include <IntPolyh_Couple.hxx>
#include <IntPolyh_MaillageAffinage.hxx>
#include <IntPolyh_SectionLine.hxx>
#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Tools.hxx>
#include <IntPolyh_Triangle.hxx>

#include <NCollection_Map.hxx>

static int ComputeIntersection(IntPolyh_PMaillageAffinage& theMaillage);

//=================================================================================================

IntPolyh_Intersection::IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                             const occ::handle<Adaptor3d_Surface>& theS2)
{
  mySurf1      = theS1;
  mySurf2      = theS2;
  myNbSU1      = 10;
  myNbSV1      = 10;
  myNbSU2      = 10;
  myNbSV2      = 10;
  myIsDone     = false;
  myIsParallel = false;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform();
}

//=================================================================================================

IntPolyh_Intersection::IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                             const int                             theNbSU1,
                                             const int                             theNbSV1,
                                             const occ::handle<Adaptor3d_Surface>& theS2,
                                             const int                             theNbSU2,
                                             const int                             theNbSV2)
{
  mySurf1      = theS1;
  mySurf2      = theS2;
  myNbSU1      = theNbSU1;
  myNbSV1      = theNbSV1;
  myNbSU2      = theNbSU2;
  myNbSV2      = theNbSV2;
  myIsDone     = false;
  myIsParallel = false;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform();
}

//=================================================================================================

IntPolyh_Intersection::IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                             const NCollection_Array1<double>&     theUPars1,
                                             const NCollection_Array1<double>&     theVPars1,
                                             const occ::handle<Adaptor3d_Surface>& theS2,
                                             const NCollection_Array1<double>&     theUPars2,
                                             const NCollection_Array1<double>&     theVPars2)
{
  mySurf1      = theS1;
  mySurf2      = theS2;
  myNbSU1      = theUPars1.Length();
  myNbSV1      = theVPars1.Length();
  myNbSU2      = theUPars2.Length();
  myNbSV2      = theVPars2.Length();
  myIsDone     = false;
  myIsParallel = false;
  mySectionLines.Init(1000);
  myTangentZones.Init(10000);
  Perform(theUPars1, theVPars1, theUPars2, theVPars2);
}

//=================================================================================================

void IntPolyh_Intersection::GetLinePoint(const int Indexl,
                                         const int Indexp,
                                         double&   x,
                                         double&   y,
                                         double&   z,
                                         double&   u1,
                                         double&   v1,
                                         double&   u2,
                                         double&   v2,
                                         double&   incidence) const
{
  const IntPolyh_SectionLine& msl = mySectionLines[Indexl - 1];
  const IntPolyh_StartPoint&  sp  = msl[Indexp - 1];
  x                               = sp.X();
  y                               = sp.Y();
  z                               = sp.Z();
  u1                              = sp.U1();
  v1                              = sp.V1();
  u2                              = sp.U2();
  v2                              = sp.V2();
  incidence                       = sp.GetAngle();
}

//=================================================================================================

void IntPolyh_Intersection::GetTangentZonePoint(const int Indexz,
                                                const int /*Indexp*/,
                                                double& x,
                                                double& y,
                                                double& z,
                                                double& u1,
                                                double& v1,
                                                double& u2,
                                                double& v2) const
{
  const IntPolyh_StartPoint& sp = myTangentZones[Indexz - 1];
  x                             = sp.X();
  y                             = sp.Y();
  z                             = sp.Z();
  u1                            = sp.U1();
  v1                            = sp.V1();
  u2                            = sp.U2();
  v2                            = sp.V2();
}

//=================================================================================================

void IntPolyh_Intersection::Perform()
{
  // Prepare the sampling of the surfaces - UV parameters of the triangulation nodes
  NCollection_Array1<double> UPars1, VPars1, UPars2, VPars2;
  IntPolyh_Tools::MakeSampling(mySurf1, myNbSU1, myNbSV1, false, UPars1, VPars1);
  IntPolyh_Tools::MakeSampling(mySurf2, myNbSU2, myNbSV2, false, UPars2, VPars2);

  // Perform intersection
  Perform(UPars1, VPars1, UPars2, VPars2);
}

//=================================================================================================

void IntPolyh_Intersection::Perform(const NCollection_Array1<double>& theUPars1,
                                    const NCollection_Array1<double>& theVPars1,
                                    const NCollection_Array1<double>& theUPars2,
                                    const NCollection_Array1<double>& theVPars2)
{
  myIsDone = true;

  // Compute the deflection of the given sampling if it is not set
  double aDeflTol1 = IntPolyh_Tools::ComputeDeflection(mySurf1, theUPars1, theVPars1);
  double aDeflTol2 = IntPolyh_Tools::ComputeDeflection(mySurf2, theUPars2, theVPars2);

  // Perform standard intersection
  IntPolyh_PMaillageAffinage pMaillageStd = nullptr;
  int                        nbCouplesStd = 0;
  bool                       isStdDone    = PerformStd(theUPars1,
                              theVPars1,
                              theUPars2,
                              theVPars2,
                              aDeflTol1,
                              aDeflTol2,
                              pMaillageStd,
                              nbCouplesStd);

  if (!isStdDone)
  {
    // Intersection not done
    myIsDone = false;
    delete pMaillageStd;
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
    IntPolyh_PMaillageAffinage pMaillageFF  = nullptr;
    IntPolyh_PMaillageAffinage pMaillageFR  = nullptr;
    IntPolyh_PMaillageAffinage pMaillageRF  = nullptr;
    IntPolyh_PMaillageAffinage pMaillageRR  = nullptr;
    int                        nbCouplesAdv = 0;

    bool isAdvDone = PerformAdv(theUPars1,
                                theVPars1,
                                theUPars2,
                                theVPars2,
                                aDeflTol1,
                                aDeflTol2,
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
    delete pMaillageFF;
    delete pMaillageFR;
    delete pMaillageRF;
    delete pMaillageRR;
  }

  // clean up
  delete pMaillageStd;
}

//=================================================================================================

bool IntPolyh_Intersection::PerformStd(const NCollection_Array1<double>& theUPars1,
                                       const NCollection_Array1<double>& theVPars1,
                                       const NCollection_Array1<double>& theUPars2,
                                       const NCollection_Array1<double>& theVPars2,
                                       const double                      theDeflTol1,
                                       const double                      theDeflTol2,
                                       IntPolyh_PMaillageAffinage&       theMaillageS,
                                       int&                              theNbCouples)
{
  bool isDone  = PerformMaillage(theUPars1,
                                theVPars1,
                                theUPars2,
                                theVPars2,
                                theDeflTol1,
                                theDeflTol2,
                                theMaillageS);
  theNbCouples = (isDone) ? (theMaillageS->GetCouples().Extent()) : 0;
  return isDone;
}

//=================================================================================================

bool IntPolyh_Intersection::PerformAdv(const NCollection_Array1<double>& theUPars1,
                                       const NCollection_Array1<double>& theVPars1,
                                       const NCollection_Array1<double>& theUPars2,
                                       const NCollection_Array1<double>& theVPars2,
                                       const double                      theDeflTol1,
                                       const double                      theDeflTol2,
                                       IntPolyh_PMaillageAffinage&       theMaillageFF,
                                       IntPolyh_PMaillageAffinage&       theMaillageFR,
                                       IntPolyh_PMaillageAffinage&       theMaillageRF,
                                       IntPolyh_PMaillageAffinage&       theMaillageRR,
                                       int&                              theNbCouples)
{
  // Compute the points on the surface and normal directions in these points
  IntPolyh_ArrayOfPointNormal aPoints1, aPoints2;
  IntPolyh_Tools::FillArrayOfPointNormal(mySurf1, theUPars1, theVPars1, aPoints1);
  IntPolyh_Tools::FillArrayOfPointNormal(mySurf2, theUPars2, theVPars2, aPoints2);

  // Perform intersection with the different shifts of the triangles
  bool isDone = PerformMaillage(theUPars1,
                                theVPars1,
                                theUPars2,
                                theVPars2, // sampling
                                theDeflTol1,
                                theDeflTol2, // deflection tolerance
                                aPoints1,
                                aPoints2, // points and normals
                                true,
                                false, // shift
                                theMaillageFR)
                && PerformMaillage(theUPars1,
                                   theVPars1,
                                   theUPars2,
                                   theVPars2, // sampling
                                   theDeflTol1,
                                   theDeflTol2, // deflection tolerance
                                   aPoints1,
                                   aPoints2, // points and normals
                                   false,
                                   true, // shift
                                   theMaillageRF)
                && PerformMaillage(theUPars1,
                                   theVPars1,
                                   theUPars2,
                                   theVPars2, // sampling
                                   theDeflTol1,
                                   theDeflTol2, // deflection tolerance
                                   aPoints1,
                                   aPoints2, // points and normals
                                   true,
                                   true, // shift
                                   theMaillageFF)
                && PerformMaillage(theUPars1,
                                   theVPars1,
                                   theUPars2,
                                   theVPars2, // sampling
                                   theDeflTol1,
                                   theDeflTol2, // deflection tolerance
                                   aPoints1,
                                   aPoints2, // points and normals
                                   false,
                                   false, // shift
                                   theMaillageRR);

  if (isDone)
  {
    theNbCouples = theMaillageFF->GetCouples().Extent() + theMaillageFR->GetCouples().Extent()
                   + theMaillageRF->GetCouples().Extent() + theMaillageRR->GetCouples().Extent();

    // Merge couples
    if (theNbCouples > 0)
      MergeCouples(theMaillageFF->GetCouples(),
                   theMaillageFR->GetCouples(),
                   theMaillageRF->GetCouples(),
                   theMaillageRR->GetCouples());
  }

  return isDone;
}

//=======================================================================
// function : PerformMaillage
// purpose  : Computes standard MaillageAffinage (without shift)
//=======================================================================
bool IntPolyh_Intersection::PerformMaillage(const NCollection_Array1<double>& theUPars1,
                                            const NCollection_Array1<double>& theVPars1,
                                            const NCollection_Array1<double>& theUPars2,
                                            const NCollection_Array1<double>& theVPars2,
                                            const double                      theDeflTol1,
                                            const double                      theDeflTol2,
                                            IntPolyh_PMaillageAffinage&       theMaillage)
{
  theMaillage = new IntPolyh_MaillageAffinage(mySurf1,
                                              theUPars1.Length(),
                                              theVPars1.Length(),
                                              mySurf2,
                                              theUPars2.Length(),
                                              theVPars2.Length(),
                                              0);

  theMaillage->FillArrayOfPnt(1, theUPars1, theVPars1, &theDeflTol1);
  theMaillage->FillArrayOfPnt(2, theUPars2, theVPars2, &theDeflTol2);

  int FinTTC = ComputeIntersection(theMaillage);

  // If no intersecting triangles are found, try enlarged surfaces
  if (FinTTC == 0)
  {
    // Check if enlarge for the surfaces is possible
    bool isEnlargeU1, isEnlargeV1, isEnlargeU2, isEnlargeV2;
    IntPolyh_Tools::IsEnlargePossible(mySurf1, isEnlargeU1, isEnlargeV1);
    IntPolyh_Tools::IsEnlargePossible(mySurf2, isEnlargeU2, isEnlargeV2);

    if (isEnlargeU1 || isEnlargeV1 || isEnlargeU2 || isEnlargeV2)
    {
      theMaillage->SetEnlargeZone(true);
      // Make new points on the enlarged surface
      theMaillage->FillArrayOfPnt(1);
      theMaillage->FillArrayOfPnt(2);
      // Compute intersection
      ComputeIntersection(theMaillage);
      theMaillage->SetEnlargeZone(false);
    }
  }

  // if too many intersections, consider surfaces parallel
  return AnalyzeIntersection(theMaillage);
}

//=======================================================================
// function : PerformMaillage
// purpose  : Computes MaillageAffinage
//=======================================================================
bool IntPolyh_Intersection::PerformMaillage(const NCollection_Array1<double>&  theUPars1,
                                            const NCollection_Array1<double>&  theVPars1,
                                            const NCollection_Array1<double>&  theUPars2,
                                            const NCollection_Array1<double>&  theVPars2,
                                            const double                       theDeflTol1,
                                            const double                       theDeflTol2,
                                            const IntPolyh_ArrayOfPointNormal& thePoints1,
                                            const IntPolyh_ArrayOfPointNormal& thePoints2,
                                            const bool                         theIsFirstFwd,
                                            const bool                         theIsSecondFwd,
                                            IntPolyh_PMaillageAffinage&        theMaillage)
{
  theMaillage = new IntPolyh_MaillageAffinage(mySurf1,
                                              theUPars1.Length(),
                                              theVPars1.Length(),
                                              mySurf2,
                                              theUPars2.Length(),
                                              theVPars2.Length(),
                                              0);

  theMaillage->FillArrayOfPnt(1, theIsFirstFwd, thePoints1, theUPars1, theVPars1, theDeflTol1);
  theMaillage->FillArrayOfPnt(2, theIsSecondFwd, thePoints2, theUPars2, theVPars2, theDeflTol2);

  ComputeIntersection(theMaillage);

  return AnalyzeIntersection(theMaillage);
}

//=======================================================================
// function : MergeCouples
// purpose  : This method analyzes the lists to find same couples.
//           If some are detected it leaves the couple in only one list
//           deleting from others.
//=======================================================================
void IntPolyh_Intersection::MergeCouples(NCollection_List<IntPolyh_Couple>& anArrayFF,
                                         NCollection_List<IntPolyh_Couple>& anArrayFR,
                                         NCollection_List<IntPolyh_Couple>& anArrayRF,
                                         NCollection_List<IntPolyh_Couple>& anArrayRR) const
{
  // Fence map to remove from the lists the duplicating elements.
  NCollection_Map<IntPolyh_Couple> aFenceMap;
  //
  NCollection_List<IntPolyh_Couple>* pLists[4] = {&anArrayFF, &anArrayFR, &anArrayRF, &anArrayRR};
  for (int i = 0; i < 4; ++i)
  {
    NCollection_List<IntPolyh_Couple>::Iterator aIt(*pLists[i]);
    for (; aIt.More();)
    {
      if (!aFenceMap.Add(aIt.Value()))
      {
        pLists[i]->Remove(aIt);
        continue;
      }
      aIt.Next();
    }
  }
}

//=======================================================================
// function : IsAdvRequired
// purpose  : Analyzes the standard intersection on the angles between triangles.
//           If the angle between some of the interfering triangles is
//           too small (less than 5 deg), the advanced intersection is required.
//           Otherwise, the standard intersection is considered satisfactory.
//=======================================================================
bool IntPolyh_Intersection::IsAdvRequired(IntPolyh_PMaillageAffinage& theMaillage)
{
  if (!theMaillage)
    return true;

  // Interfering triangles
  NCollection_List<IntPolyh_Couple>& Couples = theMaillage->GetCouples();
  // Number of interfering pairs
  int aNbCouples = Couples.Extent();
  // Flag to define whether advanced intersection is required or not
  bool isAdvReq = (aNbCouples == 0) && !IsParallel();
  if (isAdvReq)
    // No interfering triangles are found -> perform advanced intersection
    return isAdvReq;

  if (aNbCouples > 10)
    // Enough interfering triangles are found -> no need to perform advanced intersection
    return isAdvReq;

  const double                                anEps = .996; //~ cos of 5 deg
  NCollection_List<IntPolyh_Couple>::Iterator aIt(Couples);
  for (; aIt.More(); aIt.Next())
  {
    if (std::abs(aIt.Value().Angle()) > anEps)
    {
      // The angle between interfering triangles is small -> perform advanced
      // intersection to make intersection more precise
      isAdvReq = true;
      break;
    }
  }

  return isAdvReq;
}

//=======================================================================
// function : ComputeIntersection
// purpose  : Computes the intersection of the triangles
//=======================================================================
int ComputeIntersection(IntPolyh_PMaillageAffinage& theMaillage)
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
// function : AnalyzeIntersection
// purpose  : Analyzes the intersection on the number of interfering triangles
//=======================================================================
bool IntPolyh_Intersection::AnalyzeIntersection(IntPolyh_PMaillageAffinage& theMaillage)
{
  if (!theMaillage)
    return false;

  NCollection_List<IntPolyh_Couple>& Couples = theMaillage->GetCouples();
  int                                FinTTC  = Couples.Extent();
  if (FinTTC > 200)
  {
    const double                                eps   = .996; //~ cos of 5deg.
    int                                         npara = 0;
    NCollection_List<IntPolyh_Couple>::Iterator aIt(Couples);
    for (; aIt.More(); aIt.Next())
    {
      double cosa = std::abs(aIt.Value().Angle());
      if (cosa > eps)
        ++npara;
    }

    if (npara >= theMaillage->GetArrayOfTriangles(1).NbItems()
        || npara >= theMaillage->GetArrayOfTriangles(2).NbItems())
    {
      Couples.Clear();
      myIsParallel = true;
      return true;
    }
  }
  return true;
}
