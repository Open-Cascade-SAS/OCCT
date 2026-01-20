// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_Filling_HeaderFile
#define _GeomFill_Filling_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array2.hxx>

//! Root class for Filling;
class GeomFill_Filling
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_Filling();

  Standard_EXPORT int NbUPoles() const;

  Standard_EXPORT int NbVPoles() const;

  Standard_EXPORT void Poles(NCollection_Array2<gp_Pnt>& Poles) const;

  Standard_EXPORT bool isRational() const;

  Standard_EXPORT void Weights(NCollection_Array2<double>& Weights) const;

protected:
  bool              IsRational;
  occ::handle<NCollection_HArray2<gp_Pnt>>   myPoles;
  occ::handle<NCollection_HArray2<double>> myWeights;

};

#endif // _GeomFill_Filling_HeaderFile
