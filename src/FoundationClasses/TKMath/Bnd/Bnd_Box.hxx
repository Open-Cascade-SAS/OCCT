// Created on: 1991-01-28
// Created by: Remi Lequette
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Bnd_Box_HeaderFile
#define _Bnd_Box_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

#include <algorithm>
#include <cmath>

class gp_Pnt;
class gp_Dir;
class gp_Trsf;
class gp_Lin;
class gp_Pln;

//! Describes a bounding box in 3D space.
//! A bounding box is parallel to the axes of the coordinates
//! system. If it is finite, it is defined by the three intervals:
//! -   [ Xmin,Xmax ],
//! -   [ Ymin,Ymax ],
//! -   [ Zmin,Zmax ].
//! A bounding box may be infinite (i.e. open) in one or more
//! directions. It is said to be:
//! -   OpenXmin if it is infinite on the negative side of the   "X Direction";
//! -   OpenXmax if it is infinite on the positive side of the "X Direction";
//! -   OpenYmin if it is infinite on the negative side of the   "Y Direction";
//! -   OpenYmax if it is infinite on the positive side of the "Y Direction";
//! -   OpenZmin if it is infinite on the negative side of the   "Z Direction";
//! -   OpenZmax if it is infinite on the positive side of the "Z Direction";
//! -   WholeSpace if it is infinite in all six directions. In this
//! case, any point of the space is inside the box;
//! -   Void if it is empty. In this case, there is no point included in the box.
//! A bounding box is defined by:
//! -   six bounds (Xmin, Xmax, Ymin, Ymax, Zmin and
//! Zmax) which limit the bounding box if it is finite,
//! -   eight flags (OpenXmin, OpenXmax, OpenYmin,
//! OpenYmax, OpenZmin, OpenZmax,
//! WholeSpace and Void) which describe the
//! bounding box if it is infinite or empty, and
//! -   a gap, which is included on both sides in any direction
//! when consulting the finite bounds of the box.
class Bnd_Box
{
public:
  DEFINE_STANDARD_ALLOC

  //! Structure containing the box limits (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax).
  //! The values include the gap and account for open directions.
  struct Limits
  {
    double Xmin; //!< Minimum X coordinate
    double Xmax; //!< Maximum X coordinate
    double Ymin; //!< Minimum Y coordinate
    double Ymax; //!< Maximum Y coordinate
    double Zmin; //!< Minimum Z coordinate
    double Zmax; //!< Maximum Z coordinate
  };

  //! Creates an empty Box.
  //! The constructed box is qualified Void. Its gap is null.
  constexpr Bnd_Box()
      : Xmin(RealLast()),
        Xmax(-RealLast()),
        Ymin(RealLast()),
        Ymax(-RealLast()),
        Zmin(RealLast()),
        Zmax(-RealLast()),
        Gap(0.0),
        Flags(VoidMask)
  {
  }

  //! Creates a bounding box, it contains:
  //! -   minimum/maximum point of bounding box,
  //! The constructed box is qualified Void. Its gap is null.
  constexpr Bnd_Box(const gp_Pnt& theMin, const gp_Pnt& theMax)
      : Xmin(theMin.X()),
        Xmax(theMax.X()),
        Ymin(theMin.Y()),
        Ymax(theMax.Y()),
        Zmin(theMin.Z()),
        Zmax(theMax.Z()),
        Gap(0.0),
        Flags(0)
  {
  }

  //! Sets this bounding box so that it covers the whole of 3D space.
  //! It is infinitely long in all directions.
  void SetWhole() noexcept { Flags = WholeMask; }

  //! Sets this bounding box so that it is empty. All points are outside a void box.
  void SetVoid() noexcept
  {
    Xmin  = RealLast();
    Xmax  = -RealLast();
    Ymin  = RealLast();
    Ymax  = -RealLast();
    Zmin  = RealLast();
    Zmax  = -RealLast();
    Gap   = 0.0;
    Flags = VoidMask;
  }

  //! Sets this bounding box so that it bounds
  //! -   the point P. This involves first setting this bounding box
  //! to be void and then adding the point P.
  Standard_EXPORT void Set(const gp_Pnt& P);

  //! Sets this bounding box so that it bounds
  //! the half-line defined by point P and direction D, i.e. all
  //! points M defined by M=P+u*D, where u is greater than
  //! or equal to 0, are inside the bounding volume. This
  //! involves first setting this box to be void and then adding the half-line.
  Standard_EXPORT void Set(const gp_Pnt& P, const gp_Dir& D);

  //! Enlarges this bounding box, if required, so that it
  //! contains at least:
  //! -   interval [ aXmin,aXmax ] in the "X Direction",
  //! -   interval [ aYmin,aYmax ] in the "Y Direction",
  //! -   interval [ aZmin,aZmax ] in the "Z Direction";
  Standard_EXPORT void Update(const Standard_Real aXmin,
                              const Standard_Real aYmin,
                              const Standard_Real aZmin,
                              const Standard_Real aXmax,
                              const Standard_Real aYmax,
                              const Standard_Real aZmax);

  //! Adds a point of coordinates (X,Y,Z) to this bounding box.
  Standard_EXPORT void Update(const Standard_Real X, const Standard_Real Y, const Standard_Real Z);

  //! Returns the gap of this bounding box.
  [[nodiscard]] constexpr Standard_Real GetGap() const noexcept { return Gap; }

  //! Set the gap of this bounding box to abs(Tol).
  void SetGap(const Standard_Real Tol) noexcept { Gap = std::abs(Tol); }

  //! Enlarges the box with a tolerance value.
  //! (minvalues-std::abs(<tol>) and maxvalues+std::abs(<tol>))
  //! This means that the minimum values of its X, Y and Z
  //! intervals of definition, when they are finite, are reduced by
  //! the absolute value of Tol, while the maximum values are
  //! increased by the same amount.
  void Enlarge(const Standard_Real Tol) noexcept { Gap = (std::max)(Gap, std::abs(Tol)); }

  //! Returns the bounds of this bounding box. The gap is included.
  //! If this bounding box is infinite (i.e. "open"), returned values
  //! may be equal to +/- Precision::Infinite().
  //! Standard_ConstructionError exception will be thrown if the box is void.
  //! if IsVoid()
  Standard_EXPORT void Get(Standard_Real& theXmin,
                           Standard_Real& theYmin,
                           Standard_Real& theZmin,
                           Standard_Real& theXmax,
                           Standard_Real& theYmax,
                           Standard_Real& theZmax) const;

  //! Returns the bounds of this bounding box as a Limits structure.
  //! The gap is included. If this bounding box is infinite (i.e. "open"),
  //! returned values may be equal to +/- Precision::Infinite().
  //! If the box is void, returns raw internal values.
  //! Can be used with C++17 structured bindings:
  //! @code
  //!   auto [xmin, xmax, ymin, ymax, zmin, zmax] = aBox.Get();
  //! @endcode
  [[nodiscard]] Standard_EXPORT Limits Get() const;

  //! Returns the Xmin value (IsOpenXmin() ? -Precision::Infinite() : Xmin - GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetXMin() const;

  //! Returns the Xmax value (IsOpenXmax() ? Precision::Infinite() : Xmax + GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetXMax() const;

  //! Returns the Ymin value (IsOpenYmin() ? -Precision::Infinite() : Ymin - GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetYMin() const;

  //! Returns the Ymax value (IsOpenYmax() ? Precision::Infinite() : Ymax + GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetYMax() const;

  //! Returns the Zmin value (IsOpenZmin() ? -Precision::Infinite() : Zmin - GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetZMin() const;

  //! Returns the Zmax value (IsOpenZmax() ? Precision::Infinite() : Zmax + GetGap()).
  [[nodiscard]] Standard_EXPORT Standard_Real GetZMax() const;

  //! Returns the lower corner of this bounding box. The gap is included.
  //! If this bounding box is infinite (i.e. "open"), returned values
  //! may be equal to +/- Precision::Infinite().
  //! Standard_ConstructionError exception will be thrown if the box is void.
  //! if IsVoid()
  [[nodiscard]] Standard_EXPORT gp_Pnt CornerMin() const;

  //! Returns the upper corner of this bounding box. The gap is included.
  //! If this bounding box is infinite (i.e. "open"), returned values
  //! may be equal to +/- Precision::Infinite().
  //! Standard_ConstructionError exception will be thrown if the box is void.
  //! if IsVoid()
  [[nodiscard]] Standard_EXPORT gp_Pnt CornerMax() const;

  //! The Box will be infinitely long in the Xmin
  //! direction.
  void OpenXmin() noexcept { Flags |= XminMask; }

  //! The Box will be infinitely long in the Xmax
  //! direction.
  void OpenXmax() noexcept { Flags |= XmaxMask; }

  //! The Box will be infinitely long in the Ymin
  //! direction.
  void OpenYmin() noexcept { Flags |= YminMask; }

  //! The Box will be infinitely long in the Ymax
  //! direction.
  void OpenYmax() noexcept { Flags |= YmaxMask; }

  //! The Box will be infinitely long in the Zmin
  //! direction.
  void OpenZmin() noexcept { Flags |= ZminMask; }

  //! The Box will be infinitely long in the Zmax
  //! direction.
  void OpenZmax() noexcept { Flags |= ZmaxMask; }

  //! Returns true if this bounding box has at least one open direction.
  [[nodiscard]] Standard_Boolean IsOpen() const noexcept { return (Flags & WholeMask) != 0; }

  //! Returns true if this bounding box is open in the Xmin direction.
  [[nodiscard]] Standard_Boolean IsOpenXmin() const noexcept { return (Flags & XminMask) != 0; }

  //! Returns true if this bounding box is open in the Xmax direction.
  [[nodiscard]] Standard_Boolean IsOpenXmax() const noexcept { return (Flags & XmaxMask) != 0; }

  //! Returns true if this bounding box is open in the Ymix direction.
  [[nodiscard]] Standard_Boolean IsOpenYmin() const noexcept { return (Flags & YminMask) != 0; }

  //! Returns true if this bounding box is open in the Ymax direction.
  [[nodiscard]] Standard_Boolean IsOpenYmax() const noexcept { return (Flags & YmaxMask) != 0; }

  //! Returns true if this bounding box is open in the Zmin direction.
  [[nodiscard]] Standard_Boolean IsOpenZmin() const noexcept { return (Flags & ZminMask) != 0; }

  //! Returns true if this bounding box is open in the Zmax direction.
  [[nodiscard]] Standard_Boolean IsOpenZmax() const noexcept { return (Flags & ZmaxMask) != 0; }

  //! Returns true if this bounding box is infinite in all 6 directions (WholeSpace flag).
  [[nodiscard]] Standard_Boolean IsWhole() const noexcept
  {
    return (Flags & WholeMask) == WholeMask;
  }

  //! Returns true if this bounding box is empty (Void flag).
  [[nodiscard]] Standard_Boolean IsVoid() const noexcept { return (Flags & VoidMask) != 0; }

  //! true if xmax-xmin < tol.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsXThin(const Standard_Real tol) const;

  //! true if ymax-ymin < tol.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsYThin(const Standard_Real tol) const;

  //! true if zmax-zmin < tol.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsZThin(const Standard_Real tol) const;

  //! Returns true if IsXThin, IsYThin and IsZThin are all true,
  //! i.e. if the box is thin in all three dimensions.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsThin(const Standard_Real tol) const;

  //! Returns a bounding box which is the result of applying the
  //! transformation T to this bounding box.
  //! Warning
  //! Applying a geometric transformation (for example, a
  //! rotation) to a bounding box generally increases its
  //! dimensions. This is not optimal for algorithms which use it.
  [[nodiscard]] Standard_EXPORT Bnd_Box Transformed(const gp_Trsf& T) const;

  //! Adds the box <Other> to <me>.
  Standard_EXPORT void Add(const Bnd_Box& Other);

  //! Adds a Pnt to the box.
  Standard_EXPORT void Add(const gp_Pnt& P);

  //! Extends <me> from the Pnt <P> in the direction <D>.
  Standard_EXPORT void Add(const gp_Pnt& P, const gp_Dir& D);

  //! Extends the Box in the given Direction, i.e. adds
  //! an half-line. The box may become infinite in
  //! 1,2 or 3 directions.
  Standard_EXPORT void Add(const gp_Dir& D);

  //! Returns True if the Pnt is out the box.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const gp_Pnt& P) const;

  //! Returns False if the line intersects the box.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const gp_Lin& L) const;

  //! Returns False if the plane intersects the box.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const gp_Pln& P) const;

  //! Returns False if the <Box> intersects or is inside <me>.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const Bnd_Box& Other) const;

  //! Returns False if the transformed <Box> intersects
  //! or is inside <me>.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const Bnd_Box& Other,
                                                       const gp_Trsf& T) const;

  //! Returns False if the transformed <Box> intersects
  //! or is inside the transformed box <me>.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const gp_Trsf& T1,
                                                       const Bnd_Box& Other,
                                                       const gp_Trsf& T2) const;

  //! Returns False if the flat band lying between two parallel
  //! lines represented by their reference points <P1>, <P2> and
  //! direction <D> intersects the box.
  [[nodiscard]] Standard_EXPORT Standard_Boolean IsOut(const gp_Pnt& P1,
                                                       const gp_Pnt& P2,
                                                       const gp_Dir& D) const;

  //! Computes the minimum distance between two boxes.
  [[nodiscard]] Standard_EXPORT Standard_Real Distance(const Bnd_Box& Other) const;

  Standard_EXPORT void Dump() const;

  //! Computes the squared diagonal of me.
  [[nodiscard]] Standard_Real SquareExtent() const noexcept
  {
    if (IsVoid())
    {
      return 0.0;
    }

    const Standard_Real aDx = Xmax - Xmin + Gap + Gap;
    const Standard_Real aDy = Ymax - Ymin + Gap + Gap;
    const Standard_Real aDz = Zmax - Zmin + Gap + Gap;
    return aDx * aDx + aDy * aDy + aDz * aDz;
  }

  //! Returns a finite part of an infinite bounding box (returns self if this is already finite
  //! box). This can be a Void box in case if its sides has been defined as infinite (Open) without
  //! adding any finite points. WARNING! This method relies on Open flags, the infinite points added
  //! using Add() method will be returned as is.
  [[nodiscard]] Bnd_Box FinitePart() const noexcept
  {
    if (!HasFinitePart())
    {
      return Bnd_Box();
    }

    Bnd_Box aBox;
    aBox.Update(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    aBox.SetGap(Gap);
    return aBox;
  }

  //! Returns TRUE if this box has finite part.
  [[nodiscard]] Standard_Boolean HasFinitePart() const noexcept
  {
    return !IsVoid() && Xmax >= Xmin;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson(const Standard_SStream& theSStream,
                                                Standard_Integer&       theStreamPos);

protected:
  //! Bit flags.
  enum MaskFlags
  {
    VoidMask  = 0x01,
    XminMask  = 0x02,
    XmaxMask  = 0x04,
    YminMask  = 0x08,
    YmaxMask  = 0x10,
    ZminMask  = 0x20,
    ZmaxMask  = 0x40,
    WholeMask = 0x7e
  };

private:
  Standard_Real    Xmin;
  Standard_Real    Xmax;
  Standard_Real    Ymin;
  Standard_Real    Ymax;
  Standard_Real    Zmin;
  Standard_Real    Zmax;
  Standard_Real    Gap;
  Standard_Integer Flags;
};

#endif // _Bnd_Box_HeaderFile
