// Created on: 1998-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StdSelect_Shape_HeaderFile
#define _StdSelect_Shape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>

class TopoDS_Shape;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;

//! Presentable shape only for purpose of display for BRepOwner...
class StdSelect_Shape : public PrsMgr_PresentableObject
{
  DEFINE_STANDARD_RTTIEXT(StdSelect_Shape, PrsMgr_PresentableObject)
public:

  Standard_EXPORT StdSelect_Shape(const TopoDS_Shape& theShape, const Handle(Prs3d_Drawer)& theDrawer = Handle(Prs3d_Drawer)());
  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;

  const TopoDS_Shape& Shape() const { return mysh; }

  void Shape (const TopoDS_Shape& theShape) { mysh = theShape; }

private:

  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;

private:

  TopoDS_Shape mysh;

};

DEFINE_STANDARD_HANDLE(StdSelect_Shape, PrsMgr_PresentableObject)

#endif // _StdSelect_Shape_HeaderFile
