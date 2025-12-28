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
#include "RWStepGeom_RWRationalBSplineCurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_RationalBSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include "RWStepGeom_RWBSplineCurveForm.pxx"

RWStepGeom_RWRationalBSplineCurve::RWStepGeom_RWRationalBSplineCurve() = default;

void RWStepGeom_RWRationalBSplineCurve::ReadStep(
  const occ::handle<StepData_StepReaderData>&       data,
  const int                                         num,
  occ::handle<Interface_Check>&                     ach,
  const occ::handle<StepGeom_RationalBSplineCurve>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 7, ach, "rational_b_spline_curve"))
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
    int nb3            = data->NbParams(nsub3);
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

  // --- own field : weightsData ---

  occ::handle<NCollection_HArray1<double>> aWeightsData;
  double                                   aWeightsDataItem;
  int                                      nsub7;
  if (data->ReadSubList(num, 7, "weights_data", ach, nsub7))
  {
    int nb7      = data->NbParams(nsub7);
    aWeightsData = new NCollection_HArray1<double>(1, nb7);
    for (int i7 = 1; i7 <= nb7; i7++)
    {
      // szv#4:S4163:12Mar99 `bool stat7 =` not needed
      if (data->ReadReal(nsub7, i7, "weights_data", ach, aWeightsDataItem))
        aWeightsData->SetValue(i7, aWeightsDataItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName,
            aDegree,
            aControlPointsList,
            aCurveForm,
            aClosedCurve,
            aSelfIntersect,
            aWeightsData);
}

void RWStepGeom_RWRationalBSplineCurve::WriteStep(
  StepData_StepWriter&                              SW,
  const occ::handle<StepGeom_RationalBSplineCurve>& ent) const
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

  // --- own field : weightsData ---

  SW.OpenSub();
  for (int i7 = 1; i7 <= ent->NbWeightsData(); i7++)
  {
    SW.Send(ent->WeightsDataValue(i7));
  }
  SW.CloseSub();
}

void RWStepGeom_RWRationalBSplineCurve::Share(const occ::handle<StepGeom_RationalBSplineCurve>& ent,
                                              Interface_EntityIterator& iter) const
{

  int nbElem1 = ent->NbControlPointsList();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ControlPointsListValue(is1));
  }
}

void RWStepGeom_RWRationalBSplineCurve::Check(const occ::handle<StepGeom_RationalBSplineCurve>& ent,
                                              const Interface_ShareTool&,
                                              occ::handle<Interface_Check>& ach) const
{
  int nbWght = ent->NbWeightsData();
  int nbCPL  = ent->NbControlPointsList();
  //  std::cout << "RationalBSplineCurve: nbWght=" << nbWght << " nbCPL: " <<
  //    nbCPL << std::endl;
  if (nbWght != nbCPL)
  {
    ach->AddFail("ERROR: No.of ControlPoints not equal No.of Weights");
  }
  for (int i = 1; i <= nbWght; i++)
  {
    if (ent->WeightsDataValue(i) < RealEpsilon())
    {
      ach->AddFail("ERROR: WeightsData Value not greater than 0.0");
    }
  }
}
