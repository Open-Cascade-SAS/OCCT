// Created on: 1995-01-25
// Created by: Jean-Louis Frenkel
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _PrsMgr_TypeOfPresentation3d_HeaderFile
#define _PrsMgr_TypeOfPresentation3d_HeaderFile

#include <Standard_Macro.hxx>

//! The type of presentation.
enum PrsMgr_TypeOfPresentation3d
{
  //! Presentation display involves no recalculation for new projectors (points of view) in hidden
  //! line removal mode.
  PrsMgr_TOP_AllView,
  //! Every new point of view entails recalculation of the display in hidden line removal mode.
  PrsMgr_TOP_ProjectorDependent
};

Standard_DEPRECATED("PrsMgr_TOP_ProjectorDependent should be used instead")
const PrsMgr_TypeOfPresentation3d PrsMgr_TOP_ProjectorDependant = PrsMgr_TOP_ProjectorDependent;

#endif // _PrsMgr_TypeOfPresentation3d_HeaderFile
