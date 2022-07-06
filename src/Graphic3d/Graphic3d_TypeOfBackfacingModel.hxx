// Created on: 1991-10-07
// Created by: NW,JPB,CAL
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

#ifndef _Graphic3d_TypeOfBackfacingModel_HeaderFile
#define _Graphic3d_TypeOfBackfacingModel_HeaderFile

//! Modes of display of back faces in the view.
enum Graphic3d_TypeOfBackfacingModel
{
  Graphic3d_TypeOfBackfacingModel_Auto,        //!< automatic back face culling enabled for opaque groups with closed flag
                                               //!  (e.g. solids, see Graphic3d_Group::IsClosed())
  Graphic3d_TypeOfBackfacingModel_DoubleSided, //!< no culling (double-sided shading)
  Graphic3d_TypeOfBackfacingModel_BackCulled,  //!< back  face culling
  Graphic3d_TypeOfBackfacingModel_FrontCulled, //!< front face culling
  // old aliases
  Graphic3d_TOBM_AUTOMATIC  = Graphic3d_TypeOfBackfacingModel_Auto,
  Graphic3d_TOBM_FORCE      = Graphic3d_TypeOfBackfacingModel_DoubleSided,
  Graphic3d_TOBM_DISABLE    = Graphic3d_TypeOfBackfacingModel_BackCulled,
  V3d_TOBM_AUTOMATIC        = Graphic3d_TypeOfBackfacingModel_Auto,
  V3d_TOBM_ALWAYS_DISPLAYED = Graphic3d_TypeOfBackfacingModel_DoubleSided,
  V3d_TOBM_NEVER_DISPLAYED  = Graphic3d_TypeOfBackfacingModel_BackCulled
};

#endif // _Graphic3d_TypeOfBackfacingModel_HeaderFile
