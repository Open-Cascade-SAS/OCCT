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

//=================================================================================================

GeomFill_SectionGenerator::GeomFill_SectionGenerator()
{
  if (mySequence.Length() > 1)
  {
    occ::handle<NCollection_HArray1<double>> HPar =
      new (NCollection_HArray1<double>)(1, mySequence.Length());
    for (int i = 1; i <= mySequence.Length(); i++)
    {
      HPar->ChangeValue(i) = i - 1;
    }
    SetParam(HPar);
  }
}

//=================================================================================================

void GeomFill_SectionGenerator::SetParam(const occ::handle<NCollection_HArray1<double>>& Params)
{
  int ii, L = Params->Upper() - Params->Lower() + 1;
  myParams = Params;
  for (ii = 1; ii <= L; ii++)
  {
    myParams->SetValue(ii, Params->Value(Params->Lower() + ii - 1));
  }
}

//=================================================================================================

void GeomFill_SectionGenerator::GetShape(int& NbPoles,
                                         int& NbKnots,
                                         int& Degree,
                                         int& NbPoles2d) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(1));
  NbPoles                          = C->NbPoles();
  NbKnots                          = C->NbKnots();
  Degree                           = C->Degree();
  NbPoles2d                        = 0;
}

//=================================================================================================

void GeomFill_SectionGenerator::Knots(NCollection_Array1<double>& TKnots) const
{
  TKnots = (occ::down_cast<Geom_BSplineCurve>(mySequence(1)))->Knots();
}

//=================================================================================================

void GeomFill_SectionGenerator::Mults(NCollection_Array1<int>& TMults) const
{
  TMults = (occ::down_cast<Geom_BSplineCurve>(mySequence(1)))->Multiplicities();
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
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(P));

  Poles                                   = C->Poles();
  const NCollection_Array1<double>* aWPtr = C->Weights();
  if (aWPtr != nullptr)
    Weigths = *aWPtr;
  else
    Weigths.Init(1.0);
}

//=================================================================================================

double GeomFill_SectionGenerator::Parameter(const int P) const
{
  return myParams->Value(P);
}
