// Created on: 2016-02-26
// Created by: Julia GERASIMOVA
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

#ifndef _HLRBRep_TypeOfResultingEdge_HeaderFile
#define _HLRBRep_TypeOfResultingEdge_HeaderFile


//! Identifies the type of resulting edge of HLRBRep_Algo
enum HLRBRep_TypeOfResultingEdge
{
HLRBRep_Undefined,

//! isoparametric line
HLRBRep_IsoLine,

//! outline ("silhouette")
HLRBRep_OutLine,

//! smooth edge of G1-continuity between two surfaces
HLRBRep_Rg1Line,

//! sewn edge of CN-continuity on one surface
HLRBRep_RgNLine,

//! sharp edge (of C0-continuity)
HLRBRep_Sharp
};

#endif // _HLRBRep_TypeOfResultingEdge_HeaderFile
