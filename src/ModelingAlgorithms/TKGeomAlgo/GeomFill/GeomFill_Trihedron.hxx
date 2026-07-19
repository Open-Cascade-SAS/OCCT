// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_Trihedron_HeaderFile
#define _GeomFill_Trihedron_HeaderFile

enum GeomFill_Trihedron
{
  GeomFill_IsCorrectedFrenet,
  GeomFill_IsFixed,
  GeomFill_IsFrenet,
  GeomFill_IsConstantNormal,
  GeomFill_IsDarboux,
  GeomFill_IsGuideAC,
  GeomFill_IsGuidePlan,
  GeomFill_IsGuideACWithContact,
  GeomFill_IsGuidePlanWithContact,
  GeomFill_IsDiscreteTrihedron
};

#endif // _GeomFill_Trihedron_HeaderFile
