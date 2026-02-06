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

namespace
{
// Precomputed sqrt(3)
constexpr double SQRT_3 = 1.7320508075688772935;
} // namespace

//! Auxiliary class to select from the points stored in
//! BVH tree the two points giving the extreme projection
//! parameters on the axis
class OBB_ExtremePointsSelector
    : public BVH_Traverse<double, 3, BVH_BoxSet<double, 3, gp_XYZ>, Bnd_Range>
{
public:
  //! Constructor
  OBB_ExtremePointsSelector()
      : myPrmMin(RealLast()),
        myPrmMax(RealFirst())
  {
  }

public: //! @name Set axis for projection
  //! Sets the axis
  void SetAxis(const gp_XYZ& theAxis) { myAxis = theAxis; }

public: //! @name Clears the points from previous runs
  //! Clear
  void Clear()
  {
    myPrmMin = RealLast();
    myPrmMax = RealFirst();
  }

public: //! @name Getting the results
  //! Returns the minimal projection parameter
  double MinPrm() const { return myPrmMin; }

  //! Returns the maximal projection parameter
  double MaxPrm() const { return myPrmMax; }

  //! Returns the minimal projection point
  const gp_XYZ& MinPnt() const { return myPntMin; }

  //! Returns the maximal projection point
  const gp_XYZ& MaxPnt() const { return myPntMax; }

public: //! @name Definition of rejection/acceptance rules
  //! Defines the rules for node rejection
  bool RejectNode(const BVH_Vec3d& theCMin,
                  const BVH_Vec3d& theCMax,
                  Bnd_Range&       theMetric) const override
  {
    if (myPrmMin > myPrmMax)
      // No parameters computed yet
      return false;

    double aPrmMin = myPrmMin, aPrmMax = myPrmMax;
    bool   isToReject = true;

    // Check if the current node is between already found parameters
    for (int i = 0; i < 2; ++i)
    {
      double x = !i ? theCMin.x() : theCMax.x();
      for (int j = 0; j < 2; ++j)
      {
        double y = !j ? theCMin.y() : theCMax.y();
        for (int k = 0; k < 2; ++k)
        {
          double z = !k ? theCMin.z() : theCMax.z();

          double aPrm = myAxis.Dot(gp_XYZ(x, y, z));
          if (aPrm < aPrmMin)
          {
            aPrmMin    = aPrm;
            isToReject = false;
          }
          else if (aPrm > aPrmMax)
          {
            aPrmMax    = aPrm;
            isToReject = false;
          }
        }
      }
    }

    theMetric = Bnd_Range(aPrmMin, aPrmMax);

    return isToReject;
  }

  //! Rules for node rejection by the metric
  bool RejectMetric(const Bnd_Range& theMetric) const override
  {
    if (myPrmMin > myPrmMax)
      // no parameters computed
      return false;

    double aMin, aMax;
    if (!theMetric.GetBounds(aMin, aMax))
      // void metric
      return false;

    // Check if the box of the branch is inside of the already computed parameters
    return aMin > myPrmMin && aMax < myPrmMax;
  }

  //! Defines the rules for leaf acceptance
  bool Accept(const int theIndex, const Bnd_Range&) override
  {
    const gp_XYZ& theLeaf = myBVHSet->Element(theIndex);
    double        aPrm    = myAxis.Dot(theLeaf);
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
    return true;
  }

public: //! @name Choosing the best branch
  //! Returns true if the metric of the left branch is better than the metric of the right
  bool IsMetricBetter(const Bnd_Range& theLeft, const Bnd_Range& theRight) const override
  {
    if (myPrmMin > myPrmMax)
      // no parameters computed
      return true;

    double aMin[2], aMax[2];
    if (!theLeft.GetBounds(aMin[0], aMax[0]) || !theRight.GetBounds(aMin[1], aMax[1]))
      // void metrics
      return true;

    // Choose branch with larger extension over computed parameters
    double anExt[2] = {0.0, 0.0};
    for (int i = 0; i < 2; ++i)
    {
      if (aMin[i] < myPrmMin)
        anExt[i] += myPrmMin - aMin[i];
      if (aMax[i] > myPrmMax)
        anExt[i] += aMax[i] - myPrmMax;
    }
    return anExt[0] > anExt[1];
  }

protected:         //! @name Fields
  gp_XYZ myAxis;   //!< Axis to project the points to
  double myPrmMin; //!< Minimal projection parameter
  double myPrmMax; //!< Maximal projection parameter
  gp_XYZ myPntMin; //!< Minimal projection point
  gp_XYZ myPntMax; //!< Maximal projection point
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
  OBBTool(const NCollection_Array1<gp_Pnt>& theL,
          const NCollection_Array1<double>* theLT        = nullptr,
          bool                              theIsOptimal = false);

  //! DiTO algorithm for OBB construction
  //! (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf)
  void ProcessDiTetrahedron();

  //! Creates OBB with already computed parameters
  void BuildBox(Bnd_OBB& theBox);

protected:
  // Computes the extreme points on the set of Initial axes
  void ComputeExtremePoints();

  //! Works with the triangle set by the points in myTriIdx.
  //! If theIsBuiltTrg == TRUE, new set of triangles will be
  //! recomputed.
  void ProcessTriangle(const int  theIdx1,
                       const int  theIdx2,
                       const int  theIdx3,
                       const bool theIsBuiltTrg);

  //! Computes myTriIdx[2]
  void FillToTriangle3();

  //! Computes myTriIdx[3] and myTriIdx[4]
  void FillToTriangle5(const gp_XYZ& theNormal, const gp_XYZ& theBarryCenter);

  //! Returns half of the Surface area of the box
  static double ComputeQuality(const double* const thePrmArr)
  {
    const double aDX = thePrmArr[1] - thePrmArr[0], aDY = thePrmArr[3] - thePrmArr[2],
                 aDZ = thePrmArr[5] - thePrmArr[4];

    return (aDX * aDY + aDY * aDZ + aDX * aDZ);
  }

protected:
  //! Assignment operator is forbidden
  OBBTool& operator=(const OBBTool&);

private:
  //! Params structure stores the two values meaning
  //! min and max parameters on the axis
  struct Params
  {
    Params()
        : _ParamMin(RealLast()),
          _ParamMax(RealFirst())
    {
    }

    Params(double theMin, double theMax)
        : _ParamMin(theMin),
          _ParamMax(theMax)
    {
    }

    double _ParamMin;
    double _ParamMax;
  };

  //! Computes the Minimal and maximal parameters on the vector
  //! connecting the points myLExtremalPoints[theId1] and myLExtremalPoints[theId2]
  void ComputeParams(const int theId1, const int theId2, double& theMin, double& theMax)
  {
    theMin = myParams[theId1][theId2]._ParamMin;
    theMax = myParams[theId1][theId2]._ParamMax;

    if (theMin > theMax)
    {
      FindMinMax((myLExtremalPoints[theId1] - myLExtremalPoints[theId2]).Normalized(),
                 theMin,
                 theMax);
      myParams[theId1][theId2]._ParamMin = myParams[theId2][theId1]._ParamMin = theMin;
      myParams[theId1][theId2]._ParamMax = myParams[theId2][theId1]._ParamMax = theMax;
    }
  }

  //! Looks for the min-max parameters on the axis.
  //! For optimal case projects all the points on the axis,
  //! for not optimal - only the set of extreme points.
  void FindMinMax(const gp_XYZ& theAxis, double& theMin, double& theMax)
  {
    theMin = RealLast(), theMax = RealFirst();

    if (myOptimal)
      Project(theAxis, theMin, theMax);
    else
    {
      for (int i = 0; i < myNbExtremalPoints; ++i)
      {
        double aPrm = theAxis.Dot(myLExtremalPoints[i]);
        if (aPrm < theMin)
          theMin = aPrm;
        if (aPrm > theMax)
          theMax = aPrm;
      }
    }
  }

  //! Projects the set of points on the axis
  void Project(const gp_XYZ& theAxis,
               double&       theMin,
               double&       theMax,
               gp_XYZ*       thePntMin = nullptr,
               gp_XYZ*       thePntMax = nullptr)
  {
    theMin = RealLast(), theMax = RealFirst();

    if (myOptimal)
    {
      // Project BVH
      OBB_ExtremePointsSelector anExtremePointsSelector;
      anExtremePointsSelector.SetBVHSet(myPointBoxSet.get());
      anExtremePointsSelector.SetAxis(theAxis);
      anExtremePointsSelector.Select();
      theMin = anExtremePointsSelector.MinPrm();
      theMax = anExtremePointsSelector.MaxPrm();
      if (thePntMin)
        *thePntMin = anExtremePointsSelector.MinPnt();
      if (thePntMax)
        *thePntMax = anExtremePointsSelector.MaxPnt();
    }
    else
    {
      // Project all points
      for (int iP = myPntsList.Lower(); iP <= myPntsList.Upper(); ++iP)
      {
        const gp_XYZ& aPoint = myPntsList(iP).XYZ();
        const double  aPrm   = theAxis.Dot(aPoint);
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
  static const int myNbInitAxes = 7;

  //! Number of extremal points
  static const int myNbExtremalPoints = 2 * myNbInitAxes;

  //! The source list of points
  const NCollection_Array1<gp_Pnt>& myPntsList;

  //! Pointer to the array of tolerances
  const NCollection_Array1<double>* myListOfTolers;

  //! Points of ditetrahedron
  //! given by their indices in myLExtremalPoints.
  int myTriIdx[5]{};

  //! List of extremal points
  gp_XYZ myLExtremalPoints[myNbExtremalPoints];

  //! The axes of the box (always normalized or
  //! can be null-vector)
  gp_XYZ myAxes[3];

  //! The surface area of the OBB
  double myQualityCriterion;

  //! Defines if the OBB should be computed more tight.
  //! Takes more time, but the volume is less.
  bool myOptimal;

  //! Point box set organized with BVH
  opencascade::handle<BVH_BoxSet<double, 3, gp_XYZ>> myPointBoxSet;

  //! Stored min/max parameters for the axes between extremal points
  Params myParams[myNbExtremalPoints][myNbExtremalPoints];
};

//=======================================================================
// Function : SetMinMax
// purpose :
//    ATTENTION!!! thePrmArr must be initialized before this method calling.
//=======================================================================
static inline void SetMinMax(double* const thePrmArr, const double theNewParam)
{
  if (theNewParam < thePrmArr[0])
  {
    thePrmArr[0] = theNewParam;
  }
  if (theNewParam > thePrmArr[1])
  {
    thePrmArr[1] = theNewParam;
  }
}

//=================================================================================================

OBBTool::OBBTool(const NCollection_Array1<gp_Pnt>& theL,
                 const NCollection_Array1<double>* theLT,
                 const bool                        theIsOptimal)
    : myPntsList(theL),
      myListOfTolers(theLT),
      myQualityCriterion(RealLast()),
      myOptimal(theIsOptimal)
{
  if (myOptimal)
  {
    // Use linear builder for BVH construction with 30 elements in the leaf
    opencascade::handle<BVH_LinearBuilder<double, 3>> aLBuilder =
      new BVH_LinearBuilder<double, 3>(30);
    myPointBoxSet = new BVH_BoxSet<double, 3, gp_XYZ>(aLBuilder);
    myPointBoxSet->SetSize(myPntsList.Length());

    // Add the points into Set
    for (int iP = theL.Lower(); iP <= theL.Upper(); ++iP)
    {
      const gp_Pnt&      aP   = theL(iP);
      double             aTol = theLT ? theLT->Value(iP) : Precision::Confusion();
      BVH_Box<double, 3> aBox(BVH_Vec3d(aP.X() - aTol, aP.Y() - aTol, aP.Z() - aTol),
                              BVH_Vec3d(aP.X() + aTol, aP.Y() + aTol, aP.Z() + aTol));
      myPointBoxSet->Add(aP.XYZ(), aBox);
    }

    myPointBoxSet->Build();
  }

  ComputeExtremePoints();
}

//=================================================================================================

void OBBTool::ComputeExtremePoints()
{
  // Six initial axes show great quality on the Optimal OBB, plus
  // the performance is better (due to the less number of operations).
  // But they show worse quality for the not optimal approach.
  // const double a = (sqrt(5) - 1) / 2.;
  // const gp_XYZ anInitialAxes6[myNbInitAxes] = { gp_XYZ (0, 1, a),
  //                                              gp_XYZ (0, 1, -a),
  //                                              gp_XYZ (1, a, 0),
  //                                              gp_XYZ (1, -a, 0),
  //                                              gp_XYZ (a, 0, 1),
  //                                              gp_XYZ (a, 0, -1) };
  const gp_XYZ anInitialAxes7[myNbInitAxes] = {gp_XYZ(1.0, 0.0, 0.0),
                                               gp_XYZ(0.0, 1.0, 0.0),
                                               gp_XYZ(0.0, 0.0, 1.0),
                                               gp_XYZ(1.0, 1.0, 1.0) / SQRT_3,
                                               gp_XYZ(1.0, 1.0, -1.0) / SQRT_3,
                                               gp_XYZ(1.0, -1.0, 1.0) / SQRT_3,
                                               gp_XYZ(1.0, -1.0, -1.0) / SQRT_3};

  // Set of initial axes
  const gp_XYZ* anInitialAxesArray = anInitialAxes7;

  // Min and Max parameter
  double aParams[myNbExtremalPoints];
  // Look for the extremal points (myLExtremalPoints)
  for (int anAxeInd = 0, aPrmInd = -1; anAxeInd < myNbInitAxes; ++anAxeInd)
  {
    int aMinInd = ++aPrmInd, aMaxInd = ++aPrmInd;
    aParams[aMinInd] = RealLast();
    aParams[aMaxInd] = -RealLast();
    Project(anInitialAxesArray[anAxeInd],
            aParams[aMinInd],
            aParams[aMaxInd],
            &myLExtremalPoints[aMinInd],
            &myLExtremalPoints[aMaxInd]);
  }

  // For not optimal box it is necessary to compute the max axis
  // created by the maximally distant extreme points
  if (!myOptimal)
  {
    for (int i = 0; i < 5; i++)
      myTriIdx[i] = INT_MAX;

    // Compute myTriIdx[0] and myTriIdx[1].
    double aMaxSqDist = -1.0;
    for (int aPrmInd = 0; aPrmInd < myNbExtremalPoints; aPrmInd += 2)
    {
      const gp_Pnt &aP1 = myLExtremalPoints[aPrmInd], &aP2 = myLExtremalPoints[aPrmInd + 1];
      const double  aSqDist = aP1.SquareDistance(aP2);
      if (aSqDist > aMaxSqDist)
      {
        aMaxSqDist  = aSqDist;
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
  const gp_XYZ& aP0        = myLExtremalPoints[myTriIdx[0]];
  const gp_XYZ  anAxis     = myLExtremalPoints[myTriIdx[1]] - aP0;
  double        aMaxSqDist = -1.0;
  for (int i = 0; i < myNbExtremalPoints; i++)
  {
    if ((i == myTriIdx[0]) || (i == myTriIdx[1]))
      continue;

    const gp_XYZ& aP         = myLExtremalPoints[i];
    const double  aDistToAxe = anAxis.CrossSquareMagnitude(aP - aP0);
    if (aDistToAxe > aMaxSqDist)
    {
      myTriIdx[2] = i;
      aMaxSqDist  = aDistToAxe;
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
void OBBTool::FillToTriangle5(const gp_XYZ& theNormal, const gp_XYZ& theBarryCenter)
{
  double aParams[2] = {0.0, 0.0};
  int    id3 = -1, id4 = -1;

  for (int aPtIdx = 0; aPtIdx < myNbExtremalPoints; aPtIdx++)
  {
    if ((aPtIdx == myTriIdx[0]) || (aPtIdx == myTriIdx[1]) || (aPtIdx == myTriIdx[2]))
      continue;

    const gp_XYZ& aCurrPoint = myLExtremalPoints[aPtIdx];
    const double  aParam     = theNormal.Dot(aCurrPoint - theBarryCenter);

    if (aParam < aParams[0])
    {
      id3        = aPtIdx;
      aParams[0] = aParam;
    }
    else if (aParam > aParams[1])
    {
      id4        = aPtIdx;
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
void OBBTool::ProcessTriangle(const int  theIdx1,
                              const int  theIdx2,
                              const int  theIdx3,
                              const bool theIsBuiltTrg)
{
  const int aNbAxes = 3;

  // All axes must be normalized in order to provide correct area computation
  // (see ComputeQuality(...) method).
  int    ID1[3] = {theIdx2, theIdx3, theIdx1}, ID2[3] = {theIdx1, theIdx2, theIdx3};
  gp_XYZ aYAxis[aNbAxes] = {(myLExtremalPoints[ID1[0]] - myLExtremalPoints[ID2[0]]),
                            (myLExtremalPoints[ID1[1]] - myLExtremalPoints[ID2[1]]),
                            (myLExtremalPoints[ID1[2]] - myLExtremalPoints[ID2[2]])};

  // Normal to the triangle plane
  gp_XYZ aZAxis = aYAxis[0].Crossed(aYAxis[1]);

  double aSqMod = aZAxis.SquareModulus();

  if (aSqMod < Precision::SquareConfusion())
    return;

  aZAxis /= std::sqrt(aSqMod);

  gp_XYZ aXAxis[aNbAxes];
  bool   aXAxisValid[aNbAxes];
  for (int i = 0; i < aNbAxes; i++)
  {
    const gp_XYZ aCross = aYAxis[i].Crossed(aZAxis);
    const double aCrossSqMod = aCross.SquareModulus();
    if (aCrossSqMod < Precision::SquareConfusion())
    {
      aXAxisValid[i] = false;
      continue;
    }
    aXAxis[i]      = aCross / std::sqrt(aCrossSqMod);
    aXAxisValid[i] = true;
  }

  if (theIsBuiltTrg)
    FillToTriangle5(aZAxis, myLExtremalPoints[theIdx1]);

  // Min and Max parameter
  const int aNbPoints = 2 * aNbAxes;

  // Compute Min/Max params for ZAxis
  double aParams[aNbPoints];
  FindMinMax(aZAxis, aParams[4], aParams[5]); // Compute params on ZAxis once

  int aMinIdx = -1;
  for (int anAxeInd = 0; anAxeInd < aNbAxes; anAxeInd++)
  {
    if (!aXAxisValid[anAxeInd])
      continue;

    const gp_XYZ& aAX = aXAxis[anAxeInd];
    // Compute params on XAxis
    FindMinMax(aAX, aParams[0], aParams[1]);
    // Compute params on YAxis checking for stored values
    ComputeParams(ID1[anAxeInd], ID2[anAxeInd], aParams[2], aParams[3]);

    const double anArea = ComputeQuality(aParams);
    if (anArea < myQualityCriterion)
    {
      myQualityCriterion = anArea;
      aMinIdx            = anAxeInd;
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
    for (int i = 0; i < myNbExtremalPoints - 2; i++)
    {
      for (int j = i + 1; j < myNbExtremalPoints - 1; j++)
      {
        for (int k = j + 1; k < myNbExtremalPoints; k++)
        {
          ProcessTriangle(i, j, k, false);
        }
      }
    }
  }
  else
  {
    // Use the standard DiTo approach
    ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[2], true);

    if (myTriIdx[3] < myNbExtremalPoints)
    {
      ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[3], false);
      ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[3], false);
      ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[3], false);
    }

    if (myTriIdx[4] < myNbExtremalPoints)
    {
      ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[4], false);
      ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[4], false);
      ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[4], false);
    }
  }
}

//=================================================================================================

void OBBTool::BuildBox(Bnd_OBB& theBox)
{
  theBox.SetVoid();

  // In fact, use Precision::SquareConfusion().
  const bool isOBB =
    myAxes[0].SquareModulus() * myAxes[1].SquareModulus() * myAxes[2].SquareModulus() > 1.0e-14;

  const gp_Dir aXDir = isOBB ? myAxes[0] : gp_Dir(gp_Dir::D::X);
  const gp_Dir aYDir = isOBB ? myAxes[1] : gp_Dir(gp_Dir::D::Y);
  const gp_Dir aZDir = isOBB ? myAxes[2] : gp_Dir(gp_Dir::D::Z);

  const int aNbPoints = 6;
  double    aParams[aNbPoints];

  gp_XYZ aFCurrPoint = myPntsList.First().XYZ();

  aParams[0] = aParams[1] = aFCurrPoint.Dot(aXDir.XYZ());
  aParams[2] = aParams[3] = aFCurrPoint.Dot(aYDir.XYZ());
  aParams[4] = aParams[5] = aFCurrPoint.Dot(aZDir.XYZ());

  if (myListOfTolers != nullptr)
  {
    const double aTol = myListOfTolers->First();
    aParams[0] -= aTol;
    aParams[1] += aTol;
    aParams[2] -= aTol;
    aParams[3] += aTol;
    aParams[4] -= aTol;
    aParams[5] += aTol;
  }

  for (int i = myPntsList.Lower() + 1; i <= myPntsList.Upper(); i++)
  {
    const gp_XYZ& aCurrPoint = myPntsList(i).XYZ();
    const double  aDx = aCurrPoint.Dot(aXDir.XYZ()), aDy = aCurrPoint.Dot(aYDir.XYZ()),
                 aDz = aCurrPoint.Dot(aZDir.XYZ());

    if (myListOfTolers == nullptr)
    {
      SetMinMax(&aParams[0], aDx);
      SetMinMax(&aParams[2], aDy);
      SetMinMax(&aParams[4], aDz);
    }
    else
    {
      const double aTol = myListOfTolers->Value(i);
      aParams[0]        = std::min(aParams[0], aDx - aTol);
      aParams[1]        = std::max(aParams[1], aDx + aTol);
      aParams[2]        = std::min(aParams[2], aDy - aTol);
      aParams[3]        = std::max(aParams[3], aDy + aTol);
      aParams[4]        = std::min(aParams[4], aDz - aTol);
      aParams[5]        = std::max(aParams[5], aDz + aTol);
    }
  }

  // Half-sizes
  const double aHX = 0.5 * (aParams[1] - aParams[0]);
  const double aHY = 0.5 * (aParams[3] - aParams[2]);
  const double aHZ = 0.5 * (aParams[5] - aParams[4]);

  const gp_XYZ aCenter =
    0.5
    * ((aParams[1] + aParams[0]) * aXDir.XYZ() + (aParams[3] + aParams[2]) * aYDir.XYZ()
       + (aParams[5] + aParams[4]) * aZDir.XYZ());

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
void Bnd_OBB::ReBuild(const NCollection_Array1<gp_Pnt>& theListOfPoints,
                      const NCollection_Array1<double>* theListOfTolerances,
                      const bool                        theIsOptimal)
{
  switch (theListOfPoints.Length())
  {
    case 1:
      ProcessOnePoint(theListOfPoints.First());
      if (theListOfTolerances)
        Enlarge(theListOfTolerances->First());
      return;
    case 2: {
      const double aTol1 = (theListOfTolerances == nullptr) ? 0.0 : theListOfTolerances->First();

      const double aTol2 = (theListOfTolerances == nullptr) ? 0.0 : theListOfTolerances->Last();

      const gp_XYZ &aP1 = theListOfPoints.First().XYZ(), &aP2 = theListOfPoints.Last().XYZ();
      const gp_XYZ  aDP  = aP2 - aP1;
      const double  aDPm = aDP.Modulus();
      myIsAABox          = false;
      myHDims[1] = myHDims[2] = std::max(aTol1, aTol2);

      if (aDPm < Precision::Confusion())
      {
        ProcessOnePoint(aP1);
        Enlarge(myHDims[1] + Precision::Confusion());
        return;
      }

      myHDims[0] = 0.5 * (aDPm + aTol1 + aTol2);
      myAxes[0]  = aDP / aDPm;
      if (std::abs(myAxes[0].X()) > std::abs(myAxes[0].Y()))
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
      myCenter  = aP1 + 0.5 * (aDPm - aTol1 + aTol2) * myAxes[0];
    }
      return;
    default:
      break;
  }

  OBBTool aTool(theListOfPoints, theListOfTolerances, theIsOptimal);
  aTool.ProcessDiTetrahedron();
  aTool.BuildBox(*this);
}

//=================================================================================================

bool Bnd_OBB::IsOut(const Bnd_OBB& theOther) const
{
  if (IsVoid() || theOther.IsVoid())
    return true;

  if (myIsAABox && theOther.myIsAABox)
  {
    return ((std::abs(theOther.myCenter.X() - myCenter.X()) > theOther.myHDims[0] + myHDims[0])
            || (std::abs(theOther.myCenter.Y() - myCenter.Y()) > theOther.myHDims[1] + myHDims[1])
            || (std::abs(theOther.myCenter.Z() - myCenter.Z()) > theOther.myHDims[2] + myHDims[2]));
  }

  // According to the Separating Axis Theorem for Oriented Bounding Boxes
  // it is necessary to check the 15 separating axes (Ls):
  // - 6 axes of the boxes;
  // - 9 cross products of the axes of the boxes.
  // If any of these axes is valid, the boxes do not interfere.

  // The algorithm is following:
  // 1. Compute the "length" for j-th BndBox (j=1...2) according to the formula:
  //    L(j)=Sum(myHDims[i]*std::abs(myAxes[i].Dot(Ls)))
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

  for (int i = 0; i < 3; ++i)
  {
    // Length of the second segment
    double aLSegm2 = 0;
    for (int j = 0; j < 3; ++j)
      aLSegm2 += theOther.myHDims[j] * std::abs(theOther.myAxes[j].Dot(myAxes[i]));

    // Distance between projected centers
    double aDistCC = std::abs(D.Dot(myAxes[i]));

    if (aDistCC > myHDims[i] + aLSegm2)
      return true;
  }

  // Check the axes of the Other box, i.e. L is one of theOther.myAxes

  for (int i = 0; i < 3; ++i)
  {
    // Length of the first segment
    double aLSegm1 = 0.;
    for (int j = 0; j < 3; ++j)
      aLSegm1 += myHDims[j] * std::abs(myAxes[j].Dot(theOther.myAxes[i]));

    // Distance between projected centers
    double aDistCC = std::abs(D.Dot(theOther.myAxes[i]));

    if (aDistCC > aLSegm1 + theOther.myHDims[i])
      return true;
  }

  const double aTolNull = Epsilon(1.0);

  // Check the axes produced by the cross products
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      // Separating axis
      gp_XYZ aLAxe = myAxes[i].Crossed(theOther.myAxes[j]);

      const double aNorm = aLAxe.Modulus();
      if (aNorm < aTolNull)
        continue;

      aLAxe /= aNorm;

      // Length of the first segment
      double aLSegm1 = 0.;
      for (int k = 0; k < 3; ++k)
        aLSegm1 += myHDims[k] * std::abs(myAxes[k].Dot(aLAxe));

      // Length of the second segment
      double aLSegm2 = 0.;
      for (int k = 0; k < 3; ++k)
        aLSegm2 += theOther.myHDims[k] * std::abs(theOther.myAxes[k].Dot(aLAxe));

      // Distance between projected centers
      double aDistCC = std::abs(D.Dot(aLAxe));

      if (aDistCC > aLSegm1 + aLSegm2)
        return true;
    }
  }

  return false;
}

//=================================================================================================

bool Bnd_OBB::IsOut(const gp_Pnt& theP) const
{
  // 1. Project the point to myAxes[i] (i=0...2).
  // 2. Check, whether the absolute value of the correspond
  //    projection parameter is greater than myHDims[i].
  //    In this case, IsOut method will return TRUE.

  const gp_XYZ aRV = theP.XYZ() - myCenter;

  return ((std::abs(myAxes[0].Dot(aRV)) > myHDims[0]) || (std::abs(myAxes[1].Dot(aRV)) > myHDims[1])
          || (std::abs(myAxes[2].Dot(aRV)) > myHDims[2]));
}

// =======================================================================
// function : IsCompletelyInside
// purpose  : Checks if every vertex of theOther is completely inside *this
// =======================================================================
bool Bnd_OBB::IsCompletelyInside(const Bnd_OBB& theOther) const
{
  if (IsVoid() || theOther.IsVoid())
    return false;

  gp_Pnt aVert[8];
  theOther.GetVertex(aVert);
  for (int i = 0; i < 8; i++)
  {
    if (IsOut(aVert[i]))
      return false;
  }

  return true;
}

//=================================================================================================

void Bnd_OBB::Add(const gp_Pnt& theP)
{
  if (IsVoid())
  {
    myCenter   = theP.XYZ();
    myAxes[0]  = gp::DX().XYZ();
    myAxes[1]  = gp::DY().XYZ();
    myAxes[2]  = gp::DZ().XYZ();
    myHDims[0] = 0.0;
    myHDims[1] = 0.0;
    myHDims[2] = 0.0;
    myIsAABox  = true;
  }
  else
  {
    gp_Pnt aList[9];
    GetVertex(aList);
    aList[8] = theP;
    ReBuild(NCollection_Array1<gp_Pnt>(aList[0], 0, 8));
  }
}

//=================================================================================================

void Bnd_OBB::Add(const Bnd_OBB& theOther)
{
  if (!theOther.IsVoid())
  {
    if (IsVoid())
    {
      myCenter   = theOther.myCenter;
      myAxes[0]  = theOther.myAxes[0];
      myAxes[1]  = theOther.myAxes[1];
      myAxes[2]  = theOther.myAxes[2];
      myHDims[0] = theOther.myHDims[0];
      myHDims[1] = theOther.myHDims[1];
      myHDims[2] = theOther.myHDims[2];
      myIsAABox  = theOther.myIsAABox;
    }
    else
    {
      gp_Pnt aList[16];
      GetVertex(&aList[0]);
      theOther.GetVertex(&aList[8]);
      ReBuild(NCollection_Array1<gp_Pnt>(aList[0], 0, 15));
    }
  }
}

//=================================================================================================

void Bnd_OBB::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, Bnd_OBB)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCenter)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myAxes[0])
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myAxes[1])
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myAxes[2])

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHDims[0])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHDims[1])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHDims[2])
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsAABox)
}
