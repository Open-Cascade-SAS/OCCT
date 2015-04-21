// Created on: 2014-10-14
// Created by: Anton POLETAEV
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

#ifndef _StdPrs_BndBox_H__
#define _StdPrs_BndBox_H__

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Bnd_Box.hxx>

//! Tool for computing bounding box presentation.
class StdPrs_BndBox : public Prs3d_Root
{
public:

  //! Computes presentation of a bounding box.
  //! @param thePresentation [in] the presentation.
  //! @param theBndBox [in] the bounding box.
  //! @param theDrawer [in] the drawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& thePresentation,
                                   const Bnd_Box& theBndBox,
                                   const Handle(Prs3d_Drawer)& theDrawer);
};

#endif // _StdPrs_BndBox_H__
