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
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

#include <Bnd_Box.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

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

  //! Empty constructor
  Bnd_OBB() :myIsAABox(Standard_False)
  {
    myHDims[0] = myHDims[1] = myHDims[2] = -1.0;
  }

  //! Constructor taking all defining parameters
  Bnd_OBB(const gp_Pnt& theCenter,
          const gp_Dir& theXDirection,
          const gp_Dir& theYDirection,
          const gp_Dir& theZDirection,
          const Standard_Real theHXSize,
          const Standard_Real theHYSize,
          const Standard_Real theHZSize) :myCenter (theCenter.XYZ()),
                                          myIsAABox(Standard_False)
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
  Bnd_OBB(const Bnd_Box& theBox) : myIsAABox(Standard_True)
  {
    Standard_Real aX1, aY1, aZ1, aX2, aY2, aZ2;
    theBox.Get(aX1, aY1, aZ1, aX2, aY2, aZ2);

    myAxes[0].SetCoord(1.0, 0.0, 0.0);
    myAxes[1].SetCoord(0.0, 1.0, 0.0);
    myAxes[2].SetCoord(0.0, 0.0, 1.0);

    myHDims[0] = 0.5*(aX2 - aX1);
    myHDims[1] = 0.5*(aY2 - aY1);
    myHDims[2] = 0.5*(aZ2 - aZ1);

    myCenter.SetCoord(0.5*(aX2 + aX1), 0.5*(aY2 + aY1), 0.5*(aZ2 + aZ1));
  }

  //! Created new OBB covering every point in theListOfPoints.
  //! Tolerance of every such point is set by *theListOfTolerances array.
  //! If this array is not void (not null-pointer) then the resulted Bnd_OBB
  //! will be enlarged using tolerances of points lying on the box surface.
  Standard_EXPORT void ReBuild(const TColgp_Array1OfPnt& theListOfPoints,
                               const TColStd_Array1OfReal *theListOfTolerances = 0);

  //! Sets the center of OBB
  void SetCenter(const gp_Pnt& theCenter)
  {
    myCenter = theCenter.XYZ();
  }

    //! Sets the X component of OBB - direction and size
  void SetXComponent(const gp_Dir& theXDirection,
                     const Standard_Real theHXSize)
  {
    Standard_ASSERT_VOID(theHXSize >= 0.0, "Negative value of X-size");

    myAxes[0] = theXDirection.XYZ();
    myHDims[0] = theHXSize;
  }

  //! Sets the Y component of OBB - direction and size
  void SetYComponent(const gp_Dir& theYDirection,
                     const Standard_Real theHYSize)
  {
    Standard_ASSERT_VOID(theHYSize >= 0.0, "Negative value of Y-size");

    myAxes[1] = theYDirection.XYZ();
    myHDims[1] = theHYSize;
  }

  //! Sets the Z component of OBB - direction and size
  void SetZComponent(const gp_Dir& theZDirection,
                     const Standard_Real theHZSize)
  {
    Standard_ASSERT_VOID(theHZSize >= 0.0, "Negative value of Z-size");

    myAxes[2] = theZDirection.XYZ();
    myHDims[2] = theHZSize;
  }
  
  //! Returns the center of OBB
  const gp_XYZ& Center() const
  {
    return myCenter;
  }

  //! Returns the X Direction of OBB
  const gp_XYZ& XDirection() const
  {
    return myAxes[0];
  }

  //! Returns the Y Direction of OBB
  const gp_XYZ& YDirection() const
  {
    return myAxes[1];
  }

  //! Returns the Z Direction of OBB
  const gp_XYZ& ZDirection() const
  {
    return myAxes[2];
  }

  //! Returns the X Dimension of OBB
  Standard_Real XHSize() const
  {
    return myHDims[0];
  }

  //! Returns the Y Dimension of OBB
  Standard_Real YHSize() const
  {
    return myHDims[1];
  }

  //! Returns the Z Dimension of OBB
  Standard_Real ZHSize() const
  {
    return myHDims[2];
  }

  //! Checks if the box is empty.
  Standard_Boolean IsVoid() const
  {
    return ((myHDims[0] < 0.0) || (myHDims[1] < 0.0) || (myHDims[2] < 0.0));
  }

  //! Clears this box
  void SetVoid()
  {
    myHDims[0] = myHDims[1] = myHDims[2] = -1.0;
    myCenter = myAxes[0] = myAxes[1] = myAxes[2] = gp_XYZ();
    myIsAABox = Standard_False;
  }

  //! Sets the flag for axes aligned box
  void SetAABox(const Standard_Boolean& theFlag)
  {
    myIsAABox = theFlag;
  }

  //! Returns TRUE if the box is axes aligned
  Standard_Boolean IsAABox() const
  {
    return myIsAABox;
  }

  //! Enlarges the box with the given value
  void Enlarge(const Standard_Real theGapAdd)
  {
    const Standard_Real aGap = Abs(theGapAdd);
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
  Standard_Boolean GetVertex(gp_Pnt theP[8]) const
  {
    if(IsVoid())
      return Standard_False;

    theP[0].SetXYZ(myCenter - myHDims[0]*myAxes[0] - myHDims[1]*myAxes[1] - myHDims[2]*myAxes[2]);
    theP[1].SetXYZ(myCenter + myHDims[0]*myAxes[0] - myHDims[1]*myAxes[1] - myHDims[2]*myAxes[2]);
    theP[2].SetXYZ(myCenter - myHDims[0]*myAxes[0] + myHDims[1]*myAxes[1] - myHDims[2]*myAxes[2]);
    theP[3].SetXYZ(myCenter + myHDims[0]*myAxes[0] + myHDims[1]*myAxes[1] - myHDims[2]*myAxes[2]);
    theP[4].SetXYZ(myCenter - myHDims[0]*myAxes[0] - myHDims[1]*myAxes[1] + myHDims[2]*myAxes[2]);
    theP[5].SetXYZ(myCenter + myHDims[0]*myAxes[0] - myHDims[1]*myAxes[1] + myHDims[2]*myAxes[2]);
    theP[6].SetXYZ(myCenter - myHDims[0]*myAxes[0] + myHDims[1]*myAxes[1] + myHDims[2]*myAxes[2]);
    theP[7].SetXYZ(myCenter + myHDims[0]*myAxes[0] + myHDims[1]*myAxes[1] + myHDims[2]*myAxes[2]);

    return Standard_True;
  }

  //! Returns square diagonal of this box
  Standard_Real SquareExtent() const
  {
    return (4.0*myHDims[0] * myHDims[0] + 
            myHDims[1] * myHDims[1] +
            myHDims[1] * myHDims[1]);
  }

  //! Check if the box do not interfere the other box.
  Standard_EXPORT Standard_Boolean IsOut(const Bnd_OBB& theOther) const;

  //! Check if the point is inside of <this>.
  Standard_EXPORT Standard_Boolean IsOut(const gp_Pnt& theP) const;

  //! Check if the theOther is completely inside *this.
  Standard_EXPORT Standard_Boolean IsCompletelyInside(const Bnd_OBB& theOther) const;

  //! Rebuilds this in order to include all previous objects
  //! (which it was created from) and theOther.
  Standard_EXPORT void Add(const Bnd_OBB& theOther);

  //! Rebuilds this in order to include all previous objects
  //! (which it was created from) and theP.
  Standard_EXPORT void Add(const gp_Pnt& theP);

  protected:
    void ProcessOnePoint(const gp_Pnt& theP)
    {
      myIsAABox = Standard_True;
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
  Standard_Real myHDims[3];

  //! To be set if the OBB is axis aligned box;
  Standard_Boolean myIsAABox;
};

#endif
