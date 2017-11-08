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

#include <Bnd_B3d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>

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
          const TColStd_Array1OfReal *theLT = 0);

  //! DiTO algorithm for OBB construction
  //! (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf)
  void ProcessDiTetrahedron();

  //! Creates OBB with already computed parameters
  void BuildBox(Bnd_OBB& theBox);

protected:
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
  else if(theNewParam > thePrmArr[1])
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
            const TColStd_Array1OfReal *theLT) :myPntsList(theL),
                                                myListOfTolers(theLT),
                                                myQualityCriterion(RealLast())
{
  const Standard_Real aSqrt3 = Sqrt(3);
  // Origin of all initial axis is (0,0,0).
  // All axes must be normalized.
  const gp_XYZ anInitialAxesArray[myNbInitAxes] = {gp_XYZ(1.0, 0.0, 0.0),
                                                   gp_XYZ(0.0, 1.0, 0.0),
                                                   gp_XYZ(0.0, 0.0, 1.0),
                                                   gp_XYZ(1.0, 1.0, 1.0) / aSqrt3,
                                                   gp_XYZ(1.0, 1.0, -1.0) / aSqrt3,
                                                   gp_XYZ(1.0, -1.0, 1.0) / aSqrt3,
                                                   gp_XYZ(1.0, -1.0, -1.0) / aSqrt3};

  // Minimal and maximal point on every axis
  const Standard_Integer aNbPoints = 2 * myNbInitAxes;

  for(Standard_Integer i = 0; i < 5; i++)
  {
    myTriIdx[i] = INT_MAX;
  }
  
  // Min and Max parameter
  Standard_Real aParams[aNbPoints];
  for(Standard_Integer i = 0; i < aNbPoints; i += 2)
  {
    aParams[i] = RealLast();
    aParams[i + 1] = RealFirst();
  }

  // Look for the extremal points (myLExtremalPoints)
  for(Standard_Integer i = myPntsList.Lower() ; i <= myPntsList.Upper(); i++)
  {
    const gp_XYZ &aCurrPoint = myPntsList(i).XYZ();
    for(Standard_Integer anAxeInd = 0, aPrmInd = 0; anAxeInd < myNbInitAxes; anAxeInd++, aPrmInd++)
    {
      const Standard_Real aParam = aCurrPoint.Dot(anInitialAxesArray[anAxeInd]);
      if(aParam < aParams[aPrmInd])
      {
        myLExtremalPoints[aPrmInd] = aCurrPoint;
        aParams[aPrmInd] = aParam;
      }
      aPrmInd++;

      if(aParam > aParams[aPrmInd])
      {
        myLExtremalPoints[aPrmInd] = aCurrPoint;
        aParams[aPrmInd] = aParam;
      }
    }
  }

  // Compute myTriIdx[0] and myTriIdx[1].

  Standard_Real aMaxSqDist = -1.0;
  for(Standard_Integer aPrmInd = 0; aPrmInd < aNbPoints; aPrmInd += 2)
  {
    const gp_Pnt &aP1 = myLExtremalPoints[aPrmInd],
                 &aP2 = myLExtremalPoints[aPrmInd + 1];
    const Standard_Real aSqDist = aP1.SquareDistance(aP2);
    if(aSqDist > aMaxSqDist)
    {
      aMaxSqDist = aSqDist;
      myTriIdx[0] = aPrmInd;
      myTriIdx[1] = aPrmInd + 1;
    }
  }

  FillToTriangle3();
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

  for(Standard_Integer aPtIdx = 0; aPtIdx < myNbExtremalPoints; aPtIdx++)
  {
    if((aPtIdx == myTriIdx[0]) || (aPtIdx == myTriIdx[1]) || (aPtIdx == myTriIdx[2]))
      continue;

    const gp_XYZ &aCurrPoint = myLExtremalPoints[aPtIdx];
    const Standard_Real aParam = theNormal.Dot(aCurrPoint - theBarryCenter);

    if(aParam < aParams[0])
    {
      myTriIdx[3] = aPtIdx;
      aParams[0] = aParam;
    }
    else if(aParam > aParams[1])
    {
      myTriIdx[4] = aPtIdx;
      aParams[1] = aParam;
    }
  }

  // The points must be in the different sides of the triangle plane.
  if(aParams[0] > -Precision::Confusion())
  {
    myTriIdx[3] = INT_MAX;
  }

  if(aParams[1] < Precision::Confusion())
  {
    myTriIdx[4] = INT_MAX;
  }
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

  //Some vertex of the triangle
  const gp_XYZ aP0 = myLExtremalPoints[theIdx1];

  // All axes must be normalized in order to provide correct area computation
  // (see ComputeQuality(...) method).
  gp_XYZ aYAxis[aNbAxes] = {(myLExtremalPoints[theIdx2] - myLExtremalPoints[theIdx1]),
                            (myLExtremalPoints[theIdx3] - myLExtremalPoints[theIdx2]),
                            (myLExtremalPoints[theIdx1] - myLExtremalPoints[theIdx3])};

  // Normal to the triangle plane
  gp_XYZ aZAxis = aYAxis[0].Crossed(aYAxis[1]);

  Standard_Real aSqMod = aZAxis.SquareModulus();

  if(aSqMod < Precision::SquareConfusion())
    return;

  aZAxis /= Sqrt(aSqMod);

  gp_XYZ aXAxis[aNbAxes];
  for(Standard_Integer i = 0; i < aNbAxes; i++)
  {
    aXAxis[i] = aYAxis[i].Crossed(aZAxis).Normalized();
    aYAxis[i].Normalize();
  }

  if(theIsBuiltTrg)
    FillToTriangle5(aZAxis, aP0);

  // Min and Max parameter
  const Standard_Integer aNbPoints = 2 * aNbAxes;
  
  Standard_Integer aMinIdx = -1;
  for(Standard_Integer anAxeInd = 0; anAxeInd < aNbAxes; anAxeInd++)
  {
    const gp_XYZ &aAX = aXAxis[anAxeInd],
                 &aAY = aYAxis[anAxeInd];

    Standard_Real aParams[aNbPoints] = {0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0};

    for(Standard_Integer aPtIdx = 0; aPtIdx < myNbExtremalPoints; aPtIdx++)
    {
      if(aPtIdx == theIdx1)
        continue;

      const gp_XYZ aCurrPoint = myLExtremalPoints[aPtIdx] - aP0;
      SetMinMax(&aParams[0], aAX.Dot(aCurrPoint));
      SetMinMax(&aParams[2], aAY.Dot(aCurrPoint));
      SetMinMax(&aParams[4], aZAxis.Dot(aCurrPoint));
    }

    const Standard_Real anArea = ComputeQuality(aParams);
    if(anArea < myQualityCriterion)
    {
      myQualityCriterion = anArea;
      aMinIdx = anAxeInd;
    }
  }

  if(aMinIdx < 0)
    return;

  myAxes[0] = aXAxis[aMinIdx];
  myAxes[1] = aYAxis[aMinIdx];
  myAxes[2] = aZAxis;
}
//=======================================================================
// Function : ProcessDiTetrahedron
// purpose : DiTo-algorithm (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf)
//=======================================================================
void OBBTool::ProcessDiTetrahedron()
{
  ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[2], Standard_True);

  if(myTriIdx[3] <= myNbExtremalPoints)
  {
    ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[3], Standard_False);
    ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[3], Standard_False);
    ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[3], Standard_False);
  }

  if(myTriIdx[4] <= myNbExtremalPoints)
  {
    ProcessTriangle(myTriIdx[0], myTriIdx[1], myTriIdx[4], Standard_False);
    ProcessTriangle(myTriIdx[1], myTriIdx[2], myTriIdx[4], Standard_False);
    ProcessTriangle(myTriIdx[0], myTriIdx[2], myTriIdx[4], Standard_False);
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
                      const TColStd_Array1OfReal *theListOfTolerances)
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

  OBBTool aTool(theListOfPoints, theListOfTolerances);
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
  gp_Pnt aList[9];
  GetVertex(aList);
  aList[8] = theP;

  ReBuild(TColgp_Array1OfPnt(aList[0], 0, 8));
}

// =======================================================================
// function : Add
// purpose  : 
// =======================================================================
void Bnd_OBB::Add(const Bnd_OBB& theOther)
{
  gp_Pnt aList[16];
  GetVertex(&aList[0]);
  theOther.GetVertex(&aList[8]);
  ReBuild(TColgp_Array1OfPnt(aList[0], 0, 15));
}

