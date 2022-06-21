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

#include <TopoDS_Shape.hxx>
#include <PrsMgr_PresentableObject.hxx>


//! Presentable shape only for purpose of display for BRepOwner...
class StdSelect_Shape : public PrsMgr_PresentableObject
{
  DEFINE_STANDARD_RTTIEXT(StdSelect_Shape, PrsMgr_PresentableObject)
public:

  Standard_EXPORT StdSelect_Shape(const TopoDS_Shape& theShape, const Handle(Prs3d_Drawer)& theDrawer = Handle(Prs3d_Drawer)());

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  const TopoDS_Shape& Shape() const { return mysh; }

  void Shape (const TopoDS_Shape& theShape) { mysh = theShape; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  TopoDS_Shape mysh;

};

DEFINE_STANDARD_HANDLE(StdSelect_Shape, PrsMgr_PresentableObject)

#endif // _StdSelect_Shape_HeaderFile
