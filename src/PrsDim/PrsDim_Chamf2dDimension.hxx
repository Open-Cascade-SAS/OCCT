// Created on: 1996-12-05
// Created by: Flore Lantheaume/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _PrsDim_Chamf2dDimension_HeaderFile
#define _PrsDim_Chamf2dDimension_HeaderFile

#include <PrsDim_KindOfDimension.hxx>
#include <PrsDim_Relation.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <gp_Dir.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(PrsDim_Chamf2dDimension, PrsDim_Relation)

//! A framework to define display of 2D chamfers.
//! A chamfer is displayed with arrows and text. The text
//! gives the length of the chamfer if it is a symmetrical
//! chamfer, or the angle if it is not.
class PrsDim_Chamf2dDimension : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_Chamf2dDimension, PrsDim_Relation)
public:

  //! Constructs the display object for 2D chamfers.
  //! This object is defined by the face aFShape, the
  //! dimension aVal, the plane aPlane and the text aText.
  Standard_EXPORT PrsDim_Chamf2dDimension(const TopoDS_Shape& aFShape, const Handle(Geom_Plane)& aPlane, const Standard_Real aVal, const TCollection_ExtendedString& aText);
  
  //! Constructs the display object for 2D chamfers.
  //! This object is defined by the face aFShape, the plane
  //! aPlane, the dimension aVal, the position aPosition,
  //! the type of arrow aSymbolPrs with the size
  //! anArrowSize, and the text aText.
  Standard_EXPORT PrsDim_Chamf2dDimension(const TopoDS_Shape& aFShape, const Handle(Geom_Plane)& aPlane, const Standard_Real aVal, const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, const DsgPrs_ArrowSide aSymbolPrs, const Standard_Real anArrowSize = 0.0);

  //! Indicates that we are concerned with a 2d length.
  virtual PrsDim_KindOfDimension KindOfDimension() const Standard_OVERRIDE { return PrsDim_KOD_LENGTH; }

  //! Returns true if the 2d chamfer dimension is movable.
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

private:

  gp_Pnt myPntAttach;
  gp_Dir myDir;

};

#endif // _PrsDim_Chamf2dDimension_HeaderFile
