// Created on: 2016-10-14
// Created by: Aleksandr Bobkov
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

#ifndef _HLRAlgo_PolyMask_HeaderFile
#define _HLRAlgo_PolyMask_HeaderFile

enum HLRAlgo_PolyMask
{
  HLRAlgo_PolyMask_EMskOutLin1 =    1,
  HLRAlgo_PolyMask_EMskOutLin2 =    2,
  HLRAlgo_PolyMask_EMskOutLin3 =    4,
  HLRAlgo_PolyMask_EMskGrALin1 =    8,
  HLRAlgo_PolyMask_EMskGrALin2 =   16,
  HLRAlgo_PolyMask_EMskGrALin3 =   32,
  HLRAlgo_PolyMask_FMskBack    =   64,
  HLRAlgo_PolyMask_FMskSide    =  128,
  HLRAlgo_PolyMask_FMskHiding  =  256,
  HLRAlgo_PolyMask_FMskFlat    =  512,
  HLRAlgo_PolyMask_FMskOnOutL  = 1024,
  HLRAlgo_PolyMask_FMskOrBack  = 2048,
  HLRAlgo_PolyMask_FMskFrBack  = 4096
};

#endif // _HLRAlgo_PolyData_HeaderFile
