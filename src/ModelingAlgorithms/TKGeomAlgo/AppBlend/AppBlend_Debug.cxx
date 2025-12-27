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

static bool AppBlend_ContextSplineApprox = false;

Standard_EXPORT void AppBlend_SetContextSplineApprox(const bool b)
{
  AppBlend_ContextSplineApprox = b;
}

Standard_EXPORT bool AppBlend_GetContextSplineApprox()
{
  return AppBlend_ContextSplineApprox;
}

static bool AppBlend_ContextApproxWithNoTgt = false;

Standard_EXPORT void AppBlend_SetContextApproxWithNoTgt(const bool b)
{
  AppBlend_ContextApproxWithNoTgt = b;
}

Standard_EXPORT bool AppBlend_GetContextApproxWithNoTgt()
{
  return AppBlend_ContextApproxWithNoTgt;
}
