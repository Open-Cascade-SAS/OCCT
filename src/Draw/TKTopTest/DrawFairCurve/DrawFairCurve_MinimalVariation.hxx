// Created on: 1996-04-12
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _DrawFairCurve_MinimalVariation_HeaderFile
#define _DrawFairCurve_MinimalVariation_HeaderFile

#include <Standard.hxx>

#include <DrawFairCurve_Batten.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

//! Interactive Draw object of type "MVC"
class DrawFairCurve_MinimalVariation : public DrawFairCurve_Batten
{

public:
  Standard_EXPORT DrawFairCurve_MinimalVariation(void* const TheMVC);

  Standard_EXPORT void SetCurvature(const int Side, const double Rho);

  Standard_EXPORT void SetPhysicalRatio(const double Ratio);

  Standard_EXPORT double GetCurvature(const int Side) const;

  Standard_EXPORT double GetPhysicalRatio() const;

  Standard_EXPORT void FreeCurvature(const int Side);

  DEFINE_STANDARD_RTTIEXT(DrawFairCurve_MinimalVariation, DrawFairCurve_Batten)
};

#endif // _DrawFairCurve_MinimalVariation_HeaderFile
