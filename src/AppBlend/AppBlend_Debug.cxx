// Created on: 1994-10-18
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Standard_Boolean.hxx>

//****************************************************
// Functions to change the type of approx.
//****************************************************

static Standard_Boolean AppBlend_ContextSplineApprox = Standard_False;
Standard_EXPORT void AppBlend_SetContextSplineApprox(const Standard_Boolean b) 
{ AppBlend_ContextSplineApprox = b; }
Standard_EXPORT Standard_Boolean AppBlend_GetContextSplineApprox() 
{ return AppBlend_ContextSplineApprox; }

static Standard_Boolean AppBlend_ContextApproxWithNoTgt = Standard_False;
Standard_EXPORT void AppBlend_SetContextApproxWithNoTgt(const Standard_Boolean b) 
{ AppBlend_ContextApproxWithNoTgt = b; }
Standard_EXPORT Standard_Boolean AppBlend_GetContextApproxWithNoTgt() 
{ return AppBlend_ContextApproxWithNoTgt; }
