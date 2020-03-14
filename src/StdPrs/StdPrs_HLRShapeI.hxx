// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _StdPrs_HLRShapeI_HeaderFile
#define _StdPrs_HLRShapeI_HeaderFile

#include <Prs3d_Presentation.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

class TopoDS_Shape;
class Prs3d_Drawer;
class Graphic3d_Camera;

//! Computes the presentation of objects with removal of their hidden lines for a specific projector.
class StdPrs_HLRShapeI : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(StdPrs_HLRShapeI, Standard_Transient)
public:

  //! Compute presentation for specified shape.
  virtual void ComputeHLR (const Handle(Prs3d_Presentation)& thePrs,
                           const TopoDS_Shape& theShape,
                           const Handle(Prs3d_Drawer)& theDrawer,
                           const Handle(Graphic3d_Camera)& theProjector) const = 0;

};

#endif // _StdPrs_HLRShapeI_HeaderFile
