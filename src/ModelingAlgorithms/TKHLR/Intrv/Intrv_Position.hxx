// Created on: 1991-12-13
// Created by: Christophe MARION
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

#ifndef _Intrv_Position_HeaderFile
#define _Intrv_Position_HeaderFile

enum Intrv_Position
{
  Intrv_Before,
  Intrv_JustBefore,
  Intrv_OverlappingAtStart,
  Intrv_JustEnclosingAtEnd,
  Intrv_Enclosing,
  Intrv_JustOverlappingAtStart,
  Intrv_Similar,
  Intrv_JustEnclosingAtStart,
  Intrv_Inside,
  Intrv_JustOverlappingAtEnd,
  Intrv_OverlappingAtEnd,
  Intrv_JustAfter,
  Intrv_After
};

#endif // _Intrv_Position_HeaderFile
