// Created on: 2005-12-21
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _math_ComputeKronrodPointsAndWeights_HeaderFile
#define _math_ComputeKronrodPointsAndWeights_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>

class math_ComputeKronrodPointsAndWeights
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT math_ComputeKronrodPointsAndWeights(const int Number);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT math_Vector Points() const;

  Standard_EXPORT math_Vector Weights() const;

private:
  occ::handle<NCollection_HArray1<double>> myPoints;
  occ::handle<NCollection_HArray1<double>> myWeights;
  bool              myIsDone;
};

#endif // _math_ComputeKronrodPointsAndWeights_HeaderFile
