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

#ifndef _AIS_KindOfRelation_HeaderFile
#define _AIS_KindOfRelation_HeaderFile

enum AIS_KindOfRelation
{
  AIS_KOR_NONE = 0,
  AIS_KOR_CONCENTRIC,
  AIS_KOR_EQUALDISTANCE,
  AIS_KOR_EQUALRADIUS,
  AIS_KOR_FIX,
  AIS_KOR_IDENTIC,
  AIS_KOR_OFFSET,
  AIS_KOR_PARALLEL,
  AIS_KOR_PERPENDICULAR,
  AIS_KOR_TANGENT,
  AIS_KOR_SYMMETRIC
};

#endif
