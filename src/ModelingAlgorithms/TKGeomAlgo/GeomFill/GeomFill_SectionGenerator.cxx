// Created on: 1994-02-18
// Created by: Bruno DUMORTIER
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

#include <Geom_BSplineCurve.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <cmath>

//==================================================================================================

GeomFill_SectionGenerator::GeomFill_SectionGenerator() = default;

//==================================================================================================

void GeomFill_SectionGenerator::SetParam(const occ::handle<NCollection_HArray1<double>>& Params)
{
  if (Params.IsNull())
  {
    throw Standard_NullObject("GeomFill_SectionGenerator: null parameters");
  }
  if (Params->IsEmpty())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: empty parameters");
  }
  for (int anIndex = Params->Lower(); anIndex <= Params->Upper(); ++anIndex)
  {
    if (!std::isfinite(Params->Value(anIndex))
        || (anIndex > Params->Lower() && Params->Value(anIndex) <= Params->Value(anIndex - 1)))
    {
      throw Standard_ConstructionError("GeomFill_SectionGenerator: invalid parameter sequence");
    }
  }
  // Keep section indices one-based without modifying or retaining the caller's array.
  occ::handle<NCollection_HArray1<double>> aParams =
    new NCollection_HArray1<double>(1, Params->Length());
  for (int anIndex = 1; anIndex <= Params->Length(); ++anIndex)
  {
    aParams->SetValue(anIndex, Params->Value(Params->Lower() + anIndex - 1));
  }
  // Publish only after validation and copying succeed: a rejected replacement
  // leaves the previously accepted section parameterization intact.
  myParams = aParams;
}

//=================================================================================================

void GeomFill_SectionGenerator::GetShape(int& NbPoles,
                                         int& NbKnots,
                                         int& Degree,
                                         int& NbPoles2d) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(myCurves.First());
  NbPoles                          = C->NbPoles();
  NbKnots                          = C->NbKnots();
  Degree                           = C->Degree();
  NbPoles2d                        = 0;
}

//=================================================================================================

void GeomFill_SectionGenerator::Knots(NCollection_Array1<double>& TKnots) const
{
  TKnots = (occ::down_cast<Geom_BSplineCurve>(myCurves.First()))->Knots();
}

//=================================================================================================

void GeomFill_SectionGenerator::Mults(NCollection_Array1<int>& TMults) const
{
  TMults = (occ::down_cast<Geom_BSplineCurve>(myCurves.First()))->Multiplicities();
}

//=================================================================================================

bool GeomFill_SectionGenerator::Section(const int                   P,
                                        NCollection_Array1<gp_Pnt>& Poles,
                                        NCollection_Array1<gp_Vec>&, // DPoles,
                                        NCollection_Array1<gp_Pnt2d>& Poles2d,
                                        NCollection_Array1<gp_Vec2d>&, // DPoles2d,
                                        NCollection_Array1<double>& Weigths,
                                        NCollection_Array1<double>& // DWeigths
) const
{
  Section(P, Poles, Poles2d, Weigths);
  return false;
}

//=================================================================================================

void GeomFill_SectionGenerator::Section(const int                   P,
                                        NCollection_Array1<gp_Pnt>& Poles,
                                        NCollection_Array1<gp_Pnt2d>&, // Poles2d,
                                        NCollection_Array1<double>& Weigths) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(Curve(P));

  Poles   = C->Poles();
  Weigths = C->WeightsArray();
}

//=================================================================================================

double GeomFill_SectionGenerator::Parameter(const int P) const
{
  return myParams->Value(P);
}
