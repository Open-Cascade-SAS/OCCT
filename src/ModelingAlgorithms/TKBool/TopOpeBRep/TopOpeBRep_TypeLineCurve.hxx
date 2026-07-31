// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRep_TypeLineCurve_HeaderFile
#define _TopOpeBRep_TypeLineCurve_HeaderFile

enum TopOpeBRep_TypeLineCurve
{
  TopOpeBRep_ANALYTIC,
  TopOpeBRep_RESTRICTION,
  TopOpeBRep_WALKING,
  TopOpeBRep_LINE,
  TopOpeBRep_CIRCLE,
  TopOpeBRep_ELLIPSE,
  TopOpeBRep_PARABOLA,
  TopOpeBRep_HYPERBOLA,
  TopOpeBRep_OTHERTYPE
};

#endif // _TopOpeBRep_TypeLineCurve_HeaderFile
