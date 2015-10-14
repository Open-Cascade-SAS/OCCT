// Created on: 1991-01-08
// Created by: Didier Piffault
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

#ifndef _Bnd_B2f_HeaderFile
#define _Bnd_B2f_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_XY.hxx>
#include <Standard_Real.hxx>
class gp_XY;
class gp_Pnt2d;
class gp_Trsf2d;
class gp_Ax2d;



class Bnd_B2f 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    Bnd_B2f();
  
  //! Constructor.
    Bnd_B2f(const gp_XY& theCenter, const gp_XY& theHSize);
  
  //! Returns True if the box is void (non-initialized).
    Standard_Boolean IsVoid() const;
  
  //! Reset the box data.
    void Clear();
  
  //! Update the box by a point.
  Standard_EXPORT void Add (const gp_XY& thePnt);
  
  //! Update the box by a point.
  Standard_EXPORT void Add (const gp_Pnt2d& thePnt);
  
  //! Update the box by another box.
    void Add (const Bnd_B2f& theBox);
  
  //! Query a box corner: (Center - HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
    gp_XY CornerMin() const;
  
  //! Query a box corner: (Center + HSize). You must make sure that
  //! the box is NOT VOID (see IsVoid()), otherwise the method returns
  //! irrelevant result.
    gp_XY CornerMax() const;
  
  //! Query the square diagonal. If the box is VOID (see method IsVoid())
  //! then a very big real value is returned.
    Standard_Real SquareExtent() const;
  
  //! Extend the Box by the absolute value of theDiff.
    void Enlarge (const Standard_Real theDiff);
  
  //! Limit the Box by the internals of theOtherBox.
  //! Returns True if the limitation takes place, otherwise False
  //! indicating that the boxes do not intersect.
  Standard_EXPORT Standard_Boolean Limit (const Bnd_B2f& theOtherBox);
  
  //! Transform the bounding box with the given transformation.
  //! The resulting box will be larger if theTrsf contains rotation.
  Standard_EXPORT Bnd_B2f Transformed (const gp_Trsf2d& theTrsf) const;
  
  //! Check the given point for the inclusion in the Box.
  //! Returns True if the point is outside.
    Standard_Boolean IsOut (const gp_XY& thePnt) const;
  
  //! Check a circle for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_EXPORT Standard_Boolean IsOut (const gp_XY& theCenter, const Standard_Real theRadius, const Standard_Boolean isCircleHollow = Standard_False) const;
  
  //! Check the given box for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
    Standard_Boolean IsOut (const Bnd_B2f& theOtherBox) const;
  
  //! Check the given box oriented by the given transformation
  //! for the intersection with the current box.
  //! Returns True if there is no intersection between boxes.
  Standard_EXPORT Standard_Boolean IsOut (const Bnd_B2f& theOtherBox, const gp_Trsf2d& theTrsf) const;
  
  //! Check the given Line for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_EXPORT Standard_Boolean IsOut (const gp_Ax2d& theLine) const;
  
  //! Check the Segment defined by the couple of input points
  //! for the intersection with the current box.
  //! Returns True if there is no intersection.
  Standard_EXPORT Standard_Boolean IsOut (const gp_XY& theP0, const gp_XY& theP1) const;
  
  //! Check that the box 'this' is inside the given box 'theBox'. Returns
  //! True if 'this' box is fully inside 'theBox'.
    Standard_Boolean IsIn (const Bnd_B2f& theBox) const;
  
  //! Check that the box 'this' is inside the given box 'theBox'
  //! transformed by 'theTrsf'. Returns True if 'this' box is fully
  //! inside the transformed 'theBox'.
  Standard_EXPORT Standard_Boolean IsIn (const Bnd_B2f& theBox, const gp_Trsf2d& theTrsf) const;
  
  //! Set the Center coordinates
    void SetCenter (const gp_XY& theCenter);
  
  //! Set the HSize (half-diagonal) coordinates.
  //! All components of theHSize must be non-negative.
    void SetHSize (const gp_XY& theHSize);




protected:



  Standard_ShortReal myCenter[2];
  Standard_ShortReal myHSize[2];


private:





};

#define RealType Standard_ShortReal
#define RealType_hxx <Standard_ShortReal.hxx>
#define Bnd_B2x Bnd_B2f
#define Bnd_B2x_hxx <Bnd_B2f.hxx>

#include <Bnd_B2x.lxx>

#undef RealType
#undef RealType_hxx
#undef Bnd_B2x
#undef Bnd_B2x_hxx




#endif // _Bnd_B2f_HeaderFile
