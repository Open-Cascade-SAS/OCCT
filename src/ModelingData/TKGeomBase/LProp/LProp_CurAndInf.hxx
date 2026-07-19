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

#ifndef _LProp_CurAndInf_HeaderFile
#define _LProp_CurAndInf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Sequence.hxx>
#include <LProp_CIType.hxx>
#include <Standard_Boolean.hxx>

//! Stores the parameters of a curve 2d or 3d corresponding
//! to the curvature's extremas and the Inflection's Points.
class LProp_CurAndInf
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT LProp_CurAndInf();

  Standard_EXPORT void AddInflection(const double Param);

  Standard_EXPORT void AddExtCur(const double Param, const bool IsMin);

  Standard_EXPORT void Clear();

  Standard_EXPORT bool IsEmpty() const;

  //! Returns the number of points.
  //! The Points are stored to increasing parameter.
  Standard_EXPORT int NbPoints() const;

  //! Returns the parameter of the Nth point.
  //! raises if N not in the range [1,NbPoints()]
  Standard_EXPORT double Parameter(const int N) const;

  //! Returns
  //! - MinCur if the Nth parameter corresponds to
  //! a minimum of the radius of curvature.
  //! - MaxCur if the Nth parameter corresponds to
  //! a maximum of the radius of curvature.
  //! - Inflection if the parameter corresponds to
  //! a point of inflection.
  //! raises if N not in the range [1,NbPoints()]
  Standard_EXPORT LProp_CIType Type(const int N) const;

private:
  NCollection_Sequence<double>       theParams;
  NCollection_Sequence<LProp_CIType> theTypes;
};

#endif // _LProp_CurAndInf_HeaderFile
