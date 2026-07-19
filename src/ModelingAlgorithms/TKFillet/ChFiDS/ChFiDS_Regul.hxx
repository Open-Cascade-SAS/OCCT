// Created on: 1995-03-21
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _ChFiDS_Regul_HeaderFile
#define _ChFiDS_Regul_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>

//! Storage of a curve and its 2 faces or surfaces of support.
class ChFiDS_Regul
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ChFiDS_Regul();

  Standard_EXPORT void SetCurve(const int IC);

  Standard_EXPORT void SetS1(const int IS1, const bool IsFace = true);

  Standard_EXPORT void SetS2(const int IS2, const bool IsFace = true);

  Standard_EXPORT bool IsSurface1() const;

  Standard_EXPORT bool IsSurface2() const;

  Standard_EXPORT int Curve() const;

  Standard_EXPORT int S1() const;

  Standard_EXPORT int S2() const;

private:
  int icurv;
  int is1;
  int is2;
};

#endif // _ChFiDS_Regul_HeaderFile
