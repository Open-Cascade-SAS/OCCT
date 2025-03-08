// Created on: 1994-04-01
// Created by: Yves FRICAUD
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

#ifndef _Bisector_PolyBis_HeaderFile
#define _Bisector_PolyBis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bisector_PointOnBis.hxx>
#include <Standard_Boolean.hxx>
class gp_Trsf2d;

//! Polygon of PointOnBis
class Bisector_PolyBis
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Bisector_PolyBis();

  Standard_EXPORT void Append(const Bisector_PointOnBis& Point);

  Standard_EXPORT Standard_Integer Length() const;

  Standard_EXPORT Standard_Boolean IsEmpty() const;

  Standard_EXPORT const Bisector_PointOnBis& Value(const Standard_Integer Index) const;

  Standard_EXPORT const Bisector_PointOnBis& First() const;

  Standard_EXPORT const Bisector_PointOnBis& Last() const;

  Standard_EXPORT Standard_Integer Interval(const Standard_Real U) const;

  Standard_EXPORT void Transform(const gp_Trsf2d& T);

protected:
private:
  Bisector_PointOnBis thePoints[30];
  Standard_Integer    nbPoints;
};

#endif // _Bisector_PolyBis_HeaderFile
