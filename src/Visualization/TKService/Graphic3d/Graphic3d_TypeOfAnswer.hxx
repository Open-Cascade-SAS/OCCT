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

#ifndef _Graphic3d_TypeOfAnswer_HeaderFile
#define _Graphic3d_TypeOfAnswer_HeaderFile

//! The answer of the method AcceptDisplay
//! AcceptDisplay  means is it possible to display the
//! specified structure in the specified view ?
//! TOA_YES yes
//! TOA_NO  no
//! TOA_COMPUTE yes but we have to compute the representation
enum Graphic3d_TypeOfAnswer
{
  Graphic3d_TOA_YES,
  Graphic3d_TOA_NO,
  Graphic3d_TOA_COMPUTE
};

#endif // _Graphic3d_TypeOfAnswer_HeaderFile
