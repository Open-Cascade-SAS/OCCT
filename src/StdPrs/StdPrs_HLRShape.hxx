// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _StdPrs_HLRFace_H__
#define _StdPrs_HLRFace_H__

#include <Prs3d_Root.hxx>
#include <Handle_Prs3d_Presentation.hxx>
#include <Handle_Prs3d_Drawer.hxx>
#include <Handle_Prs3d_Projector.hxx>

class StdPrs_HLRShape: public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Add
                       (const Handle(Prs3d_Presentation)& thePresentation,
                        const TopoDS_Shape&               theShape,
                        const Handle(Prs3d_Drawer)&       theDrawer,
                        const Handle(Prs3d_Projector)&    theProjector);

};
#endif
