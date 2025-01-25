// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Blend_Status_HeaderFile
#define _Blend_Status_HeaderFile

enum Blend_Status
{
  Blend_StepTooLarge,
  Blend_StepTooSmall,
  Blend_Backward,
  Blend_SamePoints,
  Blend_OnRst1,
  Blend_OnRst2,
  Blend_OnRst12,
  Blend_OK
};

#endif // _Blend_Status_HeaderFile
