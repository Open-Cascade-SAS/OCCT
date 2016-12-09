// Created on: 2016-12-09
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StdSelect_TypeOfSelectionImage_HeaderFile
#define _StdSelect_TypeOfSelectionImage_HeaderFile

//! Type of output selection image.
enum StdSelect_TypeOfSelectionImage
{
  StdSelect_TypeOfSelectionImage_NormalizedDepth = 0,     //!< normalized   depth (grayscale)
  StdSelect_TypeOfSelectionImage_NormalizedDepthInverted, //!< normalized   depth, inverted
  StdSelect_TypeOfSelectionImage_UnnormalizedDepth,       //!< unnormalized depth (grayscale)
  StdSelect_TypeOfSelectionImage_ColoredDetectedObject,   //!< color of detected object
  StdSelect_TypeOfSelectionImage_ColoredEntity,           //!< random color for each entity
  StdSelect_TypeOfSelectionImage_ColoredOwner,            //!< random color for each owner
  StdSelect_TypeOfSelectionImage_ColoredSelectionMode     //!< color of selection mode
};

#endif
