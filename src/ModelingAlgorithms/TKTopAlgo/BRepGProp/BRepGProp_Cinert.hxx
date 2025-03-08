// Created on: 1991-04-11
// Created by: Michel CHAUVAT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _BRepGProp_Cinert_HeaderFile
#define _BRepGProp_Cinert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GProp_GProps.hxx>
class BRepAdaptor_Curve;
class gp_Pnt;

//! Computes the  global properties of bounded curves
//! in 3D space. The curve must have at least a continuity C1.
//! It can be a curve as defined in the template CurveTool from
//! package GProp. This template gives the minimum of methods
//! required to evaluate the global properties of a curve 3D with
//! the algorithms of GProp.
class BRepGProp_Cinert : public GProp_GProps
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepGProp_Cinert();

  Standard_EXPORT BRepGProp_Cinert(const BRepAdaptor_Curve& C, const gp_Pnt& CLocation);

  Standard_EXPORT void SetLocation(const gp_Pnt& CLocation);

  Standard_EXPORT void Perform(const BRepAdaptor_Curve& C);

protected:
private:
};

#endif // _BRepGProp_Cinert_HeaderFile
