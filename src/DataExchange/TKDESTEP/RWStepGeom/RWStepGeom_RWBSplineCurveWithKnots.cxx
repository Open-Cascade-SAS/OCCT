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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include "RWStepGeom_RWBSplineCurveWithKnots.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_KnotType.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>

#include "RWStepGeom_RWBSplineCurveForm.pxx"
#include "RWStepGeom_RWKnotType.pxx"

RWStepGeom_RWBSplineCurveWithKnots::RWStepGeom_RWBSplineCurveWithKnots() = default;

void RWStepGeom_RWBSplineCurveWithKnots::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepGeom_BSplineCurveWithKnots>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 9, ach, "b_spline_curve_with_knots"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : degree ---

  int aDegree;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadInteger(num, 2, "degree", ach, aDegree);

  // --- inherited field : controlPointsList ---

  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aControlPointsList;
  occ::handle<StepGeom_CartesianPoint>                                   anent3;
  int                                                                    nsub3;
  if (data->ReadSubList(num, 3, "control_points_list", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    if (nb3 < 1)
      ach->AddFail("Number of control points of the b_spline_curve_form is equal to 0");
    else
    {
      aControlPointsList = new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, nb3);
      for (int i3 = 1; i3 <= nb3; i3++)
      {
        // szv#4:S4163:12Mar99 `bool stat3 =` not needed
        if (data->ReadEntity(nsub3,
                             i3,
                             "cartesian_point",
                             ach,
                             STANDARD_TYPE(StepGeom_CartesianPoint),
                             anent3))
          aControlPointsList->SetValue(i3, anent3);
      }
    }
  }

  // --- inherited field : curveForm ---

  StepGeom_BSplineCurveForm aCurveForm = StepGeom_bscfPolylineForm;
  if (data->ParamType(num, 4) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 4);
    if (!RWStepGeom_RWBSplineCurveForm::ConvertToEnum(text, aCurveForm))
    {
      ach->AddFail("Enumeration b_spline_curve_form has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #4 (curve_form) is not an enumeration");

  // --- inherited field : closedCurve ---

  StepData_Logical aClosedCurve;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadLogical(num, 5, "closed_curve", ach, aClosedCurve);

  // --- inherited field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat6 =` not needed
  data->ReadLogical(num, 6, "self_intersect", ach, aSelfIntersect);

  // --- own field : knotMultiplicities ---

  occ::handle<NCollection_HArray1<int>> aKnotMultiplicities;
  int                                   aKnotMultiplicitiesItem;
  int                                   nsub7;
  if (data->ReadSubList(num, 7, "knot_multiplicities", ach, nsub7))
  {
    int nb7             = data->NbParams(nsub7);
    aKnotMultiplicities = new NCollection_HArray1<int>(1, nb7);
    for (int i7 = 1; i7 <= nb7; i7++)
    {
      // szv#4:S4163:12Mar99 `bool stat7 =` not needed
      if (data->ReadInteger(nsub7, i7, "knot_multiplicities", ach, aKnotMultiplicitiesItem))
        aKnotMultiplicities->SetValue(i7, aKnotMultiplicitiesItem);
    }
  }

  // --- own field : knots ---

  occ::handle<NCollection_HArray1<double>> aKnots;
  double                                   aKnotsItem;
  int                                      nsub8;
  if (data->ReadSubList(num, 8, "knots", ach, nsub8))
  {
    int nb8 = data->NbParams(nsub8);
    aKnots  = new NCollection_HArray1<double>(1, nb8);
    for (int i8 = 1; i8 <= nb8; i8++)
    {
      // szv#4:S4163:12Mar99 `bool stat8 =` not needed
      if (data->ReadReal(nsub8, i8, "knots", ach, aKnotsItem))
        aKnots->SetValue(i8, aKnotsItem);
    }
  }

  // --- own field : knotSpec ---

  StepGeom_KnotType aKnotSpec = StepGeom_ktUniformKnots;
  if (data->ParamType(num, 9) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 9);
    if (!RWStepGeom_RWKnotType::ConvertToEnum(text, aKnotSpec))
    {
      ach->AddFail("Enumeration knot_type has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #9 (knot_spec) is not an enumeration");

  //--- Initialisation of the read entity ---

  ent->Init(aName,
            aDegree,
            aControlPointsList,
            aCurveForm,
            aClosedCurve,
            aSelfIntersect,
            aKnotMultiplicities,
            aKnots,
            aKnotSpec);
}

void RWStepGeom_RWBSplineCurveWithKnots::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepGeom_BSplineCurveWithKnots>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field degree ---

  SW.Send(ent->Degree());

  // --- inherited field controlPointsList ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbControlPointsList(); i3++)
  {
    SW.Send(ent->ControlPointsListValue(i3));
  }
  SW.CloseSub();

  // --- inherited field curveForm ---

  SW.SendEnum(RWStepGeom_RWBSplineCurveForm::ConvertToString(ent->CurveForm()));

  // --- inherited field closedCurve ---

  SW.SendLogical(ent->ClosedCurve());

  // --- inherited field selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());

  // --- own field : knotMultiplicities ---

  SW.OpenSub();
  for (int i7 = 1; i7 <= ent->NbKnotMultiplicities(); i7++)
  {
    SW.Send(ent->KnotMultiplicitiesValue(i7));
  }
  SW.CloseSub();

  // --- own field : knots ---

  SW.OpenSub();
  for (int i8 = 1; i8 <= ent->NbKnots(); i8++)
  {
    SW.Send(ent->KnotsValue(i8));
  }
  SW.CloseSub();

  // --- own field : knotSpec ---

  SW.SendEnum(RWStepGeom_RWKnotType::ConvertToString(ent->KnotSpec()));
}

void RWStepGeom_RWBSplineCurveWithKnots::Share(
  const occ::handle<StepGeom_BSplineCurveWithKnots>& ent,
  Interface_EntityIterator&                          iter) const
{

  int nbElem1 = ent->NbControlPointsList();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ControlPointsListValue(is1));
  }
}

void RWStepGeom_RWBSplineCurveWithKnots::Check(
  const occ::handle<StepGeom_BSplineCurveWithKnots>& ent,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& ach) const
{
  int nbCPL   = ent->NbControlPointsList();
  int dgBSC   = ent->Degree();
  int nbMult  = ent->NbKnotMultiplicities();
  int nbKno   = ent->NbKnots();
  int sumMult = 0;
  //  std::cout << "BSplineCurveWithKnots: nbMult=" << nbMult << " nbKno= " <<
  //    nbKno << " nbCPL= " << nbCPL << " degree= " << dgBSC << std::endl;
  if (nbMult != nbKno)
  {
    ach->AddFail("ERROR: No.of KnotMultiplicities not equal No.of Knots");
  }
  if (nbMult == 0)
  {
    ach->AddFail("ERROR: No.of KnotMultiplicities is equal to 0");
    return;
  }

  int i; // svv Jan 10 2000: porting on DEC
  for (i = 1; i <= nbMult - 1; i++)
  {
    sumMult = sumMult + ent->KnotMultiplicitiesValue(i);
  }
  int sumNonP = nbCPL + dgBSC + 1;
  int mult1   = ent->KnotMultiplicitiesValue(1);
  int multN   = ent->KnotMultiplicitiesValue(nbMult);
  //  std::cout << "BSplineCurveWithKnots: mult1=" << mult1 << " multN= " <<
  //    multN << " sumMult= " << sumMult << std::endl;
  if ((sumMult + multN) == sumNonP)
  {
  }
  else if ((sumMult == nbCPL) && (mult1 == multN))
  {
  }
  else
  {
    ach->AddFail("ERROR: wrong number of Knot Multiplicities");
  }
  for (i = 2; i <= nbKno; i++)
  {
    double distKn = ent->KnotsValue(i - 1) - ent->KnotsValue(i);
    if (std::abs(distKn) <= RealEpsilon())
      ach->AddWarning("WARNING: Curve contains identical KnotsValues");
    else if (distKn > RealEpsilon())
      ach->AddFail("ERROR: Curve contains descending KnotsValues");
  }
}
