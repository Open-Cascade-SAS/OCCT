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

#ifndef _Bnd_OBB_HeaderFile
#define _Bnd_OBB_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>

//! The class describes the Oriented Bounding Box (OBB),
//! much tighter enclosing volume for the shape than the
//! Axis Aligned Bounding Box (AABB).
//! The OBB is defined by a center of the box, the axes and the halves
//! of its three dimensions.
//! The OBB can be used more effectively than AABB as a rejection mechanism
//! for non-interfering objects.
class Bnd_OBB
{
public:
  DEFINE_STANDARD_ALLOC

  //! Structure containing the OBB half-size dimensions.
  //! Can be used with C++17 structured bindings:
  //! @code
  //!   auto [aHX, aHY, aHZ] = anOBB.GetHalfSizes();
  //! @endcode
  struct HalfSizes
  {
    double X; //!< Half-size along X axis
    double Y; //!< Half-size along Y axis
    double Z; //!< Half-size along Z axis
  };

  //! Empty constructor
  Bnd_OBB()
      : myIsAABox(false)
  {
    myHDims[0] = myHDims[1] = myHDims[2] = -1.0;
  }

  //! Constructor taking all defining parameters
  Bnd_OBB(const gp_Pnt& theCenter,
          const gp_Dir& theXDirection,
          const gp_Dir& theYDirection,
          const gp_Dir& theZDirection,
          const double  theHXSize,
          const double  theHYSize,
          const double  theHZSize)
      : myCenter(theCenter.XYZ()),
        myIsAABox(false)
  {
    myAxes[0] = theXDirection.XYZ();
    myAxes[1] = theYDirection.XYZ();
    myAxes[2] = theZDirection.XYZ();

    Standard_ASSERT_VOID(theHXSize >= 0.0, "Negative value of X-size");
    Standard_ASSERT_VOID(theHYSize >= 0.0, "Negative value of Y-size");
    Standard_ASSERT_VOID(theHZSize >= 0.0, "Negative value of Z-size");

    myHDims[0] = theHXSize;
    myHDims[1] = theHYSize;
    myHDims[2] = theHZSize;
  }

  //! Constructor to create OBB from AABB.
  Bnd_OBB(const Bnd_Box& theBox)
      : myIsAABox(true)
  {
    if (theBox.IsVoid())
    {
      myHDims[0] = myHDims[1] = myHDims[2] = -1.0;
      myIsAABox                            = false;
      return;
    }

    double aX1, aY1, aZ1, aX2, aY2, aZ2;
    theBox.Get(aX1, aY1, aZ1, aX2, aY2, aZ2);

    myAxes[0].SetCoord(1.0, 0.0, 0.0);
    myAxes[1].SetCoord(0.0, 1.0, 0.0);
    myAxes[2].SetCoord(0.0, 0.0, 1.0);

    myHDims[0] = 0.5 * (aX2 - aX1);
    myHDims[1] = 0.5 * (aY2 - aY1);
    myHDims[2] = 0.5 * (aZ2 - aZ1);

    myCenter.SetCoord(0.5 * (aX2 + aX1), 0.5 * (aY2 + aY1), 0.5 * (aZ2 + aZ1));
  }

  //! Creates new OBB covering every point in theListOfPoints.
  //! Tolerance of every such point is set by *theListOfTolerances array.
  //! If this array is not void (not null-pointer) then the resulted Bnd_OBB
  //! will be enlarged using tolerances of points lying on the box surface.
  //! <theIsOptimal> flag defines the mode in which the OBB will be built.
  //! Constructing Optimal box takes more time, but the resulting box is usually
  //! more tight. In case of construction of Optimal OBB more possible
  //! axes are checked.
  Standard_EXPORT void ReBuild(const NCollection_Array1<gp_Pnt>& theListOfPoints,
                               const NCollection_Array1<double>* theListOfTolerances = nullptr,
                               const bool                        theIsOptimal        = false);

  //! Sets the center of OBB
  void SetCenter(const gp_Pnt& theCenter) { myCenter = theCenter.XYZ(); }

  //! Sets the X component of OBB - direction and size
  void SetXComponent(const gp_Dir& theXDirection, const double theHXSize)
  {
    Standard_ASSERT_VOID(theHXSize >= 0.0, "Negative value of X-size");

    myAxes[0]  = theXDirection.XYZ();
    myHDims[0] = theHXSize;
  }

  //! Sets the Y component of OBB - direction and size
  void SetYComponent(const gp_Dir& theYDirection, const double theHYSize)
  {
    Standard_ASSERT_VOID(theHYSize >= 0.0, "Negative value of Y-size");

    myAxes[1]  = theYDirection.XYZ();
    myHDims[1] = theHYSize;
  }

  //! Sets the Z component of OBB - direction and size
  void SetZComponent(const gp_Dir& theZDirection, const double theHZSize)
  {
    Standard_ASSERT_VOID(theHZSize >= 0.0, "Negative value of Z-size");

    myAxes[2]  = theZDirection.XYZ();
    myHDims[2] = theHZSize;
  }

  //! Returns the local coordinates system of this oriented box.
  //! So that applying it to axis-aligned box ((-XHSize, -YHSize, -ZHSize), (XHSize, YHSize,
  //! ZHSize)) will produce this oriented box.
  //! @code
  //!   gp_Trsf aLoc;
  //!   aLoc.SetTransformation (theOBB.Position(), gp::XOY());
  //! @endcode
  [[nodiscard]] gp_Ax3 Position() const { return gp_Ax3(myCenter, ZDirection(), XDirection()); }

  //! Returns the center of OBB
  [[nodiscard]] const gp_XYZ& Center() const noexcept { return myCenter; }

  //! Returns the X Direction of OBB
  [[nodiscard]] const gp_XYZ& XDirection() const noexcept { return myAxes[0]; }

  //! Returns the Y Direction of OBB
  [[nodiscard]] const gp_XYZ& YDirection() const noexcept { return myAxes[1]; }

  //! Returns the Z Direction of OBB
  [[nodiscard]] const gp_XYZ& ZDirection() const noexcept { return myAxes[2]; }

  //! Returns the X Dimension of OBB
  [[nodiscard]] double XHSize() const noexcept { return myHDims[0]; }

  //! Returns the Y Dimension of OBB
  [[nodiscard]] double YHSize() const noexcept { return myHDims[1]; }

  //! Returns the Z Dimension of OBB
  [[nodiscard]] double ZHSize() const noexcept { return myHDims[2]; }

  //! Returns the half-size dimensions of the OBB as a HalfSizes structure.
  //! Can be used with C++17 structured bindings:
  //! @code
  //!   auto [aHX, aHY, aHZ] = anOBB.GetHalfSizes();
  //! @endcode
  [[nodiscard]] HalfSizes GetHalfSizes() const noexcept { return {myHDims[0], myHDims[1], myHDims[2]}; }

  //! Checks if the box is empty.
  [[nodiscard]] bool IsVoid() const noexcept
  {
    return ((myHDims[0] < 0.0) || (myHDims[1] < 0.0) || (myHDims[2] < 0.0));
  }

  //! Clears this box
  void SetVoid()
  {
    myHDims[0] = myHDims[1] = myHDims[2] = -1.0;
    myCenter = myAxes[0] = myAxes[1] = myAxes[2] = gp_XYZ();
    myIsAABox                                    = false;
  }

  //! Sets the flag for axes aligned box
  void SetAABox(const bool& theFlag) { myIsAABox = theFlag; }

  //! Returns TRUE if the box is axes aligned
  [[nodiscard]] bool IsAABox() const noexcept { return myIsAABox; }

  //! Enlarges the box with the given value
  void Enlarge(const double theGapAdd)
  {
    const double aGap = std::abs(theGapAdd);
    myHDims[0] += aGap;
    myHDims[1] += aGap;
    myHDims[2] += aGap;
  }

  //! Returns the array of vertices in <this>.
  //! The local coordinate of the vertex depending on the
  //! index of the array are follow:
  //! Index == 0: (-XHSize(), -YHSize(), -ZHSize())
  //! Index == 1: ( XHSize(), -YHSize(), -ZHSize())
  //! Index == 2: (-XHSize(),  YHSize(), -ZHSize())
  //! Index == 3: ( XHSize(),  YHSize(), -ZHSize())
  //! Index == 4: (-XHSize(), -YHSize(),  ZHSize())
  //! Index == 5: ( XHSize(), -YHSize(),  ZHSize())
  //! Index == 6: (-XHSize(),  YHSize(),  ZHSize())
  //! Index == 7: ( XHSize(),  YHSize(),  ZHSize()).
  bool GetVertex(gp_Pnt theP[8]) const
  {
    if (IsVoid())
      return false;

    theP[0].SetXYZ(myCenter - myHDims[0] * myAxes[0] - myHDims[1] * myAxes[1]
                   - myHDims[2] * myAxes[2]);
    theP[1].SetXYZ(myCenter + myHDims[0] * myAxes[0] - myHDims[1] * myAxes[1]
                   - myHDims[2] * myAxes[2]);
    theP[2].SetXYZ(myCenter - myHDims[0] * myAxes[0] + myHDims[1] * myAxes[1]
                   - myHDims[2] * myAxes[2]);
    theP[3].SetXYZ(myCenter + myHDims[0] * myAxes[0] + myHDims[1] * myAxes[1]
                   - myHDims[2] * myAxes[2]);
    theP[4].SetXYZ(myCenter - myHDims[0] * myAxes[0] - myHDims[1] * myAxes[1]
                   + myHDims[2] * myAxes[2]);
    theP[5].SetXYZ(myCenter + myHDims[0] * myAxes[0] - myHDims[1] * myAxes[1]
                   + myHDims[2] * myAxes[2]);
    theP[6].SetXYZ(myCenter - myHDims[0] * myAxes[0] + myHDims[1] * myAxes[1]
                   + myHDims[2] * myAxes[2]);
    theP[7].SetXYZ(myCenter + myHDims[0] * myAxes[0] + myHDims[1] * myAxes[1]
                   + myHDims[2] * myAxes[2]);

    return true;
  }

  //! Returns square diagonal of this box
  [[nodiscard]] double SquareExtent() const noexcept
  {
    return 4.0 * (myHDims[0] * myHDims[0] + myHDims[1] * myHDims[1] + myHDims[2] * myHDims[2]);
  }

  //! Check if the box do not interfere the other box.
  [[nodiscard]] Standard_EXPORT bool IsOut(const Bnd_OBB& theOther) const;

  //! Check if the point is inside of <this>.
  [[nodiscard]] Standard_EXPORT bool IsOut(const gp_Pnt& theP) const;

  //! Returns True if the point is inside or on the boundary of this OBB.
  [[nodiscard]] bool Contains(const gp_Pnt& theP) const { return !IsOut(theP); }

  //! Returns True if the other OBB intersects or is inside this OBB.
  [[nodiscard]] bool Intersects(const Bnd_OBB& theOther) const { return !IsOut(theOther); }

  //! Check if the theOther is completely inside *this.
  [[nodiscard]] Standard_EXPORT bool IsCompletelyInside(const Bnd_OBB& theOther) const;

  //! Rebuilds this in order to include all previous objects
  //! (which it was created from) and theOther.
  Standard_EXPORT void Add(const Bnd_OBB& theOther);

  //! Rebuilds this in order to include all previous objects
  //! (which it was created from) and theP.
  Standard_EXPORT void Add(const gp_Pnt& theP);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

protected:
  void ProcessOnePoint(const gp_Pnt& theP)
  {
    myIsAABox  = true;
    myHDims[0] = myHDims[1] = myHDims[2] = 0.0;
    myAxes[0].SetCoord(1.0, 0.0, 0.0);
    myAxes[1].SetCoord(0.0, 1.0, 0.0);
    myAxes[2].SetCoord(0.0, 0.0, 1.0);
    myCenter = theP.XYZ();
  }

private:
  //! Center of the OBB
  gp_XYZ myCenter;

  //! Directions of the box's axes
  //! (all vectors are already normalized)
  gp_XYZ myAxes[3];

  //! Half-size dimensions of the OBB
  double myHDims[3];

  //! To be set if the OBB is axis aligned box;
  bool myIsAABox;
};

#endif
