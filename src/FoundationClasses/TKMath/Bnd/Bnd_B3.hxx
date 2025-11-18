// Created on: 2005-09-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Bnd_B3_HeaderFile
#define _Bnd_B3_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <gp.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>

#include <array>

//! Template class for 3D bounding box.
//! This is a base template that is instantiated for Standard_Real and Standard_ShortReal.
template <typename RealType>
class Bnd_B3
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  constexpr Bnd_B3() noexcept;

  //! Constructor.
  constexpr Bnd_B3(const gp_XYZ& theCenter, const gp_XYZ& theHSize) noexcept;

  //! Constructor.
  constexpr Bnd_B3(const std::array<RealType, 3>& theCenter,
                   const std::array<RealType, 3>& theHSize) noexcept;

  //! Returns True if the box is void (non-initialized).
  constexpr Standard_Boolean IsVoid() const noexcept;

  //! Reset the box data.
  void Clear() noexcept;

  //! Update the box by a point.
  void Add(const gp_XYZ& thePnt);

  //! Update the box by a point.
  void Add(const gp_Pnt& thePnt);

  //! Update the box by another box.
  void Add(const Bnd_B3<RealType>& theBox);

  //! Query the lower corner: (Center - HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XYZ CornerMin() const noexcept;

  //! Query the upper corner: (Center + HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XYZ CornerMax() const noexcept;

  //! Query the square diagonal. If the box is VOID (see method IsVoid())
  //! then a very big real value is returned.
  constexpr Standard_Real SquareExtent() const noexcept;

  //! Extend the Box by the absolute value of theDiff.
  void Enlarge(const Standard_Real theDiff) noexcept;

  //! Limit the Box by the internals of theOtherBox.
  //! Returns True if the limitation takes place, otherwise False
  //! indicating that the boxes do not intersect.
  Standard_Boolean Limit(const Bnd_B3<RealType>& theOtherBox);

  //! Transform the bounding box with the given transformation.
  //! The resulting box will be larger if theTrsf contains rotation.
  Standard_NODISCARD Bnd_B3<RealType> Transformed(const gp_Trsf& theTrsf) const;

  //! Check the given point for the inclusion in the Box.
  //! Returns True if the point is outside.
  constexpr Standard_Boolean IsOut(const gp_XYZ& thePnt) const noexcept;

  //! Check a sphere for the intersection with the current box.
  //! Returns True if there is no intersection between boxes. If the
  //! parameter 'IsSphereHollow' is True, then the intersection is not
  //! reported for a box that is completely inside the sphere (otherwise
  //! this method would report an intersection).
  Standard_Boolean IsOut(const gp_XYZ&          theCenter,
                         const Standard_Real    theRadius,
                         const Standard_Boolean isSphereHollow = Standard_False) const;

  //! Check the given box for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  constexpr Standard_Boolean IsOut(const Bnd_B3<RealType>& theOtherBox) const noexcept;

  //! Check the given box oriented by the given transformation
  //! for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_Boolean IsOut(const Bnd_B3<RealType>& theOtherBox, const gp_Trsf& theTrsf) const;

  //! Check the given Line for the intersection with the current box.
  //! Returns True if there is no intersection.
  //! isRay==True means intersection check with the positive half-line
  //! theOverthickness is the addition to the size of the current box
  //! (may be negative). If positive, it can be treated as the thickness
  //! of the line 'theLine' or the radius of the cylinder along 'theLine'
  Standard_Boolean IsOut(const gp_Ax1&          theLine,
                         const Standard_Boolean isRay            = Standard_False,
                         const Standard_Real    theOverthickness = 0.0) const;

  //! Check the given Plane for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_Boolean IsOut(const gp_Ax3& thePlane) const;

  //! Check that the box 'this' is inside the given box 'theBox'. Returns
  //! True if 'this' box is fully inside 'theBox'.
  constexpr Standard_Boolean IsIn(const Bnd_B3<RealType>& theBox) const noexcept;

  //! Check that the box 'this' is inside the given box 'theBox'
  //! transformed by 'theTrsf'. Returns True if 'this' box is fully
  //! inside the transformed 'theBox'.
  Standard_Boolean IsIn(const Bnd_B3<RealType>& theBox, const gp_Trsf& theTrsf) const;

  //! Set the Center coordinates
  void SetCenter(const gp_XYZ& theCenter) noexcept;

  //! Set the Center coordinates
  void SetCenter(const std::array<RealType, 3>& theCenter) noexcept;

  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
  void SetHSize(const gp_XYZ& theHSize) noexcept;

  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
  void SetHSize(const std::array<RealType, 3>& theHSize) noexcept;

  //! Get the Center coordinates
  constexpr const std::array<RealType, 3>& Center() const noexcept;

  //! Get the HSize (half-diagonal) coordinates
  constexpr const std::array<RealType, 3>& HSize() const noexcept;

protected:
  static constexpr Standard_Boolean compareDist(const RealType aHSize[3],
                                                const RealType aDist[3]) noexcept
  {
    return (std::abs(aDist[0]) > aHSize[0] || std::abs(aDist[1]) > aHSize[1]
            || std::abs(aDist[2]) > aHSize[2]);
  }

  static Standard_Boolean compareDistD(const gp_XYZ& aHSize, const gp_XYZ& aDist) noexcept
  {
    return (std::abs(aDist.X()) > aHSize.X() || std::abs(aDist.Y()) > aHSize.Y()
            || std::abs(aDist.Z()) > aHSize.Z());
  }

  //! Constant representing a very large value for void box initialization
  static constexpr RealType THE_RealLast = RealType(1e30);

private:
  std::array<RealType, 3> myCenter;
  std::array<RealType, 3> myHSize;
};

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B3<RealType>::Bnd_B3() noexcept
    : myCenter{THE_RealLast, THE_RealLast, THE_RealLast},
      myHSize{-THE_RealLast, -THE_RealLast, -THE_RealLast}
{
}

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B3<RealType>::Bnd_B3(const gp_XYZ& theCenter, const gp_XYZ& theHSize) noexcept
    : myCenter{RealType(theCenter.X()), RealType(theCenter.Y()), RealType(theCenter.Z())},
      myHSize{RealType(theHSize.X()), RealType(theHSize.Y()), RealType(theHSize.Z())}
{
}

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B3<RealType>::Bnd_B3(const std::array<RealType, 3>& theCenter,
                                          const std::array<RealType, 3>& theHSize) noexcept
    : myCenter(theCenter),
      myHSize(theHSize)
{
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::Clear() noexcept
{
  myCenter[0] = THE_RealLast;
  myCenter[1] = THE_RealLast;
  myCenter[2] = THE_RealLast;
  myHSize[0]  = -THE_RealLast;
  myHSize[1]  = -THE_RealLast;
  myHSize[2]  = -THE_RealLast;
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B3<RealType>::IsVoid() const noexcept
{
  return (myHSize[0] < -1e-5);
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::Add(const gp_Pnt& thePnt)
{
  Add(thePnt.XYZ());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::Add(const Bnd_B3<RealType>& theBox)
{
  if (!theBox.IsVoid())
  {
    Add(theBox.CornerMin());
    Add(theBox.CornerMax());
  }
}

//=================================================================================================

template <typename RealType>
inline gp_XYZ Bnd_B3<RealType>::CornerMin() const noexcept
{
  return gp_XYZ(myCenter[0] - myHSize[0], myCenter[1] - myHSize[1], myCenter[2] - myHSize[2]);
}

//=================================================================================================

template <typename RealType>
inline gp_XYZ Bnd_B3<RealType>::CornerMax() const noexcept
{
  return gp_XYZ(myCenter[0] + myHSize[0], myCenter[1] + myHSize[1], myCenter[2] + myHSize[2]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Real Bnd_B3<RealType>::SquareExtent() const noexcept
{
  return 4 * (myHSize[0] * myHSize[0] + myHSize[1] * myHSize[1] + myHSize[2] * myHSize[2]);
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::SetCenter(const gp_XYZ& theCenter) noexcept
{
  myCenter[0] = RealType(theCenter.X());
  myCenter[1] = RealType(theCenter.Y());
  myCenter[2] = RealType(theCenter.Z());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::SetHSize(const gp_XYZ& theHSize) noexcept
{
  myHSize[0] = RealType(theHSize.X());
  myHSize[1] = RealType(theHSize.Y());
  myHSize[2] = RealType(theHSize.Z());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::SetCenter(const std::array<RealType, 3>& theCenter) noexcept
{
  myCenter = theCenter;
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::SetHSize(const std::array<RealType, 3>& theHSize) noexcept
{
  myHSize = theHSize;
}

//=================================================================================================

template <typename RealType>
constexpr inline const std::array<RealType, 3>& Bnd_B3<RealType>::Center() const noexcept
{
  return myCenter;
}

//=================================================================================================

template <typename RealType>
constexpr inline const std::array<RealType, 3>& Bnd_B3<RealType>::HSize() const noexcept
{
  return myHSize;
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B3<RealType>::Enlarge(const Standard_Real aDiff) noexcept
{
  const Standard_Real aD = std::abs(aDiff);
  myHSize[0] += RealType(aD);
  myHSize[1] += RealType(aD);
  myHSize[2] += RealType(aD);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B3<RealType>::IsOut(const gp_XYZ& thePnt) const noexcept
{
  return (std::abs(RealType(thePnt.X()) - myCenter[0]) > myHSize[0]
          || std::abs(RealType(thePnt.Y()) - myCenter[1]) > myHSize[1]
          || std::abs(RealType(thePnt.Z()) - myCenter[2]) > myHSize[2]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B3<RealType>::IsOut(
  const Bnd_B3<RealType>& theBox) const noexcept
{
  return (std::abs(theBox.myCenter[0] - myCenter[0]) > theBox.myHSize[0] + myHSize[0]
          || std::abs(theBox.myCenter[1] - myCenter[1]) > theBox.myHSize[1] + myHSize[1]
          || std::abs(theBox.myCenter[2] - myCenter[2]) > theBox.myHSize[2] + myHSize[2]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B3<RealType>::IsIn(
  const Bnd_B3<RealType>& theBox) const noexcept
{
  return (std::abs(theBox.myCenter[0] - myCenter[0]) < theBox.myHSize[0] - myHSize[0]
          && std::abs(theBox.myCenter[1] - myCenter[1]) < theBox.myHSize[1] - myHSize[1]
          && std::abs(theBox.myCenter[2] - myCenter[2]) < theBox.myHSize[2] - myHSize[2]);
}

//=================================================================================================

template <typename RealType>
void Bnd_B3<RealType>::Add(const gp_XYZ& thePnt)
{
  if (IsVoid())
  {
    myCenter[0] = RealType(thePnt.X());
    myCenter[1] = RealType(thePnt.Y());
    myCenter[2] = RealType(thePnt.Z());
    myHSize[0]  = 0.;
    myHSize[1]  = 0.;
    myHSize[2]  = 0.;
  }
  else
  {
    const RealType aDiff[3] = {RealType(thePnt.X()) - myCenter[0],
                               RealType(thePnt.Y()) - myCenter[1],
                               RealType(thePnt.Z()) - myCenter[2]};
    if (aDiff[0] > myHSize[0])
    {
      const RealType aShift = (aDiff[0] - myHSize[0]) / 2;
      myCenter[0] += aShift;
      myHSize[0] += aShift;
    }
    else if (aDiff[0] < -myHSize[0])
    {
      const RealType aShift = (aDiff[0] + myHSize[0]) / 2;
      myCenter[0] += aShift;
      myHSize[0] -= aShift;
    }
    if (aDiff[1] > myHSize[1])
    {
      const RealType aShift = (aDiff[1] - myHSize[1]) / 2;
      myCenter[1] += aShift;
      myHSize[1] += aShift;
    }
    else if (aDiff[1] < -myHSize[1])
    {
      const RealType aShift = (aDiff[1] + myHSize[1]) / 2;
      myCenter[1] += aShift;
      myHSize[1] -= aShift;
    }
    if (aDiff[2] > myHSize[2])
    {
      const RealType aShift = (aDiff[2] - myHSize[2]) / 2;
      myCenter[2] += aShift;
      myHSize[2] += aShift;
    }
    else if (aDiff[2] < -myHSize[2])
    {
      const RealType aShift = (aDiff[2] + myHSize[2]) / 2;
      myCenter[2] += aShift;
      myHSize[2] -= aShift;
    }
  }
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::Limit(const Bnd_B3<RealType>& theBox)
{
  Standard_Boolean aResult(Standard_False);
  const RealType   diffC[3] = {theBox.myCenter[0] - myCenter[0],
                               theBox.myCenter[1] - myCenter[1],
                               theBox.myCenter[2] - myCenter[2]};
  const RealType   sumH[3]  = {theBox.myHSize[0] + myHSize[0],
                               theBox.myHSize[1] + myHSize[1],
                               theBox.myHSize[2] + myHSize[2]};
  // check the condition IsOut
  if (!compareDist(sumH, diffC))
  {
    const RealType diffH[3] = {theBox.myHSize[0] - myHSize[0],
                               theBox.myHSize[1] - myHSize[1],
                               theBox.myHSize[2] - myHSize[2]};
    if (diffC[0] - diffH[0] > 0.)
    {
      const RealType aShift = (diffC[0] - diffH[0]) / 2; // positive
      myCenter[0] += aShift;
      myHSize[0] -= aShift;
    }
    else if (diffC[0] + diffH[0] < 0.)
    {
      const RealType aShift = (diffC[0] + diffH[0]) / 2; // negative
      myCenter[0] += aShift;
      myHSize[0] += aShift;
    }
    if (diffC[1] - diffH[1] > 0.)
    {
      const RealType aShift = (diffC[1] - diffH[1]) / 2; // positive
      myCenter[1] += aShift;
      myHSize[1] -= aShift;
    }
    else if (diffC[1] + diffH[1] < 0.)
    {
      const RealType aShift = (diffC[1] + diffH[1]) / 2; // negative
      myCenter[1] += aShift;
      myHSize[1] += aShift;
    }
    if (diffC[2] - diffH[2] > 0.)
    {
      const RealType aShift = (diffC[2] - diffH[2]) / 2; // positive
      myCenter[2] += aShift;
      myHSize[2] -= aShift;
    }
    else if (diffC[2] + diffH[2] < 0.)
    {
      const RealType aShift = (diffC[2] + diffH[2]) / 2; // negative
      myCenter[2] += aShift;
      myHSize[2] += aShift;
    }
    aResult = Standard_True;
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Bnd_B3<RealType> Bnd_B3<RealType>::Transformed(const gp_Trsf& theTrsf) const
{
  Bnd_B3<RealType>    aResult;
  const gp_TrsfForm   aForm     = theTrsf.Form();
  const Standard_Real aScale    = theTrsf.ScaleFactor();
  const Standard_Real aScaleAbs = std::abs(aScale);
  if (aForm == gp_Identity)
    aResult = *this;
  else if (aForm == gp_Translation || aForm == gp_PntMirror || aForm == gp_Scale)
  {
    aResult.myCenter[0] = (RealType)(myCenter[0] * aScale + theTrsf.TranslationPart().X());
    aResult.myCenter[1] = (RealType)(myCenter[1] * aScale + theTrsf.TranslationPart().Y());
    aResult.myCenter[2] = (RealType)(myCenter[2] * aScale + theTrsf.TranslationPart().Z());
    aResult.myHSize[0]  = (RealType)(myHSize[0] * aScaleAbs);
    aResult.myHSize[1]  = (RealType)(myHSize[1] * aScaleAbs);
    aResult.myHSize[2]  = (RealType)(myHSize[2] * aScaleAbs);
  }
  else
  {
    gp_XYZ aCenter((Standard_Real)myCenter[0],
                   (Standard_Real)myCenter[1],
                   (Standard_Real)myCenter[2]);
    theTrsf.Transforms(aCenter);
    aResult.myCenter[0] = (RealType)aCenter.X();
    aResult.myCenter[1] = (RealType)aCenter.Y();
    aResult.myCenter[2] = (RealType)aCenter.Z();

    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);
    aResult.myHSize[0] =
      (RealType)(aScaleAbs
                 * (std::abs(aMat[0]) * myHSize[0] + std::abs(aMat[1]) * myHSize[1]
                    + std::abs(aMat[2]) * myHSize[2]));
    aResult.myHSize[1] =
      (RealType)(aScaleAbs
                 * (std::abs(aMat[3]) * myHSize[0] + std::abs(aMat[4]) * myHSize[1]
                    + std::abs(aMat[5]) * myHSize[2]));
    aResult.myHSize[2] =
      (RealType)(aScaleAbs
                 * (std::abs(aMat[6]) * myHSize[0] + std::abs(aMat[7]) * myHSize[1]
                    + std::abs(aMat[8]) * myHSize[2]));
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::IsOut(const gp_XYZ&          theCenter,
                                         const Standard_Real    theRadius,
                                         const Standard_Boolean isSphereHollow) const
{
  Standard_Boolean aResult(Standard_True);
  if (!isSphereHollow)
  {
    // vector from the center of the sphere to the nearest box face
    const Standard_Real aDist[3] = {
      std::abs(theCenter.X() - Standard_Real(myCenter[0])) - Standard_Real(myHSize[0]),
      std::abs(theCenter.Y() - Standard_Real(myCenter[1])) - Standard_Real(myHSize[1]),
      std::abs(theCenter.Z() - Standard_Real(myCenter[2])) - Standard_Real(myHSize[2])};
    Standard_Real aD(0.);
    if (aDist[0] > 0.)
      aD = aDist[0] * aDist[0];
    if (aDist[1] > 0.)
      aD += aDist[1] * aDist[1];
    if (aDist[2] > 0.)
      aD += aDist[2] * aDist[2];
    aResult = (aD > theRadius * theRadius);
  }
  else
  {
    const Standard_Real aDistC[3] = {std::abs(theCenter.X() - Standard_Real(myCenter[0])),
                                     std::abs(theCenter.Y() - Standard_Real(myCenter[1])),
                                     std::abs(theCenter.Z() - Standard_Real(myCenter[2]))};
    // vector from the center of the sphere to the nearest box face
    Standard_Real aDist[3] = {aDistC[0] - Standard_Real(myHSize[0]),
                              aDistC[1] - Standard_Real(myHSize[1]),
                              aDistC[2] - Standard_Real(myHSize[2])};
    Standard_Real aD(0.);
    if (aDist[0] > 0.)
      aD = aDist[0] * aDist[0];
    if (aDist[1] > 0.)
      aD += aDist[1] * aDist[1];
    if (aDist[2] > 0.)
      aD += aDist[2] * aDist[2];
    if (aD < theRadius * theRadius)
    {
      // the box intersects the solid sphere; check if it is completely
      // inside the circle (in such case return isOut==True)
      aDist[0] = aDistC[0] + Standard_Real(myHSize[0]);
      aDist[1] = aDistC[1] + Standard_Real(myHSize[1]);
      aDist[2] = aDistC[2] + Standard_Real(myHSize[2]);
      if (aDist[0] * aDist[0] + aDist[1] * aDist[1] + aDist[2] * aDist[2] > theRadius * theRadius)
        aResult = Standard_False;
    }
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::IsOut(const Bnd_B3<RealType>& theBox,
                                         const gp_Trsf&          theTrsf) const
{
  Standard_Boolean    aResult(Standard_False);
  const gp_TrsfForm   aForm     = theTrsf.Form();
  const Standard_Real aScale    = theTrsf.ScaleFactor();
  const Standard_Real aScaleAbs = std::abs(aScale);
  if (aForm == gp_Translation || aForm == gp_Identity || aForm == gp_PntMirror || aForm == gp_Scale)
  {
    aResult =
      (std::abs(RealType(theBox.myCenter[0] * aScale + theTrsf.TranslationPart().X()) - myCenter[0])
         > RealType(theBox.myHSize[0] * aScaleAbs) + myHSize[0]
       || std::abs(RealType(theBox.myCenter[1] * aScale + theTrsf.TranslationPart().Y())
                   - myCenter[1])
            > RealType(theBox.myHSize[1] * aScaleAbs) + myHSize[1]
       || std::abs(RealType(theBox.myCenter[2] * aScale + theTrsf.TranslationPart().Z())
                   - myCenter[2])
            > RealType(theBox.myHSize[2] * aScaleAbs) + myHSize[2]);
  }
  else
  {
    // theBox is transformed and we check the resulting (enlarged) box against
    // 'this' box.
    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);

    gp_XYZ aCenter((Standard_Real)theBox.myCenter[0],
                   (Standard_Real)theBox.myCenter[1],
                   (Standard_Real)theBox.myCenter[2]);
    theTrsf.Transforms(aCenter);
    const Standard_Real aDist[3]   = {aCenter.X() - (Standard_Real)myCenter[0],
                                      aCenter.Y() - (Standard_Real)myCenter[1],
                                      aCenter.Z() - (Standard_Real)myCenter[2]};
    const Standard_Real aMatAbs[9] = {std::abs(aMat[0]),
                                      std::abs(aMat[1]),
                                      std::abs(aMat[2]),
                                      std::abs(aMat[3]),
                                      std::abs(aMat[4]),
                                      std::abs(aMat[5]),
                                      std::abs(aMat[6]),
                                      std::abs(aMat[7]),
                                      std::abs(aMat[8])};
    if (std::abs(aDist[0]) > (aScaleAbs
                                * (aMatAbs[0] * theBox.myHSize[0] + aMatAbs[1] * theBox.myHSize[1]
                                   + aMatAbs[2] * theBox.myHSize[2])
                              + (Standard_Real)myHSize[0])
        || std::abs(aDist[1])
             > (aScaleAbs
                  * (aMatAbs[3] * theBox.myHSize[0] + aMatAbs[4] * theBox.myHSize[1]
                     + aMatAbs[5] * theBox.myHSize[2])
                + (Standard_Real)myHSize[1])
        || std::abs(aDist[2])
             > (aScaleAbs
                  * (aMatAbs[6] * theBox.myHSize[0] + aMatAbs[7] * theBox.myHSize[1]
                     + aMatAbs[8] * theBox.myHSize[2])
                + (Standard_Real)myHSize[2]))
      aResult = Standard_True;

    else
    {
      // theBox is rotated, scaled and translated. We apply the reverse
      // translation and scaling then check against the rotated box 'this'
      if ((std::abs(aMat[0] * aDist[0] + aMat[3] * aDist[1] + aMat[6] * aDist[2])
           > theBox.myHSize[0] * aScaleAbs
               + (aMatAbs[0] * myHSize[0] + aMatAbs[3] * myHSize[1] + aMatAbs[6] * myHSize[2]))
          || (std::abs(aMat[1] * aDist[0] + aMat[4] * aDist[1] + aMat[7] * aDist[2])
              > theBox.myHSize[1] * aScaleAbs
                  + (aMatAbs[1] * myHSize[0] + aMatAbs[4] * myHSize[1] + aMatAbs[7] * myHSize[2]))
          || (std::abs(aMat[2] * aDist[0] + aMat[5] * aDist[1] + aMat[8] * aDist[2])
              > theBox.myHSize[2] * aScaleAbs
                  + (aMatAbs[2] * myHSize[0] + aMatAbs[5] * myHSize[1] + aMatAbs[8] * myHSize[2])))
        aResult = Standard_True;
    }
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::IsOut(const gp_Ax3& thePlane) const
{
  if (IsVoid())
    return Standard_True;
  const gp_XYZ&       anOrigin = thePlane.Location().XYZ();
  const gp_XYZ&       aDir     = thePlane.Direction().XYZ();
  const gp_XYZ        aBoxCenter((Standard_Real)myCenter[0],
                          (Standard_Real)myCenter[1],
                          (Standard_Real)myCenter[2]);
  const Standard_Real aDist0 = (aBoxCenter - anOrigin) * aDir;
  // Find the signed distances from two opposite corners of the box to the plane
  // If the distances are not the same sign, then the plane crosses the box
  const Standard_Real aDist1 = // proj of HSize on aDir
    Standard_Real(myHSize[0]) * std::abs(aDir.X()) + Standard_Real(myHSize[1]) * std::abs(aDir.Y())
    + Standard_Real(myHSize[2]) * std::abs(aDir.Z());
  return ((aDist0 + aDist1) * (aDist0 - aDist1) > 0.);
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::IsOut(const gp_Ax1&          theLine,
                                         const Standard_Boolean isRay,
                                         const Standard_Real    theOverthickness) const
{
  const Standard_Real aRes = gp::Resolution() * 100.;
  if (IsVoid())
    return Standard_True;
  Standard_Real anInter0[2] = {-RealLast(), RealLast()}, anInter1[2] = {-RealLast(), RealLast()};
  const gp_XYZ& aDir = theLine.Direction().XYZ();
  const gp_XYZ  aDiff((Standard_Real)myCenter[0] - theLine.Location().X(),
                     (Standard_Real)myCenter[1] - theLine.Location().Y(),
                     (Standard_Real)myCenter[2] - theLine.Location().Z());

  // Find the parameter interval in X dimension
  Standard_Real aHSize = (Standard_Real)myHSize[0] + theOverthickness;
  if (aDir.X() > aRes)
  {
    anInter0[0] = (aDiff.X() - aHSize) / aDir.X();
    anInter0[1] = (aDiff.X() + aHSize) / aDir.X();
  }
  else if (aDir.X() < -aRes)
  {
    anInter0[0] = (aDiff.X() + aHSize) / aDir.X();
    anInter0[1] = (aDiff.X() - aHSize) / aDir.X();
  }
  else
    // the line is orthogonal to OX axis. Test for inclusion in box limits
    if (std::abs(aDiff.X()) > aHSize)
      return Standard_True;

  // Find the parameter interval in Y dimension
  aHSize = (Standard_Real)myHSize[1] + theOverthickness;
  if (aDir.Y() > aRes)
  {
    anInter1[0] = (aDiff.Y() - aHSize) / aDir.Y();
    anInter1[1] = (aDiff.Y() + aHSize) / aDir.Y();
  }
  else if (aDir.Y() < -aRes)
  {
    anInter1[0] = (aDiff.Y() + aHSize) / aDir.Y();
    anInter1[1] = (aDiff.Y() - aHSize) / aDir.Y();
  }
  else
    // the line is orthogonal to OY axis. Test for inclusion in box limits
    if (std::abs(aDiff.Y()) > aHSize)
      return Standard_True;

  // Intersect Y-interval with X-interval
  if (anInter0[0] > (anInter1[1] + aRes) || anInter0[1] < (anInter1[0] - aRes))
    return Standard_True;
  if (anInter1[0] > anInter0[0])
    anInter0[0] = anInter1[0];
  if (anInter1[1] < anInter0[1])
    anInter0[1] = anInter1[1];
  if (isRay && anInter0[1] < -aRes)
    return Standard_True;

  // Find the parameter interval in Z dimension
  aHSize = (Standard_Real)myHSize[2] + theOverthickness;
  if (aDir.Z() > aRes)
  {
    anInter1[0] = (aDiff.Z() - aHSize) / aDir.Z();
    anInter1[1] = (aDiff.Z() + aHSize) / aDir.Z();
  }
  else if (aDir.Z() < -aRes)
  {
    anInter1[0] = (aDiff.Z() + aHSize) / aDir.Z();
    anInter1[1] = (aDiff.Z() - aHSize) / aDir.Z();
  }
  else
    // the line is orthogonal to OZ axis. Test for inclusion in box limits
    return (std::abs(aDiff.Z()) > aHSize);
  if (isRay && anInter1[1] < -aRes)
    return Standard_True;

  return (anInter0[0] > (anInter1[1] + aRes) || anInter0[1] < (anInter1[0] - aRes));
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B3<RealType>::IsIn(const Bnd_B3<RealType>& theBox,
                                        const gp_Trsf&          theTrsf) const
{
  Standard_Boolean    aResult(Standard_False);
  const gp_TrsfForm   aForm     = theTrsf.Form();
  const Standard_Real aScale    = theTrsf.ScaleFactor();
  const Standard_Real aScaleAbs = std::abs(aScale);
  if (aForm == gp_Translation || aForm == gp_Identity || aForm == gp_PntMirror || aForm == gp_Scale)
  {
    aResult =
      (std::abs(RealType(theBox.myCenter[0] * aScale + theTrsf.TranslationPart().X()) - myCenter[0])
         < RealType(theBox.myHSize[0] * aScaleAbs) - myHSize[0]
       && std::abs(RealType(theBox.myCenter[1] * aScale + theTrsf.TranslationPart().Y())
                   - myCenter[1])
            < RealType(theBox.myHSize[1] * aScaleAbs) - myHSize[1]
       && std::abs(RealType(theBox.myCenter[2] * aScale + theTrsf.TranslationPart().Z())
                   - myCenter[2])
            < RealType(theBox.myHSize[2] * aScaleAbs) - myHSize[2]);
  }
  else
  {
    // theBox is rotated, scaled and translated. We apply the reverse
    // translation and scaling then check against the rotated box 'this'
    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);
    gp_XYZ               aCenter((Standard_Real)theBox.myCenter[0],
                   (Standard_Real)theBox.myCenter[1],
                   (Standard_Real)theBox.myCenter[2]);
    theTrsf.Transforms(aCenter);
    const Standard_Real aDist[3] = {aCenter.X() - (Standard_Real)myCenter[0],
                                    aCenter.Y() - (Standard_Real)myCenter[1],
                                    aCenter.Z() - (Standard_Real)myCenter[2]};
    if ((std::abs(aMat[0] * aDist[0] + aMat[3] * aDist[1] + aMat[6] * aDist[2])
         < theBox.myHSize[0] * aScaleAbs
             - (std::abs(aMat[0]) * myHSize[0] + std::abs(aMat[3]) * myHSize[1]
                + std::abs(aMat[6]) * myHSize[2]))
        && (std::abs(aMat[1] * aDist[0] + aMat[4] * aDist[1] + aMat[7] * aDist[2])
            < theBox.myHSize[1] * aScaleAbs
                - (std::abs(aMat[1]) * myHSize[0] + std::abs(aMat[4]) * myHSize[1]
                   + std::abs(aMat[7]) * myHSize[2]))
        && (std::abs(aMat[2] * aDist[0] + aMat[5] * aDist[1] + aMat[8] * aDist[2])
            < theBox.myHSize[2] * aScaleAbs
                - (std::abs(aMat[2]) * myHSize[0] + std::abs(aMat[5]) * myHSize[1]
                   + std::abs(aMat[8]) * myHSize[2])))
      aResult = Standard_True;
  }
  return aResult;
}

//=================================================================================================

//! 3D bounding box with double precision
using Bnd_B3d = Bnd_B3<Standard_Real>;

//! 3D bounding box with single precision
using Bnd_B3f = Bnd_B3<Standard_ShortReal>;

#endif // _Bnd_B3_HeaderFile
