// Created on: 1994-09-02
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

#ifndef _Geom2dLProp_NumericCurInf2d_HeaderFile
#define _Geom2dLProp_NumericCurInf2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom2d_Curve;
class LProp_CurAndInf;

//! Computes the locals extremas of curvature and the
//! inflections of a bounded curve in 2d.
class Geom2dLProp_NumericCurInf2d
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dLProp_NumericCurInf2d();

  //! Computes the locals extremas of curvature.
  Standard_EXPORT void PerformCurExt(const Handle(Geom2d_Curve)& C, LProp_CurAndInf& Result);

  //! Computes the inflections.
  Standard_EXPORT void PerformInf(const Handle(Geom2d_Curve)& C, LProp_CurAndInf& Result);

  //! Computes the locals extremas of curvature.
  //! in the interval of parameters [UMin,UMax].
  Standard_EXPORT void PerformCurExt(const Handle(Geom2d_Curve)& C,
                                     const Standard_Real         UMin,
                                     const Standard_Real         UMax,
                                     LProp_CurAndInf&            Result);

  //! Computes the inflections in the interval of
  //! parameters [UMin,UMax].
  Standard_EXPORT void PerformInf(const Handle(Geom2d_Curve)& C,
                                  const Standard_Real         UMin,
                                  const Standard_Real         UMax,
                                  LProp_CurAndInf&            Result);

  //! True if the solutions are found.
  Standard_EXPORT Standard_Boolean IsDone() const;

protected:
private:
  Standard_Boolean isDone;
};

#endif // _Geom2dLProp_NumericCurInf2d_HeaderFile
