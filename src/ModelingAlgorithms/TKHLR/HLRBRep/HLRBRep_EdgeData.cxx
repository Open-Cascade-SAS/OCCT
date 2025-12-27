// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <TopoDS_Edge.hxx>

//=================================================================================================

void HLRBRep_EdgeData::Set(const bool   Rg1L,
                           const bool   RgNL,
                           const TopoDS_Edge&       EG,
                           const int   V1,
                           const int   V2,
                           const bool   Out1,
                           const bool   Out2,
                           const bool   Cut1,
                           const bool   Cut2,
                           const double      Start,
                           const float TolStart,
                           const double      End,
                           const float TolEnd)
{
  Rg1Line(Rg1L);
  RgNLine(RgNL);
  Used(false);
  ChangeGeometry().Curve(EG);
  myTolerance = (float)(BRep_Tool::Tolerance(EG));
  VSta(V1);
  VEnd(V2);
  OutLVSta(Out1);
  OutLVEnd(Out2);
  CutAtSta(Cut1);
  CutAtEnd(Cut2);
  Status().Initialize(
    Start,
    (float)(ChangeGeometry().Curve().Resolution((double)TolStart)),
    End,
    (float)(ChangeGeometry().Curve().Resolution((double)TolEnd)));
}
