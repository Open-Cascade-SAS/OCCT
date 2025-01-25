// Created on: 1993-11-17
// Created by: Isabelle GRIGNON
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

#ifndef _ChFiDS_State_HeaderFile
#define _ChFiDS_State_HeaderFile

//! This enum describe the  different kinds of extremities
//! of   a   fillet.   OnSame,   Ondiff and  AllSame   are
//! particular cases of BreakPoint   for a corner   with 3
//! edges and three faces :
//! - AllSame means that  the three concavities are on the
//! same side of the Shape,
//! - OnDiff  means  that the  edge of  the  fillet  has a
//! concave side different than the two other edges,
//! - OnSame  means  that the  edge of  the  fillet  has a
//! concave side different than one of the two other edges
//! and identical to the third edge.
enum ChFiDS_State
{
  ChFiDS_OnSame,
  ChFiDS_OnDiff,
  ChFiDS_AllSame,
  ChFiDS_BreakPoint,
  ChFiDS_FreeBoundary,
  ChFiDS_Closed,
  ChFiDS_Tangent
};

#endif // _ChFiDS_State_HeaderFile
