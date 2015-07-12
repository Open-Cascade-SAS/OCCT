// Created on: 1996-12-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BiTgte_HCurveOnVertex.hxx>

#include <Standard_Type.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>
#include <BiTgte_CurveOnVertex.hxx>
#include <Adaptor3d_Curve.hxx>

 








#define TheCurve BiTgte_CurveOnVertex
#define TheCurve_hxx <BiTgte_CurveOnVertex.hxx>
#define Adaptor3d_GenHCurve BiTgte_HCurveOnVertex
#define Adaptor3d_GenHCurve_hxx <BiTgte_HCurveOnVertex.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(BiTgte_HCurveOnVertex)
#include <Adaptor3d_GenHCurve.gxx>

