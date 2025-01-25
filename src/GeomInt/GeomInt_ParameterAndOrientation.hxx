// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_ParameterAndOrientation_HeaderFile
#define _GeomInt_ParameterAndOrientation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>

class GeomInt_ParameterAndOrientation
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomInt_ParameterAndOrientation();

  Standard_EXPORT GeomInt_ParameterAndOrientation(const Standard_Real      P,
                                                  const TopAbs_Orientation Or1,
                                                  const TopAbs_Orientation Or2);

  Standard_EXPORT void SetOrientation1(const TopAbs_Orientation Or);

  Standard_EXPORT void SetOrientation2(const TopAbs_Orientation Or);

  Standard_EXPORT Standard_Real Parameter() const;

  Standard_EXPORT TopAbs_Orientation Orientation1() const;

  Standard_EXPORT TopAbs_Orientation Orientation2() const;

protected:
private:
  Standard_Real      prm;
  TopAbs_Orientation or1;
  TopAbs_Orientation or2;
};

#endif // _GeomInt_ParameterAndOrientation_HeaderFile
