// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef StdPrs_HLRShape_HeaderFile
#define StdPrs_HLRShape_HeaderFile

#include <StdPrs_HLRShapeI.hxx>

//! Computes the presentation of objects with removal of their hidden lines for a specific
//! projector. The exact algorithm is used.
class StdPrs_HLRShape : public StdPrs_HLRShapeI
{
  DEFINE_STANDARD_RTTIEXT(StdPrs_HLRShape, StdPrs_HLRShapeI)
public:
  //! Compute presentation for specified shape.
  Standard_EXPORT virtual void ComputeHLR(const Handle(Prs3d_Presentation)& thePrs,
                                          const TopoDS_Shape&               theShape,
                                          const Handle(Prs3d_Drawer)&       theDrawer,
                                          const Handle(Graphic3d_Camera)&   theProjector) const
    Standard_OVERRIDE;
};
#endif
