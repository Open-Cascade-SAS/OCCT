// Created on: 1999-07-22
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeUpgrade_ClosedFaceDivide_HeaderFile
#define _ShapeUpgrade_ClosedFaceDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
class TopoDS_Face;


class ShapeUpgrade_ClosedFaceDivide;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_ClosedFaceDivide, ShapeUpgrade_FaceDivide)

//! Divides a Face with one or more seam edge to avoid closed faces.
//! Splitting is performed by U and V direction. The number of
//! resulting faces can be defined by user.
class ShapeUpgrade_ClosedFaceDivide : public ShapeUpgrade_FaceDivide
{

public:

  //! Creates empty  constructor.
  Standard_EXPORT ShapeUpgrade_ClosedFaceDivide();
  
  //! Initialize by a Face.
  Standard_EXPORT ShapeUpgrade_ClosedFaceDivide(const TopoDS_Face& F);
  
  //! Performs splitting of surface and computes the shell
  //! from source face.
  Standard_EXPORT virtual Standard_Boolean SplitSurface(const Standard_Real theArea = 0.) Standard_OVERRIDE;
  
  //! Sets the number of cutting lines by which closed face will be split.
  //! The resulting faces will be num+1.
  Standard_EXPORT void SetNbSplitPoints (const Standard_Integer num);
  
  //! Returns the number of splitting points
  Standard_EXPORT Standard_Integer GetNbSplitPoints() const;


  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_ClosedFaceDivide,ShapeUpgrade_FaceDivide)

private:

  Standard_Integer myNbSplit;

};

#endif // _ShapeUpgrade_ClosedFaceDivide_HeaderFile
