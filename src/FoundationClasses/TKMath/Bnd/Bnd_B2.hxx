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

#ifndef _Bnd_B2_HeaderFile
#define _Bnd_B2_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Ax2d.hxx>

#include <array>

//! Template class for 2D bounding box.
//! This is a base template that is instantiated for Standard_Real and Standard_ShortReal.
template <typename RealType>
class Bnd_B2
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  constexpr Bnd_B2() noexcept;

  //! Constructor.
  constexpr Bnd_B2(const gp_XY& theCenter, const gp_XY& theHSize) noexcept;

  //! Constructor.
  constexpr Bnd_B2(const std::array<RealType, 2>& theCenter,
                   const std::array<RealType, 2>& theHSize) noexcept;

  //! Returns True if the box is void (non-initialized).
  constexpr Standard_Boolean IsVoid() const noexcept;

  //! Reset the box data.
  void Clear() noexcept;

  //! Update the box by a point.
  void Add(const gp_XY& thePnt);

  //! Update the box by a point.
  void Add(const gp_Pnt2d& thePnt);

  //! Update the box by another box.
  void Add(const Bnd_B2<RealType>& theBox);

  //! Query a box corner: (Center - HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XY CornerMin() const noexcept;

  //! Query a box corner: (Center + HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
  gp_XY CornerMax() const noexcept;

  //! Query the square diagonal. If the box is VOID (see method IsVoid())
  //! then a very big real value is returned.
  constexpr Standard_Real SquareExtent() const noexcept;

  //! Extend the Box by the absolute value of theDiff.
  void Enlarge(const Standard_Real theDiff) noexcept;

  //! Limit the Box by the internals of theOtherBox.
  //! Returns True if the limitation takes place, otherwise False
  //! indicating that the boxes do not intersect.
  Standard_Boolean Limit(const Bnd_B2<RealType>& theOtherBox);

  //! Transform the bounding box with the given transformation.
  //! The resulting box will be larger if theTrsf contains rotation.
  Standard_NODISCARD Bnd_B2<RealType> Transformed(const gp_Trsf2d& theTrsf) const;

  //! Check the given point for the inclusion in the Box.
  //! Returns True if the point is outside.
  constexpr Standard_Boolean IsOut(const gp_XY& thePnt) const noexcept;

  //! Check a circle for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_Boolean IsOut(const gp_XY&           theCenter,
                         const Standard_Real    theRadius,
                         const Standard_Boolean isCircleHollow = Standard_False) const;

  //! Check the given box for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  constexpr Standard_Boolean IsOut(const Bnd_B2<RealType>& theOtherBox) const noexcept;

  //! Check the given box oriented by the given transformation
  //! for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_Boolean IsOut(const Bnd_B2<RealType>& theOtherBox, const gp_Trsf2d& theTrsf) const;

  //! Check the given Line for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_Boolean IsOut(const gp_Ax2d& theLine) const;

  //! Check the Segment defined by the couple of input points
  //! for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_Boolean IsOut(const gp_XY& theP0, const gp_XY& theP1) const;

  //! Check that the box 'this' is inside the given box 'theBox'. Returns
  //! True if 'this' box is fully inside 'theBox'.
  constexpr Standard_Boolean IsIn(const Bnd_B2<RealType>& theBox) const noexcept;

  //! Check that the box 'this' is inside the given box 'theBox'
  //! transformed by 'theTrsf'. Returns True if 'this' box is fully
  //! inside the transformed 'theBox'.
  Standard_Boolean IsIn(const Bnd_B2<RealType>& theBox, const gp_Trsf2d& theTrsf) const;

  //! Set the Center coordinates
  void SetCenter(const gp_XY& theCenter) noexcept;

  //! Set the Center coordinates
  void SetCenter(const std::array<RealType, 2>& theCenter) noexcept;

  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
  void SetHSize(const gp_XY& theHSize) noexcept;

  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
  void SetHSize(const std::array<RealType, 2>& theHSize) noexcept;

  //! Get the Center coordinates
  constexpr const std::array<RealType, 2>& Center() const noexcept;

  //! Get the HSize (half-diagonal) coordinates
  constexpr const std::array<RealType, 2>& HSize() const noexcept;

protected:
  static constexpr Standard_Boolean compareDist(const RealType aHSize[2],
                                                const RealType aDist[2]) noexcept
  {
    return (std::abs(aDist[0]) > aHSize[0] || std::abs(aDist[1]) > aHSize[1]);
  }

  static Standard_Boolean compareDistD(const gp_XY& aHSize, const gp_XY& aDist) noexcept
  {
    return (std::abs(aDist.X()) > aHSize.X() || std::abs(aDist.Y()) > aHSize.Y());
  }

  //! Constant representing a very large value for void box initialization
  static constexpr RealType THE_RealLast = RealType(1e30);

private:
  std::array<RealType, 2> myCenter;
  std::array<RealType, 2> myHSize;
};

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B2<RealType>::Bnd_B2() noexcept
    : myCenter{THE_RealLast, THE_RealLast},
      myHSize{-THE_RealLast, -THE_RealLast}
{
}

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B2<RealType>::Bnd_B2(const gp_XY& theCenter, const gp_XY& theHSize) noexcept
    : myCenter{RealType(theCenter.X()), RealType(theCenter.Y())},
      myHSize{RealType(theHSize.X()), RealType(theHSize.Y())}
{
}

//=================================================================================================

template <typename RealType>
constexpr inline Bnd_B2<RealType>::Bnd_B2(const std::array<RealType, 2>& theCenter,
                                          const std::array<RealType, 2>& theHSize) noexcept
    : myCenter(theCenter),
      myHSize(theHSize)
{
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::Clear() noexcept
{
  myCenter[0] = THE_RealLast;
  myCenter[1] = THE_RealLast;
  myHSize[0]  = -THE_RealLast;
  myHSize[1]  = -THE_RealLast;
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B2<RealType>::IsVoid() const noexcept
{
  return (myHSize[0] < -1e-5);
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::Add(const gp_Pnt2d& thePnt)
{
  Add(thePnt.XY());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::Add(const Bnd_B2<RealType>& theBox)
{
  if (!theBox.IsVoid())
  {
    Add(theBox.CornerMin());
    Add(theBox.CornerMax());
  }
}

//=================================================================================================

template <typename RealType>
inline gp_XY Bnd_B2<RealType>::CornerMin() const noexcept
{
  return gp_XY(myCenter[0] - myHSize[0], myCenter[1] - myHSize[1]);
}

//=================================================================================================

template <typename RealType>
inline gp_XY Bnd_B2<RealType>::CornerMax() const noexcept
{
  return gp_XY(myCenter[0] + myHSize[0], myCenter[1] + myHSize[1]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Real Bnd_B2<RealType>::SquareExtent() const noexcept
{
  return 4 * (myHSize[0] * myHSize[0] + myHSize[1] * myHSize[1]);
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::SetCenter(const gp_XY& theCenter) noexcept
{
  myCenter[0] = RealType(theCenter.X());
  myCenter[1] = RealType(theCenter.Y());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::SetHSize(const gp_XY& theHSize) noexcept
{
  myHSize[0] = RealType(theHSize.X());
  myHSize[1] = RealType(theHSize.Y());
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::SetCenter(const std::array<RealType, 2>& theCenter) noexcept
{
  myCenter = theCenter;
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::SetHSize(const std::array<RealType, 2>& theHSize) noexcept
{
  myHSize = theHSize;
}

//=================================================================================================

template <typename RealType>
constexpr inline const std::array<RealType, 2>& Bnd_B2<RealType>::Center() const noexcept
{
  return myCenter;
}

//=================================================================================================

template <typename RealType>
constexpr inline const std::array<RealType, 2>& Bnd_B2<RealType>::HSize() const noexcept
{
  return myHSize;
}

//=================================================================================================

template <typename RealType>
inline void Bnd_B2<RealType>::Enlarge(const Standard_Real aDiff) noexcept
{
  const RealType aD = RealType(std::abs(aDiff));
  myHSize[0] += aD;
  myHSize[1] += aD;
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B2<RealType>::IsOut(const gp_XY& thePnt) const noexcept
{
  return (std::abs(RealType(thePnt.X()) - myCenter[0]) > myHSize[0]
          || std::abs(RealType(thePnt.Y()) - myCenter[1]) > myHSize[1]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B2<RealType>::IsOut(
  const Bnd_B2<RealType>& theBox) const noexcept
{
  return (std::abs(theBox.myCenter[0] - myCenter[0]) > theBox.myHSize[0] + myHSize[0]
          || std::abs(theBox.myCenter[1] - myCenter[1]) > theBox.myHSize[1] + myHSize[1]);
}

//=================================================================================================

template <typename RealType>
constexpr inline Standard_Boolean Bnd_B2<RealType>::IsIn(
  const Bnd_B2<RealType>& theBox) const noexcept
{
  return (std::abs(theBox.myCenter[0] - myCenter[0]) < theBox.myHSize[0] - myHSize[0]
          && std::abs(theBox.myCenter[1] - myCenter[1]) < theBox.myHSize[1] - myHSize[1]);
}

//=================================================================================================

template <typename RealType>
void Bnd_B2<RealType>::Add(const gp_XY& thePnt)
{
  if (IsVoid())
  {
    myCenter[0] = RealType(thePnt.X());
    myCenter[1] = RealType(thePnt.Y());
    myHSize[0]  = 0.;
    myHSize[1]  = 0.;
  }
  else
  {
    const RealType aDiff[2] = {RealType(thePnt.X()) - myCenter[0],
                               RealType(thePnt.Y()) - myCenter[1]};
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
  }
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::Limit(const Bnd_B2<RealType>& theBox)
{
  Standard_Boolean aResult(Standard_False);
  const RealType   diffC[2] = {theBox.myCenter[0] - myCenter[0], theBox.myCenter[1] - myCenter[1]};
  const RealType   sumH[2]  = {theBox.myHSize[0] + myHSize[0], theBox.myHSize[1] + myHSize[1]};
  // check the condition IsOut
  if (!compareDist(sumH, diffC))
  {
    const RealType diffH[2] = {theBox.myHSize[0] - myHSize[0], theBox.myHSize[1] - myHSize[1]};
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
    aResult = Standard_True;
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Bnd_B2<RealType> Bnd_B2<RealType>::Transformed(const gp_Trsf2d& theTrsf) const
{
  Bnd_B2<RealType>    aResult;
  const gp_TrsfForm   aForm     = theTrsf.Form();
  const Standard_Real aScale    = theTrsf.ScaleFactor();
  const Standard_Real aScaleAbs = std::abs(aScale);
  if (aForm == gp_Identity)
    aResult = *this;
  else if (aForm == gp_Translation || aForm == gp_PntMirror || aForm == gp_Scale)
  {
    aResult.myCenter[0] = (RealType)(myCenter[0] * aScale + theTrsf.TranslationPart().X());
    aResult.myCenter[1] = (RealType)(myCenter[1] * aScale + theTrsf.TranslationPart().Y());
    aResult.myHSize[0]  = (RealType)(myHSize[0] * aScaleAbs);
    aResult.myHSize[1]  = (RealType)(myHSize[1] * aScaleAbs);
  }
  else
  {
    gp_XY aCenter((Standard_Real)myCenter[0], (Standard_Real)myCenter[1]);
    theTrsf.Transforms(aCenter);
    aResult.myCenter[0] = (RealType)aCenter.X();
    aResult.myCenter[1] = (RealType)aCenter.Y();

    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);
    aResult.myHSize[0] =
      (RealType)(aScaleAbs * (std::abs(aMat[0]) * myHSize[0] + std::abs(aMat[1]) * myHSize[1]));
    aResult.myHSize[1] =
      (RealType)(aScaleAbs * (std::abs(aMat[2]) * myHSize[0] + std::abs(aMat[3]) * myHSize[1]));
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::IsOut(const gp_XY&           theCenter,
                                         const Standard_Real    theRadius,
                                         const Standard_Boolean isCircleHollow) const
{
  Standard_Boolean aResult(Standard_True);
  if (!isCircleHollow)
  {
    // vector from the center of the circle to the nearest box face
    const Standard_Real aDist[2] = {
      std::abs(theCenter.X() - Standard_Real(myCenter[0])) - Standard_Real(myHSize[0]),
      std::abs(theCenter.Y() - Standard_Real(myCenter[1])) - Standard_Real(myHSize[1])};
    Standard_Real aD(0.);
    if (aDist[0] > 0.)
      aD = aDist[0] * aDist[0];
    if (aDist[1] > 0.)
      aD += aDist[1] * aDist[1];
    aResult = (aD > theRadius * theRadius);
  }
  else
  {
    const Standard_Real aDistC[2] = {std::abs(theCenter.X() - Standard_Real(myCenter[0])),
                                     std::abs(theCenter.Y() - Standard_Real(myCenter[1]))};
    // vector from the center of the circle to the nearest box face
    Standard_Real aDist[2] = {aDistC[0] - Standard_Real(myHSize[0]),
                              aDistC[1] - Standard_Real(myHSize[1])};
    Standard_Real aD(0.);
    if (aDist[0] > 0.)
      aD = aDist[0] * aDist[0];
    if (aDist[1] > 0.)
      aD += aDist[1] * aDist[1];
    if (aD < theRadius * theRadius)
    {
      // the box intersects the solid circle; check if it is completely
      // inside the circle (in such case return isOut==True)
      aDist[0] = aDistC[0] + Standard_Real(myHSize[0]);
      aDist[1] = aDistC[1] + Standard_Real(myHSize[1]);
      if (aDist[0] * aDist[0] + aDist[1] * aDist[1] > theRadius * theRadius)
        aResult = Standard_False;
    }
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::IsOut(const Bnd_B2<RealType>& theBox,
                                         const gp_Trsf2d&        theTrsf) const
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
            > RealType(theBox.myHSize[1] * aScaleAbs) + myHSize[1]);
  }
  else
  {
    // theBox is transformed and we check the resulting (enlarged) box against
    // 'this' box.
    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);

    gp_XY aCenter((Standard_Real)theBox.myCenter[0], (Standard_Real)theBox.myCenter[1]);
    theTrsf.Transforms(aCenter);
    const Standard_Real aDist[2]   = {aCenter.X() - (Standard_Real)myCenter[0],
                                      aCenter.Y() - (Standard_Real)myCenter[1]};
    const Standard_Real aMatAbs[4] = {std::abs(aMat[0]),
                                      std::abs(aMat[1]),
                                      std::abs(aMat[2]),
                                      std::abs(aMat[3])};
    if (std::abs(aDist[0])
          > (aScaleAbs * (aMatAbs[0] * theBox.myHSize[0] + aMatAbs[1] * theBox.myHSize[1])
             + (Standard_Real)myHSize[0])
        || std::abs(aDist[1])
             > (aScaleAbs * (aMatAbs[2] * theBox.myHSize[0] + aMatAbs[3] * theBox.myHSize[1])
                + (Standard_Real)myHSize[1]))
      aResult = Standard_True;

    else
    {
      // theBox is rotated, scaled and translated. We apply the reverse
      // translation and scaling then check against the rotated box 'this'
      if ((std::abs(aMat[0] * aDist[0] + aMat[2] * aDist[1])
           > theBox.myHSize[0] * aScaleAbs + (aMatAbs[0] * myHSize[0] + aMatAbs[2] * myHSize[1]))
          || (std::abs(aMat[1] * aDist[0] + aMat[3] * aDist[1])
              > theBox.myHSize[1] * aScaleAbs
                  + (aMatAbs[1] * myHSize[0] + aMatAbs[3] * myHSize[1])))
        aResult = Standard_True;
    }
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::IsOut(const gp_Ax2d& theLine) const
{
  if (IsVoid())
    return Standard_True;
  // Intersect the line containing the segment.
  const Standard_Real aProd[3] = {
    theLine.Direction().XY()
      ^ (gp_XY(myCenter[0] - theLine.Location().X(), myCenter[1] - theLine.Location().Y())),
    theLine.Direction().X() * Standard_Real(myHSize[1]),
    theLine.Direction().Y() * Standard_Real(myHSize[0])};
  return (std::abs(aProd[0]) > (std::abs(aProd[1]) + std::abs(aProd[2])));
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::IsOut(const gp_XY& theP0, const gp_XY& theP1) const
{
  Standard_Boolean aResult(Standard_True);
  if (!IsVoid())
  {
    // Intersect the line containing the segment.
    const gp_XY         aSegDelta(theP1 - theP0);
    const Standard_Real aProd[3] = {aSegDelta ^ (gp_XY(myCenter[0], myCenter[1]) - theP0),
                                    aSegDelta.X() * Standard_Real(myHSize[1]),
                                    aSegDelta.Y() * Standard_Real(myHSize[0])};
    if (std::abs(aProd[0]) < (std::abs(aProd[1]) + std::abs(aProd[2])))
    {
      // Intersection with line detected; check the segment as bounding box
      const gp_XY aHSeg(0.5 * aSegDelta.X(), 0.5 * aSegDelta.Y());
      const gp_XY aHSegAbs(std::abs(aHSeg.X()), std::abs(aHSeg.Y()));
      aResult =
        compareDistD(gp_XY((Standard_Real)myHSize[0], (Standard_Real)myHSize[1]) + aHSegAbs,
                     theP0 + aHSeg - gp_XY((Standard_Real)myCenter[0], (Standard_Real)myCenter[1]));
    }
  }
  return aResult;
}

//=================================================================================================

template <typename RealType>
Standard_Boolean Bnd_B2<RealType>::IsIn(const Bnd_B2<RealType>& theBox,
                                        const gp_Trsf2d&        theTrsf) const
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
            < RealType(theBox.myHSize[1] * aScaleAbs) - myHSize[1]);
  }
  else
  {
    // theBox is rotated, scaled and translated. We apply the reverse
    // translation and scaling then check against the rotated box 'this'
    const Standard_Real* aMat = &theTrsf.HVectorialPart().Value(1, 1);
    gp_XY aCenter((Standard_Real)theBox.myCenter[0], (Standard_Real)theBox.myCenter[1]);
    theTrsf.Transforms(aCenter);
    const Standard_Real aDist[2] = {aCenter.X() - (Standard_Real)myCenter[0],
                                    aCenter.Y() - (Standard_Real)myCenter[1]};
    if ((std::abs(aMat[0] * aDist[0] + aMat[2] * aDist[1])
         < theBox.myHSize[0] * aScaleAbs
             - (std::abs(aMat[0]) * myHSize[0] + std::abs(aMat[2]) * myHSize[1]))
        && (std::abs(aMat[1] * aDist[0] + aMat[3] * aDist[1])
            < theBox.myHSize[1] * aScaleAbs
                - (std::abs(aMat[1]) * myHSize[0] + std::abs(aMat[3]) * myHSize[1])))
      aResult = Standard_True;
  }
  return aResult;
}

//=================================================================================================

//! 2D bounding box with double precision
using Bnd_B2d = Bnd_B2<Standard_Real>;

//! 2D bounding box with single precision
using Bnd_B2f = Bnd_B2<Standard_ShortReal>;

#endif // _Bnd_B2_HeaderFile
