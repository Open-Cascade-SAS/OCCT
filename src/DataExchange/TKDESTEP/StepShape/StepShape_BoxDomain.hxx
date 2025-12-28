// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepShape_BoxDomain_HeaderFile
#define _StepShape_BoxDomain_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepGeom_CartesianPoint;

class StepShape_BoxDomain : public Standard_Transient
{

public:
  //! Returns a BoxDomain
  Standard_EXPORT StepShape_BoxDomain();

  Standard_EXPORT void Init(const occ::handle<StepGeom_CartesianPoint>& aCorner,
                            const double                    aXlength,
                            const double                    aYlength,
                            const double                    aZlength);

  Standard_EXPORT void SetCorner(const occ::handle<StepGeom_CartesianPoint>& aCorner);

  Standard_EXPORT occ::handle<StepGeom_CartesianPoint> Corner() const;

  Standard_EXPORT void SetXlength(const double aXlength);

  Standard_EXPORT double Xlength() const;

  Standard_EXPORT void SetYlength(const double aYlength);

  Standard_EXPORT double Ylength() const;

  Standard_EXPORT void SetZlength(const double aZlength);

  Standard_EXPORT double Zlength() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_BoxDomain, Standard_Transient)

private:
  occ::handle<StepGeom_CartesianPoint> corner;
  double                   xlength;
  double                   ylength;
  double                   zlength;
};

#endif // _StepShape_BoxDomain_HeaderFile
