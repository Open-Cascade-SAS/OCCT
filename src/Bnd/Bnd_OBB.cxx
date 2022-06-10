// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Bnd_OBB.hxx>

#include <Bnd_Tools.hxx>
#include <Bnd_Range.hxx>

#include <BVH_BoxSet.hxx>
#include <BVH_LinearBuilder.hxx>

#include <BVH_Traverse.hxx>

#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_Dump.hxx>
#include <TColStd_Array1OfReal.hxx>

//! Auxiliary class to select from the points stored in
//! BVH tree the two points giving the extreme projection
//! parameters on the axis
class OBB_ExtremePointsSelector : 
  public BVH_Traverse <Standard_Real, 3, BVH_BoxSet <Standard_Real, 3, gp_XYZ>, Bnd_Range>
{
public:

  //! Constructor
  OBB_ExtremePointsSelector() :
    BVH_Traverse <Standard_Real, 3, BVH_BoxSet <Standard_Real, 3, gp_XYZ>, Bnd_Range>(),
    myPrmMin (RealLast()),
    myPrmMax (RealFirst())
  {}

public: //! @name Set axis for projection

  //! Sets the axis
  void SetAxis (const gp_XYZ& theAxis) { myAxis = theAxis; }

public: //! @name Clears the points from previous runs

  //! Clear
  void Clear()
  {
    myPrmMin = RealLast();
    myPrmMax = RealFirst();
  }

public: //! @name Getting the results

  //! Returns the minimal projection parameter
  Standard_Real MinPrm() const { return myPrmMin; }

  //! Returns the maximal projection parameter
  Standard_Real MaxPrm() const { return myPrmMax; }

  //! Returns the minimal projection point
  const gp_XYZ& MinPnt() const { return myPntMin; }

  //! Returns the maximal projection point
  const gp_XYZ& MaxPnt() const { return myPntMax; }

public: //! @name Definition of rejection/acceptance rules

  //! Defines the rules for node rejection
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCMin,
                                       const BVH_Vec3d& theCMax,
                                       Bnd_Range& theMetric) const Standard_OVERRIDE
  {
    if (myPrmMin > myPrmMax)
      // No parameters computed yet
      return Standard_False;

    Standard_Real aPrmMin = myPrmMin, aPrmMax = myPrmMax;
    Standard_Boolean isToReject = Standard_True;

    // Check if the current node is between already found parameters
    for (Standard_Integer i = 0; i < 2; ++i)
    {
      Standard_Real x = !i ? theCMin.x() : theCMax.x();
      for (Standard_Integer j = 0; j < 2; ++j)
      {
        Standard_Real y = !j ? theCMin.y() : theCMax.y();
        for (Standard_Integer k = 0; k < 2; ++k)
        {
          Standard_Real z = !k ? theCMin.z() : theCMax.z();

          Standard_Real aPrm = myAxis.Dot (gp_XYZ (x, y, z));
          if (aPrm < aPrmMin)
          {
            aPrmMin = aPrm;
            isToReject = Standard_False;
          }
          else if (aPrm > aPrmMax)
          {
            aPrmMax = aPrm;
            isToReject = Standard_False;
          }
        }
      }
    }

    theMetric = Bnd_Range (aPrmMin, aPrmMax);

    return isToReject;
  }

  //! Rules for node rejection by the metric
  virtual Standard_Boolean RejectMetric (const Bnd_Range& theMetric) const Standard_OVERRIDE
  {
    if (myPrmMin > myPrmMax)
      // no parameters computed
      return Standard_False;

    Standard_Real aMin, aMax;
    if (!theMetric.GetBounds (aMin, aMax))
      // void metric
      return Standard_False;

    // Check if the box of the branch is inside of the already computed parameters
    return aMin > myPrmMin && aMax < myPrmMax;
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex,
                                   const Bnd_Range&) Standard_OVERRIDE
  {
    const gp_XYZ& theLeaf = myBVHSet->Element (theIndex);
    Standard_Real aPrm = myAxis.Dot (theLeaf);
    if (aPrm < myPrmMin)
    {
      myPrmMin = aPrm;
      myPntMin = theLeaf;
    }
    if (aPrm > myPrmMax)
    {
      myPrmMax = aPrm;
      myPntMax = theLeaf;
    }
    return Standard_True;
  }

public: //! @name Choosing the best branch

  //! Returns true if the metric of the left branch is better than the metric of the right
  virtual Standard_Boolean IsMetricBetter (const Bnd_Range& theLeft,
                                           const Bnd_Range& theRight) const Standard_OVERRIDE
  {
    if (myPrmMin > myPrmMax)
      // no parameters computed
      return Standard_True;

    Standard_Real aMin[2], aMax[2];
    if (!theLeft.GetBounds  (aMin[0], aMax[0]) ||
        !theRight.GetBounds (aMin[1], aMax[1]))
      // void metrics
      return Standard_True;

    // Choose branch with larger extension over computed parameters
    Standard_Real anExt[2] = {0.0, 0.0};
    for (int i = 0; i < 2; ++i)
    {
      if (aMin[i] < myPrmMin) anExt[i] += myPrmMin - aMin[i];
      if (aMax[i] > myPrmMax) anExt[i] += aMax[i] - myPrmMax;
    }
    return anExt[0] > anExt[1];
  }

protected: //! @name Fields

  gp_XYZ myAxis;          //!< Axis to project the points to
  Standard_Real myPrmMin; //!< Minimal projection parameter
  Standard_Real myPrmMax; //!< Maximal projection parameter
  gp_XYZ myPntMin;        //!< Minimal projection point
  gp_XYZ myPntMax;        //!< Maximal projection point
};

//! Tool for OBB construction
class OBBTool
{
public:
  //! Constructor. theL - list of points.
  //! theLT is a pointer to the list of tolerances
  //! (i-th element of this array is a tolerance
  //! of i-th point in theL). If theLT is empty
  //! then the tolerance of every point is equal to 0.
  //! Attention! The objects, which theL and theLT links on,
  //! must be available during all time of OBB creation
  //! (i.e. while the object of OBBTool exists).
  OBBTool(const TColgp_Array1OfPnt& theL,
          const TColStd_Array1OfReal *theLT = 0,
          Standard_Boolean theIsOptimal = Standard_False);

  //! DiTO algorithm for OBB construction
  //! (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf)
  void ProcessDiTetrahedron();

  //! Creates OBB with already computed parameters
  void BuildBox(Bnd_OBB& theBox);

protected:

  // Computes the extreme points on the set of Initial axes
  void ComputeExtremePoints ();

  //! Works with the triangle set by the points in myTriIdx.
  //! If theIsBuiltTrg == TRUE, new set of triangles will be
  //! recomputed.
  void ProcessTriangle(const Standard_Integer theIdx1,
                       const Standard_Integer theIdx2,
                       const Standard_Integer theIdx3,
                       const Standard_Boolean theIsBuiltTrg);

  //! Computes myTriIdx[2]
  void FillToTriangle3();

  //! Computes myTriIdx[3] and myTriIdx[4]
  void FillToTriangle5(const gp_XYZ& theNormal,
                       const gp_XYZ& theBarryCenter);

  //! Returns half of the Surface area of the box
  static Standard_Real ComputeQuality(const Standard_Real* const thePrmArr)
  {
    const Standard_Real aDX = thePrmArr[1] - thePrmArr[0],
                        aDY = thePrmArr[3] - thePrmArr[2],
                        aDZ = thePrmArr[5] - thePrmArr[4];

    return (aDX*aDY + aDY*aDZ + aDX*aDZ);
  }

protected:
  //! Assignment operator is forbidden
  OBBTool& operator=(const OBBTool&);

private:
  //! Params structure stores the two values meaning
  //! min and max parameters on the axis
  struct Params
  {
    Params() :
      _ParamMin(RealLast()), _ParamMax(RealFirst())
    {}

    Params(Standard_Real theMin, Standard_Real theMax)
      : _ParamMin(theMin), _ParamMax(theMax)
    {}

    Standard_Real _ParamMin;
    Standard_Real _ParamMax;
  };

  //! Computes the Minimal and maximal parameters on the vector
  //! connecting the points myLExtremalPoints[theId1] and myLExtremalPoints[theId2]
  void ComputeParams (const Standard_Integer theId1,
                      const Standard_Integer theId2,
                      Standard_Real &theMin,
                      Standard_Real &theMax)
  {
    theMin = myParams[theId1][theId2]._ParamMin;
    theMax = myParams[theId1][theId2]._ParamMax;

    if (theMin > theMax)
    {
      FindMinMax ((myLExtremalPoints[theId1] - myLExtremalPoints[theId2]).Normalized(), theMin, theMax);
      myParams[theId1][theId2]._ParamMin = myParams[theId2][theId1]._ParamMin = theMin;
      myParams[theId1][theId2]._ParamMax = myParams[theId2][theId1]._ParamMax = theMax;
    }
  }

  //! Looks for the min-max parameters on the axis.
  //! For optimal case projects all the points on the axis,
  //! for not optimal - only the set of extreme points.
  void FindMinMax (const gp_XYZ& theAxis,
                   Standard_Real &theMin,
                   Standard_Real &theMax)
  {
    theMin = RealLast(), theMax = RealFirst();

    if (myOptimal)
      Project (theAxis, theMin, theMax);
    else
    {
      for (Standard_Integer i = 0; i < myNbExtremalPoints; ++i)
      {
        Standard_Real aPrm = theAxis.Dot (myLExtremalPoints[i]);
        if (aPrm < theMin) theMin = aPrm;
        if (aPrm > theMax) theMax = aPrm;
      }
    }
  }

  //! Projects the set of points on the axis
  void Project (const gp_XYZ& theAxis,
                Standard_Real& theMin, Standard_Real& theMax,
                gp_XYZ* thePntMin = 0, gp_XYZ* thePntMax = 0)
  {
    theMin = RealLast(), theMax = RealFirst();

    if (myOptimal)
    {
      // Project BVH
      OBB_ExtremePointsSelector anExtremePointsSelector;
      anExtremePointsSelector.SetBVHSet (myPointBoxSet.get());
      anExtremePointsSelector.SetAxis (theAxis);
      anExtremePointsSelector.Select();
      theMin = anExtremePointsSelector.MinPrm();
      theMax = anExtremePointsSelector.MaxPrm();
      if (thePntMin) *thePntMin = anExtremePointsSelector.MinPnt();
      if (thePntMax) *thePntMax = anExtremePointsSelector.MaxPnt();
    }
    else
    {
      // Project all points
      for (Standard_Integer iP = myPntsList.Lower(); iP <= myPntsList.Upper(); ++iP)
      {
        const gp_XYZ& aPoint = myPntsList(iP).XYZ();
        const Standard_Real aPrm = theAxis.Dot (aPoint);
        if (aPrm < theMin)
        {
          theMin = aPrm;
          if (thePntMin)
            *thePntMin = aPoint;
        }
        if (aPrm > theMax)
        {
          theMax = aPrm;
          if (thePntMax)
            *thePntMax = aPoint;
        }
      }
    }
  }

private:

  //! Number of the initial axes.
  static const Standard_Integer myNbInitAxes = 7;

  //! Number of extremal points
  static const Standard_Integer myNbExtremalPoints = 2 * myNbInitAxes;

  //! The source list of points
  const TColgp_Array1OfPnt& myPntsList;

  //! Pointer to the array of tolerances
  const TColStd_Array1OfReal *myListOfTolers;

  //! Points of ditetrahedron
  //! given by their indices in myLExtremalPoints.
  Standard_Integer myTriIdx[5];

  //! List of extremal points
  gp_XYZ myLExtremalPoints[myNbExtremalPoints];

  //! The axes of the box (always normalized or
  //! can be null-vector)
  gp_XYZ myAxes[3];

  //! The surface area of the OBB
  Standard_Real myQualityCriterion;

  //! Defines if the OBB should be computed more tight.
  //! Takes more time, but the volume is less.
  Standard_Boolean myOptimal;

  //! Point box set organized with BVH
  opencascade::handle<BVH_BoxSet <Standard_Real, 3, gp_XYZ>> myPointBoxSet;

  //! Stored min/max parameters for the axes between extremal points
  Params myParams[myNbExtremalPoints][myNbExtremalPoints];
};

//=======================================================================
// Function : SetMinMax
// purpose : 
//    ATTENTION!!! thePrmArr must be initialized before this method calling.
//=======================================================================
static inline void SetMinMax(Standard_Real* const thePrmArr,
                             const Standard_Real theNewParam)
{
  if(theNewParam < thePrmArr[0])
  {
    thePrmArr[0] = theNewParam;
  }
  if(theNewParam > thePrmArr[1])
  {
    thePrmArr[1] = theNewParam;
  }
}

//=======================================================================
// Function : Constructor
// purpose : 
//=======================================================================
OBBTool::
    OBBTool(const TColgp_Array1OfPnt& theL,
            const TColStd_Array1OfReal *theLT,
            const Standard_Boolean theIsOptimal) : myPntsList(theL),
                                                   myListOfTolers(theLT),
                                                   myQualityCriterion(RealLast()),
                                                   myOptimal (theIsOptimal)
{
  if (myOptimal)
  {
    // Use linear builder for BVH construction with 30 elements in the leaf
    opencascade::handle<BVH_LinearBuilder<Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder<Standard_Real, 3> (30);
    myPointBoxSet = new BVH_BoxSet <Standard_Real, 3, gp_XYZ> (aLBuilder);
    myPointBoxSet->SetSize(myPntsList.Length());

    // Add the points into Set
    for (Standard_Integer iP = theL.Lower(); iP <= theL.Upper(); ++iP)
    {
      const gp_Pnt& aP = theL (iP);
      Standard_Real aTol = theLT ? theLT->Value(iP) : Precision::Confusion();
      BVH_Box <Standard_Real, 3> aBox (BVH_Vec3d (aP.X() - aTol, aP.Y() - aTol, aP.Z() - aTol),
                                       BVH_Vec3d (aP.X() + aTol, aP.Y() + aTol, aP.Z() + aTol));
      myPointBoxSet->Add (aP.XYZ(), aBox);
    }

    myPointBoxSet->Build();
  }

  ComputeExtremePoints();
}

//=======================================================================
// Function : ComputeExtremePoints
// purpose : 
//=======================================================================
void OBBTool::ComputeExtremePoints()
{
  // Six initial axes show great quality on the Optimal OBB, plus
  // the performance is better (due to the less number of operations).
  // But they show worse quality for the not optimal approach.
  //const Standard_Real a = (sqrt(5) - 1) / 2.;
  //const gp_XYZ anInitialAxes6[myNbInitAxes] = { gp_XYZ (0, 1, a),
  //                                              gp_XYZ (0, 1, -a),
  //                                              gp_XYZ (1, a, 0),
  //                                              gp_XYZ (1, -a, 0),
  //                                              gp_XYZ (a, 0, 1),
  //                                              gp_XYZ (a, 0, -1) };
  const Standard_Real aSqrt3 = Sqrt(3);
  const gp_XYZ anInitialAxes7[myNbInitAxes] = { gp_XYZ (1.0, 0.0, 0.0),
                                                gp_XYZ (0.0, 1.0, 0.0),
                                                gp_XYZ (0.0, 0.0, 1.0),
                                                gp_XYZ (1.0, 1.0, 1.0) / aSqrt3,
                                                gp_XYZ (1.0, 1.0, -1.0) / aSqrt3,
                                                gp_XYZ (1.0, -1.0, 1.0) / aSqrt3,
                                                gp_XYZ (1.0, -1.0, -1.0) / aSqrt3 };

  // Set of initial axes
  const gp_XYZ *anInitialAxesArray = anInitialAxes7;

  // Min and Max parameter
  Standard_Real aParams[myNbExtremalPoints];
  // Look for the extremal points (myLExtremalPoints)
  for (Standard_Integer anAxeInd = 0, aPrmInd = -1; anAxeInd < myNbInitAxes; ++anAxeInd)
  {
    Standard_Integer aMinInd = ++aPrmInd, aMaxInd = ++aPrmInd;
    aParams[aMinInd] = RealLast();
    aParams[aMaxInd] = -RealLast();
    Project (anInitialAxesArray[anAxeInd],
             aParams[aMinInd], aParams[aMaxInd],
             &myLExtremalPoints[aMinInd], &myLExtremalPoints[aMaxInd]);
  }

  // For not optimal box it is necessary to compute the max axis
  // created by the maximally distant extreme points
  if (!myOptimal)
  {
    for(Standard_Integer i = 0; i < 5; i++)
      myTriIdx[i] = INT_MAX;

    // Compute myTriIdx[0] and myTriIdx[1].
    Standard_Real aMaxSqDist = -1.0;
    for (Standard_Integer aPrmInd = 0; aPrmInd < myNbExtremalPoints; aPrmInd += 2)
    {
      const gp_Pnt &aP1 = myLExtremalPoints[aPrmInd],
        &aP2 = myLExtremalPoints[aPrmInd + 1];
      const Standard_Real aSqDist = aP1.SquareDistance(aP2);
      if (aSqDist > aMaxSqDist)
      {
        aMaxSqDist = aSqDist;
        myTriIdx[0] = aPrmInd;
        myTriIdx[1] = aPrmInd + 1;
      }
    }

    // Compute the maximal axis orthogonal to the found one
    FillToTriangle3();
  }
}

//=======================================================================
// Function : FillToTriangle3
// purpose : Two value of myTriIdx array is known. Let us find myTriIdx[2].
//            It must be in maximal distance from the infinite axis going
//            through the points with indexes myTriIdx[0] and myTriIdx[1].
//=======================================================================
void OBBTool::FillToTriangle3()
{
  const gp_XYZ &aP0 = myLExtremalPoints[myTriIdx[0]];
  const gp_XYZ anAxis = myLExtremalPoints[myTriIdx[1]] - aP0;
  Standard_Real aMaxSqDist = -1.0;
  for(Standard_Integer i = 0; i < myNbExtremalPoints; i++)
  {
    if((i == myTriIdx[0]) || (i == myTriIdx[1]))
      continue;

    const gp_XYZ &aP = myLExtremalPoints[i];
    const Standard_Real aDistToAxe = anAxis.CrossSquareMagnitude(aP - aP0);
    if(aDistToAxe > aMaxSqDist)
    {
      myTriIdx[2] = i;
      aMaxSqDist = aDistToAxe;
    }
  }
}

//=======================================================================
// Function : FillToTriangle5
// purpose : Three value of myTriIdx array is known.
//            Let us find myTriIdx[3] and myTriIdx[4].
//           They must be in the different sides of the plane of
//            triangle set by points myTriIdx[0], myTriIdx[1] and
//            myTriIdx[2]. Moreover, the distance from these points
//            to the triangle plane must be maximal.
//=======================================================================
void OBBTool::FillToTriangle5(const gp_XYZ& theNormal,
                              const gp_XYZ& theBarryCenter)
{
  Standard_Real aParams[2] = {0.0, 0.0};
  Standard_Integer id3 = -1, id4 = -1;

  for(Standard_Integer aPtIdx = 0; aPtIdx < myNbExtremalPoints; aPtIdx++)
  {
    if((aPtIdx == myTriIdx[0]) || (aPtIdx == myTriIdx[1]) || (aPtIdx == myTriIdx[2]))
      continue;

    const gp_XYZ &aCurrPoint = myLExtremalPoints[aPtIdx];
    const Standard_Real aParam = theNormal.Dot(aCurrPoint - theBarryCenter);

    if (aParam < aParams[0])
    {
      id3 = aPtIdx;
      aParams[0] = aParam;
    }
    else if (aParam > aParams[1])
    {
      id4 = aPtIdx;
      aParams[1] = aParam;
    }
  }

  // The points must be in the different sides of the triangle plane.
  if (id3 >= 0 && aParams[0] < -Precision::Confusion())
    myTriIdx[3] = id3;

  if (id4 >= 0 && aParams[1] > Precision::Confusion())
    myTriIdx[4] = id4;
}

//=======================================================================
// Function : ProcessTriangle
// purpose : Choose the optimal box with triple axes containing normal
//            to the triangle and some edge of the triangle (3rd axis is
//            computed from these two ones).
//=======================================================================
void OBBTool::ProcessTriangle(const Standard_Integer theIdx1,
                              const Standard_Integer theIdx2,
                              const Standard_Integer theIdx3,
                              const Standard_Boolean theIsBuiltTrg)
{
  const Standard_Integer aNbAxes = 3;

  // All axes must be normalized in order to provide correct area computation
  // (see ComputeQuality(...) method).
  int ID1[3] = { theIdx2, theIdx3, theIdx1 },
      ID2[3] = { theIdx1, theIdx2, theIdx3 };
  gp_XYZ aYAxis[aNbAxes] = {(myLExtremalPoints[ID1[0]] - myLExtremalPoints[ID2[0]]),
                            (myLExtremalPoints[ID1[1]] - myLExtremalPoints[ID2[1]]),
                            (myLExtremalPoints[ID1[2]] - myLExtremalPoints[ID2[2]])};

  // Normal to the triangle plane
  gp_XYZ aZAxis = aYAxis[0].Crossed(aYAxis[1]);

  Standard_Real aSqMod = aZAxis.SquareModulus();

  if (aSqMod < Precision::SquareConfusion())
    return;

  aZAxis /= Sqrt(aSqMod);

  gp_XYZ aXAxis[aNbAxes];
  for (Standard_Integer i = 0; i < aNbAxes; i++)
    aXAxis[i] = aYAxis[i].Crossed(aZAxis).Normalized();

  if (theIsBuiltTrg)
    FillToTriangle5 (aZAxis, myLExtremalPoints[theIdx1]);

  // Min and Max parameter
  const Standard_Integer aNbPoints = 2 * aNbAxes;

  // Compute Min/Max params for ZAxis
  Standard_Real aParams[aNbPoints];
  FindMinMax (aZAxis, aParams[4], aParams[5]); // Compute params on ZAxis once

  Standard_Integer aMinIdx = -1;
  for(Standard_Integer anAxeInd = 0; anAxeInd < aNbAxes; anAxeInd++)
  {
    const gp_XYZ &aAX = aXAxis[anAxeInd];
    // Compute params on XAxis
    FindMinMax (aAX, aParams[0], aParams[1]);
    // Compute params on YAxis checking for stored values
    ComputeParams (ID1[anAxeInd], ID2[anAxeInd], aParams[2], aParams[3]);

    const Standard_Real anArea = ComputeQuality(aParams);
    if (anArea < myQualityCriterion)
    {
      myQualityCriterion = anArea;
      aMinIdx = anAxeInd;
    }
  }

  if (aMinIdx < 0)
    return;

  myAxes[0] = aXAxis[aMinIdx];
  myAxes[1] = aYAxis[aMinIdx].Normalized();
  myAxes[2] = aZAxis;
}
//=======================================================================
// Function : ProcessDiTetrahedron
// purpose : DiTo-algorithm (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf)
//=======================================================================
void OBBTool::ProcessDiTetrahedron()
{
  // To compute the optimal OBB it is necessary to check all possible
  // axes created by the extremal points. It is also necessary to project
  // all the points on the axis, as for each different axis there will be
  // different extremal points.
  if (myOptimal)
  {
    for (Standard_Integer i = 0; i < myNbExtremalPoints - 2; i++)
    {
      for (Standard_Integer j = i + 1; j < myNbExtremalPoints - 1; j++)
      {
        for (Standard_Integer k = j + 1; k < myNbExtremalPoints; k++)
        {
          ProcessTriangle (i, j, k, Standard_False);
        }
      }
    }
  }
  else
  {
    // Use the standard DiTo approach
    ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[2], Standard_True);

    if (myTriIdx[3] <= myNbExtremalPoints)
    {
      ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[3], Standard_False);
      ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[3], Standard_False);
      ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[3], Standard_False);
    }

    if (myTriIdx[4] <= myNbExtremalPoints)
    {
      ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[4], Standard_False);
      ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[4], Standard_False);
      ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[4], Standard_False);
    }
  }
}

//=======================================================================
// Function : BuildBox
// purpose : 
//=======================================================================
void OBBTool::BuildBox(Bnd_OBB& theBox)
{
  theBox.SetVoid();

  // In fact, use Precision::SquareConfusion().
  const Standard_Boolean isOBB = myAxes[0].SquareModulus()*
                                 myAxes[1].SquareModulus()*
                                 myAxes[2].SquareModulus() > 1.0e-14;

  const gp_Dir aXDir = isOBB ? myAxes[0] : gp_Dir(1, 0, 0);
  const gp_Dir aYDir = isOBB ? myAxes[1] : gp_Dir(0, 1, 0);
  const gp_Dir aZDir = isOBB ? myAxes[2] : gp_Dir(0, 0, 1);

  const Standard_Integer aNbPoints = 6;
  Standard_Real aParams[aNbPoints];

  gp_XYZ aFCurrPoint = myPntsList.First().XYZ();
  
  aParams[0] = aParams[1] = aFCurrPoint.Dot(aXDir.XYZ());
  aParams[2] = aParams[3] = aFCurrPoint.Dot(aYDir.XYZ());
  aParams[4] = aParams[5] = aFCurrPoint.Dot(aZDir.XYZ());

  if(myListOfTolers != 0)
  {
    const Standard_Real aTol = myListOfTolers->First();
    aParams[0] -= aTol;
    aParams[1] += aTol;
    aParams[2] -= aTol;
    aParams[3] += aTol;
    aParams[4] -= aTol;
    aParams[5] += aTol;
  }

  for(Standard_Integer i = myPntsList.Lower() + 1; i <= myPntsList.Upper(); i++)
  {
    const gp_XYZ &aCurrPoint = myPntsList(i).XYZ();
    const Standard_Real aDx = aCurrPoint.Dot(aXDir.XYZ()),
                        aDy = aCurrPoint.Dot(aYDir.XYZ()),
                        aDz = aCurrPoint.Dot(aZDir.XYZ());
        
    if(myListOfTolers == 0)
    {
      SetMinMax(&aParams[0], aDx);
      SetMinMax(&aParams[2], aDy);
      SetMinMax(&aParams[4], aDz);
    }
    else
    {
      const Standard_Real aTol = myListOfTolers->Value(i);
      aParams[0] = Min(aParams[0], aDx - aTol);
      aParams[1] = Max(aParams[1], aDx + aTol);
      aParams[2] = Min(aParams[2], aDy - aTol);
      aParams[3] = Max(aParams[3], aDy + aTol);
      aParams[4] = Min(aParams[4], aDz - aTol);
      aParams[5] = Max(aParams[5], aDz + aTol);
    }
  }

  //Half-sizes
  const Standard_Real aHX = 0.5*(aParams[1] - aParams[0]);
  const Standard_Real aHY = 0.5*(aParams[3] - aParams[2]);
  const Standard_Real aHZ = 0.5*(aParams[5] - aParams[4]);

  const gp_XYZ aCenter = 0.5*((aParams[1] + aParams[0])*aXDir.XYZ() +
                              (aParams[3] + aParams[2])*aYDir.XYZ() +
                              (aParams[5] + aParams[4])*aZDir.XYZ());

  theBox.SetCenter(aCenter);
  theBox.SetXComponent(aXDir, aHX);
  theBox.SetYComponent(aYDir, aHY);
  theBox.SetZComponent(aZDir, aHZ);
  theBox.SetAABox(!isOBB);
}

// =======================================================================
// function : ReBuild
// purpose  : http://www.idt.mdh.se/~tla/publ/
// =======================================================================
void Bnd_OBB::ReBuild(const TColgp_Array1OfPnt& theListOfPoints,
                      const TColStd_Array1OfReal *theListOfTolerances,
                      const Standard_Boolean theIsOptimal)
{
  switch(theListOfPoints.Length())
  {
    case 1:
      ProcessOnePoint(theListOfPoints.First());
      if(theListOfTolerances)
        Enlarge(theListOfTolerances->First());
      return;
    case 2:
    {
      const Standard_Real aTol1 = (theListOfTolerances == 0) ? 0.0 :
                                      theListOfTolerances->First();

      const Standard_Real aTol2 = (theListOfTolerances == 0) ? 0.0 :
                                      theListOfTolerances->Last();
      
      const gp_XYZ &aP1 = theListOfPoints.First().XYZ(),
                   &aP2 = theListOfPoints.Last().XYZ();
      const gp_XYZ aDP = aP2 - aP1;
      const Standard_Real aDPm = aDP.Modulus();
      myIsAABox = Standard_False;
      myHDims[1] = myHDims[2] = Max(aTol1, aTol2);

      if(aDPm < Precision::Confusion())
      {
        ProcessOnePoint(aP1);
        Enlarge(myHDims[1] + Precision::Confusion());
        return;
      }

      myHDims[0] = 0.5*(aDPm+aTol1+aTol2);
      myAxes[0] = aDP/aDPm;
      if(Abs(myAxes[0].X()) > Abs(myAxes[0].Y()))
      {
        // Z-coord. is maximal or X-coord. is maximal
        myAxes[1].SetCoord(-myAxes[0].Z(), 0.0, myAxes[0].X());
      }
      else
      {
        // Z-coord. is maximal or Y-coord. is maximal
        myAxes[1].SetCoord(0.0, -myAxes[0].Z(), myAxes[0].Y());
      }

      myAxes[2] = myAxes[0].Crossed(myAxes[1]).Normalized();
      myCenter = aP1 + 0.5*(aDPm - aTol1 + aTol2)*myAxes[0];
    }
    return;
    default:
      break;
  }

  OBBTool aTool(theListOfPoints, theListOfTolerances, theIsOptimal);
  aTool.ProcessDiTetrahedron();
  aTool.BuildBox(*this);
}

// =======================================================================
// function : IsOut
// purpose  : 
// =======================================================================
Standard_Boolean Bnd_OBB::IsOut(const Bnd_OBB& theOther) const
{
  if (IsVoid() || theOther.IsVoid())
    return Standard_True;

  if (myIsAABox && theOther.myIsAABox)
  {
    return ((Abs(theOther.myCenter.X() - myCenter.X()) > theOther.myHDims[0] + myHDims[0]) ||
            (Abs(theOther.myCenter.Y() - myCenter.Y()) > theOther.myHDims[1] + myHDims[1]) ||
            (Abs(theOther.myCenter.Z() - myCenter.Z()) > theOther.myHDims[2] + myHDims[2]));
  }

  // According to the Separating Axis Theorem for Oriented Bounding Boxes
  // it is necessary to check the 15 separating axes (Ls):
  // - 6 axes of the boxes;
  // - 9 cross products of the axes of the boxes.
  // If any of these axes is valid, the boxes do not interfere.

  // The algorithm is following:
  // 1. Compute the "length" for j-th BndBox (j=1...2) according to the formula:
  //    L(j)=Sum(myHDims[i]*Abs(myAxes[i].Dot(Ls)))
  // 2. If (theCenter2 - theCenter1).Dot(Ls) > (L(1) + L(2))
  //    then the considered OBBs are not interfered in terms of the axis Ls.
  //
  // If OBBs are not interfered in terms of at least one axis (of 15) then
  // they are not interfered at all.

  // Precomputed difference between centers
  gp_XYZ D = theOther.myCenter - myCenter;

  // Check the axes of the this box, i.e. L is one of myAxes
  // Since the Dot product of two of these directions is null, it could be skipped:
  // myXDirection.Dot(myYDirection) = 0

  for(Standard_Integer i = 0; i < 3; ++i)
  {
    // Length of the second segment
    Standard_Real aLSegm2 = 0;
    for(Standard_Integer j = 0; j < 3; ++j)
      aLSegm2 += theOther.myHDims[j] * Abs(theOther.myAxes[j].Dot(myAxes[i]));

    // Distance between projected centers
    Standard_Real aDistCC = Abs(D.Dot(myAxes[i]));

    if(aDistCC > myHDims[i] + aLSegm2)
      return Standard_True;
  }

  // Check the axes of the Other box, i.e. L is one of theOther.myAxes

  for(Standard_Integer i = 0; i < 3; ++i)
  {
    // Length of the first segment
    Standard_Real aLSegm1 = 0.;
    for(Standard_Integer j = 0; j < 3; ++j)
      aLSegm1 += myHDims[j] * Abs(myAxes[j].Dot(theOther.myAxes[i]));

    // Distance between projected centers
    Standard_Real aDistCC = Abs(D.Dot(theOther.myAxes[i]));

    if(aDistCC > aLSegm1 + theOther.myHDims[i])
      return Standard_True;
  }

  const Standard_Real aTolNull = Epsilon(1.0);

  // Check the axes produced by the cross products
  for(Standard_Integer i = 0; i < 3; ++i)
  {
    for(Standard_Integer j = 0; j < 3; ++j)
    {
      // Separating axis
      gp_XYZ aLAxe = myAxes[i].Crossed(theOther.myAxes[j]);

      const Standard_Real aNorm = aLAxe.Modulus();
      if(aNorm < aTolNull)
        continue;

      aLAxe /= aNorm;

      // Length of the first segment
      Standard_Real aLSegm1 = 0.;
      for(Standard_Integer k = 0; k < 3; ++k)
        aLSegm1 += myHDims[k] * Abs(myAxes[k].Dot(aLAxe));

      // Length of the second segment
      Standard_Real aLSegm2 = 0.;
      for(Standard_Integer k = 0; k < 3; ++k)
        aLSegm2 += theOther.myHDims[k] * Abs(theOther.myAxes[k].Dot(aLAxe));

      // Distance between projected centers
      Standard_Real aDistCC = Abs(D.Dot(aLAxe));

      if(aDistCC > aLSegm1 + aLSegm2)
        return Standard_True;
    }
  }

  return Standard_False;
}

// =======================================================================
// function : IsOut
// purpose  : 
// =======================================================================
Standard_Boolean Bnd_OBB::IsOut(const gp_Pnt& theP) const
{
  // 1. Project the point to myAxes[i] (i=0...2).
  // 2. Check, whether the absolute value of the correspond
  //    projection parameter is greater than myHDims[i].
  //    In this case, IsOut method will return TRUE.

  const gp_XYZ aRV = theP.XYZ() - myCenter;

  return ((Abs(myAxes[0].Dot(aRV)) > myHDims[0]) || 
          (Abs(myAxes[1].Dot(aRV)) > myHDims[1]) ||
          (Abs(myAxes[2].Dot(aRV)) > myHDims[2]));
}

// =======================================================================
// function : IsCompletelyInside
// purpose  : Checks if every vertex of theOther is completely inside *this
// =======================================================================
Standard_Boolean Bnd_OBB::IsCompletelyInside(const Bnd_OBB& theOther) const
{
  if(IsVoid() || theOther.IsVoid())
    return Standard_False;

  gp_Pnt aVert[8];
  theOther.GetVertex(aVert);
  for(Standard_Integer i = 0; i < 8; i++)
  {
    if(IsOut(aVert[i]))
      return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : Add
// purpose  : 
// =======================================================================
void Bnd_OBB::Add(const gp_Pnt& theP)
{
  if (IsVoid())
  {
    myCenter = theP.XYZ();
    myAxes[0] = gp::DX().XYZ();
    myAxes[1] = gp::DY().XYZ();
    myAxes[2] = gp::DZ().XYZ();
    myHDims[0] = 0.0;
    myHDims[1] = 0.0;
    myHDims[2] = 0.0;
    myIsAABox = Standard_True;
  }
  else
  {
    gp_Pnt aList[9];
    GetVertex(aList);
    aList[8] = theP;
    ReBuild(TColgp_Array1OfPnt(aList[0], 0, 8));
  }
}

// =======================================================================
// function : Add
// purpose  : 
// =======================================================================
void Bnd_OBB::Add(const Bnd_OBB& theOther)
{
  if (!theOther.IsVoid())
  {
    if (IsVoid())
    {
      myCenter = theOther.myCenter;
      myAxes[0] = theOther.myAxes[0];
      myAxes[1] = theOther.myAxes[1];
      myAxes[2] = theOther.myAxes[2];
      myHDims[0] = theOther.myHDims[0];
      myHDims[1] = theOther.myHDims[1];
      myHDims[2] = theOther.myHDims[2];
      myIsAABox = theOther.myIsAABox;
    }
    else
    {
      gp_Pnt aList[16];
      GetVertex(&aList[0]);
      theOther.GetVertex(&aList[8]);
      ReBuild(TColgp_Array1OfPnt(aList[0], 0, 15));
    }
  }
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Bnd_OBB::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, Bnd_OBB)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCenter)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAxes[0])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAxes[1])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAxes[2])

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHDims[0])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHDims[1])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHDims[2])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsAABox)
}
