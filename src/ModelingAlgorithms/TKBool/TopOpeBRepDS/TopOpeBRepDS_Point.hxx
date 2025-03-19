// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_Point_HeaderFile
#define _TopOpeBRepDS_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
class TopoDS_Shape;

//! A Geom point and a tolerance.
class TopOpeBRepDS_Point
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_Point();

  Standard_EXPORT TopOpeBRepDS_Point(const gp_Pnt& P, const Standard_Real T);

  Standard_EXPORT TopOpeBRepDS_Point(const TopoDS_Shape& S);

  Standard_EXPORT Standard_Boolean IsEqual(const TopOpeBRepDS_Point& other) const;

  Standard_EXPORT const gp_Pnt& Point() const;

  Standard_EXPORT gp_Pnt& ChangePoint();

  Standard_EXPORT Standard_Real Tolerance() const;

  Standard_EXPORT void Tolerance(const Standard_Real Tol);

  Standard_EXPORT Standard_Boolean Keep() const;

  Standard_EXPORT void ChangeKeep(const Standard_Boolean B);

protected:
private:
  gp_Pnt           myPoint;
  Standard_Real    myTolerance;
  Standard_Boolean myKeep;
};

#endif // _TopOpeBRepDS_Point_HeaderFile
