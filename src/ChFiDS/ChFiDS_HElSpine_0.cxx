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

#include <ChFiDS_HElSpine.hxx>

#include <Standard_Type.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <Adaptor3d_Curve.hxx>

 








#define TheCurve ChFiDS_ElSpine
#define TheCurve_hxx <ChFiDS_ElSpine.hxx>
#define Adaptor3d_GenHCurve ChFiDS_HElSpine
#define Adaptor3d_GenHCurve_hxx <ChFiDS_HElSpine.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(ChFiDS_HElSpine)
#include <Adaptor3d_GenHCurve.gxx>

