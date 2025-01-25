// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepCheck_Solid_HeaderFile
#define _BRepCheck_Solid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepCheck_Result.hxx>
class TopoDS_Solid;
class TopoDS_Shape;

class BRepCheck_Solid;
DEFINE_STANDARD_HANDLE(BRepCheck_Solid, BRepCheck_Result)

//! The class is to check a solid.
class BRepCheck_Solid : public BRepCheck_Result
{

public:
  //! Constructor
  //! <theS> is the solid to check
  Standard_EXPORT BRepCheck_Solid(const TopoDS_Solid& theS);

  //! Checks the solid in context of
  //! the shape <theContextShape>
  Standard_EXPORT virtual void InContext(const TopoDS_Shape& theContextShape) Standard_OVERRIDE;

  //! Checks the solid per se.
  //!
  //! The scan area is:
  //! 1.  Shells that overlaps each other
  //! Status:  BRepCheck_InvalidImbricationOfShells
  //!
  //! 2.  Detached parts of the solid (vertices, edges)
  //! that have non-internal orientation
  //! Status:  BRepCheck_BadOrientationOfSubshape
  //!
  //! 3.  For closed, non-internal shells:
  //! 3.1 Shells containing entities  of the solid that
  //! are outside towards the shells
  //! Status:  BRepCheck_SubshapeNotInShape
  //!
  //! 3.2 Shells that encloses other Shells
  //! (for non-holes)
  //! Status:  BRepCheck_EnclosedRegion
  Standard_EXPORT virtual void Minimum() Standard_OVERRIDE;

  //! see the parent class for more details
  Standard_EXPORT virtual void Blind() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepCheck_Solid, BRepCheck_Result)

protected:
private:
};

#endif // _BRepCheck_Solid_HeaderFile
