// Created on: 2016-11-25
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

#ifndef _PrsDim_KindOfRelation_HeaderFile
#define _PrsDim_KindOfRelation_HeaderFile

enum PrsDim_KindOfRelation
{
  PrsDim_KOR_NONE = 0,
  PrsDim_KOR_CONCENTRIC,
  PrsDim_KOR_EQUALDISTANCE,
  PrsDim_KOR_EQUALRADIUS,
  PrsDim_KOR_FIX,
  PrsDim_KOR_IDENTIC,
  PrsDim_KOR_OFFSET,
  PrsDim_KOR_PARALLEL,
  PrsDim_KOR_PERPENDICULAR,
  PrsDim_KOR_TANGENT,
  PrsDim_KOR_SYMMETRIC
};

#endif
