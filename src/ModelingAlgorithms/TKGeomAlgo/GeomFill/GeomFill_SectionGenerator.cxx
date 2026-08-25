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
#include <gp.hxx>
#include <Standard_ConstructionError.hxx>

//=================================================================================================

GeomFill_SectionGenerator::GeomFill_SectionGenerator()
    : myNbSections(0),
      myFirstTangentSection(0),
      myLastTangentSection(0),
      myFirstCurvatureSection(0),
      myLastCurvatureSection(0),
      myHasFirstTangent(false),
      myHasLastTangent(false),
      myHasFirstCurvature(false),
      myHasLastCurvature(false)
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

void GeomFill_SectionGenerator::AddCurve(const occ::handle<Geom_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: null section curve");
  }
  if (myIsDone || myNbSections != 0)
  {
    throw Standard_ConstructionError(
      "GeomFill_SectionGenerator: section curves must precede endpoint constraints");
  }
  GeomFill_Profiler::AddCurve(theCurve);
}

//=================================================================================================

void GeomFill_SectionGenerator::Perform(const double thePTol)
{
  if (myNbSections != 0)
  {
    const int aNbAuxiliaryCurves = (myFirstTangentSection != 0) + (myLastTangentSection != 0)
                                   + (myFirstCurvatureSection != 0) + (myLastCurvatureSection != 0);
    if (mySequence.Length() != myNbSections + aNbAuxiliaryCurves)
    {
      throw Standard_ConstructionError(
        "GeomFill_SectionGenerator: section curves must precede endpoint constraints");
    }
  }
  GeomFill_Profiler::Perform(thePTol);
}

//=================================================================================================

void GeomFill_SectionGenerator::BeginConstraintConfiguration()
{
  if (myIsDone)
  {
    throw Standard_ConstructionError(
      "GeomFill_SectionGenerator: endpoint constraints cannot follow Perform");
  }
  if (myNbSections == 0)
  {
    if (mySequence.Length() == 0)
    {
      throw Standard_ConstructionError(
        "GeomFill_SectionGenerator: endpoint constraints require section curves");
    }
    myNbSections = mySequence.Length();
  }
}

//=================================================================================================

void GeomFill_SectionGenerator::RemoveAuxiliaryCurve(int& theIndex)
{
  if (theIndex == 0)
  {
    return;
  }

  const int anIndex = theIndex;
  theIndex          = 0;
  mySequence.Remove(anIndex);
  const auto adjustIndex = [anIndex](int& theOtherIndex) {
    if (theOtherIndex > anIndex)
    {
      --theOtherIndex;
    }
  };
  adjustIndex(myFirstTangentSection);
  adjustIndex(myLastTangentSection);
  adjustIndex(myFirstCurvatureSection);
  adjustIndex(myLastCurvatureSection);

  myIsPeriodic = true;
  for (int anOtherIndex = 1; anOtherIndex <= mySequence.Length(); ++anOtherIndex)
  {
    if (!mySequence(anOtherIndex)->IsPeriodic())
    {
      myIsPeriodic = false;
      break;
    }
  }
}

//=================================================================================================

void GeomFill_SectionGenerator::ResetFirstCurvature()
{
  RemoveAuxiliaryCurve(myFirstCurvatureSection);
  myFirstCurvature.SetCoord(0.0, 0.0, 0.0);
  myHasFirstCurvature = false;
}

//=================================================================================================

void GeomFill_SectionGenerator::ResetLastCurvature()
{
  RemoveAuxiliaryCurve(myLastCurvatureSection);
  myLastCurvature.SetCoord(0.0, 0.0, 0.0);
  myHasLastCurvature = false;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetFirstTangent(const gp_Vec& theTangent)
{
  BeginConstraintConfiguration();
  if (theTangent.Magnitude() <= gp::Resolution())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: null first tangent");
  }
  ResetFirstCurvature();
  RemoveAuxiliaryCurve(myFirstTangentSection);
  myFirstTangent    = theTangent;
  myHasFirstTangent = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetLastTangent(const gp_Vec& theTangent)
{
  BeginConstraintConfiguration();
  if (theTangent.Magnitude() <= gp::Resolution())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: null last tangent");
  }
  ResetLastCurvature();
  RemoveAuxiliaryCurve(myLastTangentSection);
  myLastTangent    = theTangent;
  myHasLastTangent = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetFirstTangentSection(
  const occ::handle<Geom_Curve>& theTangentSection)
{
  BeginConstraintConfiguration();
  if (theTangentSection.IsNull())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: null first tangent section");
  }
  ResetFirstCurvature();
  RemoveAuxiliaryCurve(myFirstTangentSection);
  GeomFill_Profiler::AddCurve(theTangentSection);
  myFirstTangentSection = mySequence.Length();
  myHasFirstTangent     = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetLastTangentSection(
  const occ::handle<Geom_Curve>& theTangentSection)
{
  BeginConstraintConfiguration();
  if (theTangentSection.IsNull())
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: null last tangent section");
  }
  ResetLastCurvature();
  RemoveAuxiliaryCurve(myLastTangentSection);
  GeomFill_Profiler::AddCurve(theTangentSection);
  myLastTangentSection = mySequence.Length();
  myHasLastTangent     = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetFirstCurvature(const gp_Vec& theCurvature)
{
  BeginConstraintConfiguration();
  if (!myHasFirstTangent)
  {
    throw Standard_ConstructionError(
      "GeomFill_SectionGenerator: first curvature requires a tangent constraint");
  }
  ResetFirstCurvature();
  myFirstCurvature    = theCurvature;
  myHasFirstCurvature = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetLastCurvature(const gp_Vec& theCurvature)
{
  BeginConstraintConfiguration();
  if (!myHasLastTangent)
  {
    throw Standard_ConstructionError(
      "GeomFill_SectionGenerator: last curvature requires a tangent constraint");
  }
  ResetLastCurvature();
  myLastCurvature    = theCurvature;
  myHasLastCurvature = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetFirstCurvatureSection(
  const occ::handle<Geom_Curve>& theCurvatureSection)
{
  BeginConstraintConfiguration();
  if (theCurvatureSection.IsNull() || myFirstTangentSection == 0)
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: invalid first curvature section");
  }
  ResetFirstCurvature();
  GeomFill_Profiler::AddCurve(theCurvatureSection);
  myFirstCurvatureSection = mySequence.Length();
  myHasFirstCurvature     = true;
}

//=================================================================================================

void GeomFill_SectionGenerator::SetLastCurvatureSection(
  const occ::handle<Geom_Curve>& theCurvatureSection)
{
  BeginConstraintConfiguration();
  if (theCurvatureSection.IsNull() || myLastTangentSection == 0)
  {
    throw Standard_ConstructionError("GeomFill_SectionGenerator: invalid last curvature section");
  }
  ResetLastCurvature();
  GeomFill_Profiler::AddCurve(theCurvatureSection);
  myLastCurvatureSection = mySequence.Length();
  myHasLastCurvature     = true;
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

bool GeomFill_SectionGenerator::Section(const int                     P,
                                        NCollection_Array1<gp_Pnt>&   Poles,
                                        NCollection_Array1<gp_Vec>&   DPoles,
                                        NCollection_Array1<gp_Pnt2d>& Poles2d,
                                        NCollection_Array1<gp_Vec2d>&, // DPoles2d,
                                        NCollection_Array1<double>& Weigths,
                                        NCollection_Array1<double>& DWeigths) const
{
  Section(P, Poles, Poles2d, Weigths);

  const int aNbSections     = myNbSections > 0 ? myNbSections : mySequence.Length();
  int       aTangentSection = 0;
  gp_Vec    aTangent;
  if (P == 1 && myHasFirstTangent)
  {
    aTangentSection = myFirstTangentSection;
    aTangent        = myFirstTangent;
  }
  else if (P == aNbSections && myHasLastTangent)
  {
    aTangentSection = myLastTangentSection;
    aTangent        = myLastTangent;
  }
  else
  {
    return false;
  }

  if (aTangentSection == 0)
  {
    for (int anIndex = DPoles.Lower(); anIndex <= DPoles.Upper(); ++anIndex)
    {
      DPoles(anIndex)   = aTangent;
      DWeigths(anIndex) = 0.0;
    }
    return true;
  }

  const occ::handle<Geom_BSplineCurve> aSection = occ::down_cast<Geom_BSplineCurve>(mySequence(P));
  const occ::handle<Geom_BSplineCurve> aTangentCurve =
    occ::down_cast<Geom_BSplineCurve>(mySequence(aTangentSection));
  if (aSection.IsNull() || aTangentCurve.IsNull()
      || aSection->NbPoles() != aTangentCurve->NbPoles())
  {
    return false;
  }

  for (int anIndex = DPoles.Lower(); anIndex <= DPoles.Upper(); ++anIndex)
  {
    const double aWeight        = aSection->Weight(anIndex);
    const double aTangentWeight = aTangentCurve->Weight(anIndex);
    if (aWeight <= gp::Resolution() || aTangentWeight <= gp::Resolution())
    {
      return false;
    }
    DWeigths(anIndex) = aTangentWeight - aWeight;
    DPoles(anIndex)   = gp_Vec(aSection->Pole(anIndex), aTangentCurve->Pole(anIndex))
                        .Multiplied(aTangentWeight / aWeight);
  }
  return true;
}

//=================================================================================================

bool GeomFill_SectionGenerator::SectionWithCurvature(const int                     P,
                                                     NCollection_Array1<gp_Pnt>&   Poles,
                                                     NCollection_Array1<gp_Vec>&   DPoles,
                                                     NCollection_Array1<gp_Vec>&   D2Poles,
                                                     NCollection_Array1<gp_Pnt2d>& Poles2d,
                                                     NCollection_Array1<gp_Vec2d>& DPoles2d,
                                                     NCollection_Array1<gp_Vec2d>&,
                                                     NCollection_Array1<double>& Weigths,
                                                     NCollection_Array1<double>& DWeigths,
                                                     NCollection_Array1<double>& D2Weigths) const
{
  const int aNbSections       = myNbSections > 0 ? myNbSections : mySequence.Length();
  int       aCurvatureSection = 0;
  gp_Vec    aCurvature;
  if (P == 1 && myHasFirstCurvature)
  {
    aCurvatureSection = myFirstCurvatureSection;
    aCurvature        = myFirstCurvature;
  }
  else if (P == aNbSections && myHasLastCurvature)
  {
    aCurvatureSection = myLastCurvatureSection;
    aCurvature        = myLastCurvature;
  }
  else
  {
    return false;
  }

  if (!Section(P, Poles, DPoles, Poles2d, DPoles2d, Weigths, DWeigths))
  {
    return false;
  }

  if (aCurvatureSection == 0)
  {
    for (int anIndex = D2Poles.Lower(); anIndex <= D2Poles.Upper(); ++anIndex)
    {
      D2Poles(anIndex)   = aCurvature;
      D2Weigths(anIndex) = 0.0;
    }
    return true;
  }

  int aTangentSection = P == 1 ? myFirstTangentSection : myLastTangentSection;
  if (aTangentSection == 0)
  {
    return false;
  }
  const occ::handle<Geom_BSplineCurve> aSection = occ::down_cast<Geom_BSplineCurve>(mySequence(P));
  const occ::handle<Geom_BSplineCurve> aTangentCurve =
    occ::down_cast<Geom_BSplineCurve>(mySequence(aTangentSection));
  const occ::handle<Geom_BSplineCurve> aCurvatureCurve =
    occ::down_cast<Geom_BSplineCurve>(mySequence(aCurvatureSection));
  if (aSection.IsNull() || aTangentCurve.IsNull() || aCurvatureCurve.IsNull()
      || aSection->NbPoles() != aTangentCurve->NbPoles()
      || aSection->NbPoles() != aCurvatureCurve->NbPoles())
  {
    return false;
  }

  for (int anIndex = D2Poles.Lower(); anIndex <= D2Poles.Upper(); ++anIndex)
  {
    const double aWeight          = aSection->Weight(anIndex);
    const double aTangentWeight   = aTangentCurve->Weight(anIndex);
    const double aCurvatureWeight = aCurvatureCurve->Weight(anIndex);
    if (aWeight <= gp::Resolution() || aTangentWeight <= gp::Resolution()
        || aCurvatureWeight <= gp::Resolution())
    {
      return false;
    }

    const gp_XYZ aSecondHomogeneous = aCurvatureCurve->Pole(anIndex).XYZ() * aCurvatureWeight
                                      - 2.0 * aTangentCurve->Pole(anIndex).XYZ() * aTangentWeight
                                      + aSection->Pole(anIndex).XYZ() * aWeight;
    const double aSecondWeight = aCurvatureWeight - 2.0 * aTangentWeight + aWeight;
    D2Weigths(anIndex)         = aSecondWeight;
    D2Poles(anIndex).SetXYZ((aSecondHomogeneous - aSection->Pole(anIndex).XYZ() * aSecondWeight
                             - 2.0 * DPoles(anIndex).XYZ() * DWeigths(anIndex))
                            / aWeight);
  }
  return true;
}

//=================================================================================================

void GeomFill_SectionGenerator::Section(const int                   P,
                                        NCollection_Array1<gp_Pnt>& Poles,
                                        NCollection_Array1<gp_Pnt2d>&, // Poles2d,
                                        NCollection_Array1<double>& Weigths) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(P));

  Poles   = C->Poles();
  Weigths = C->WeightsArray();
}

//=================================================================================================

double GeomFill_SectionGenerator::Parameter(const int P) const
{
  return myParams->Value(P);
}
