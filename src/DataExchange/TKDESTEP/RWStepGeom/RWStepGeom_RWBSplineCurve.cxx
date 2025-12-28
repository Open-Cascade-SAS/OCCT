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
#include "RWStepGeom_RWBSplineCurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineCurve.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepGeom_RWBSplineCurveForm.pxx"

RWStepGeom_RWBSplineCurve::RWStepGeom_RWBSplineCurve() {}

void RWStepGeom_RWBSplineCurve::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                         const int                                   num,
                                         occ::handle<Interface_Check>&               ach,
                                         const occ::handle<StepGeom_BSplineCurve>&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 6, ach, "b_spline_curve"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : degree ---

  int aDegree;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadInteger(num, 2, "degree", ach, aDegree);

  // --- own field : controlPointsList ---

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

  // --- own field : curveForm ---

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

  // --- own field : closedCurve ---

  StepData_Logical aClosedCurve;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadLogical(num, 5, "closed_curve", ach, aClosedCurve);

  // --- own field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat6 =` not needed
  data->ReadLogical(num, 6, "self_intersect", ach, aSelfIntersect);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}

void RWStepGeom_RWBSplineCurve::WriteStep(StepData_StepWriter&                      SW,
                                          const occ::handle<StepGeom_BSplineCurve>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : degree ---

  SW.Send(ent->Degree());

  // --- own field : controlPointsList ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbControlPointsList(); i3++)
  {
    SW.Send(ent->ControlPointsListValue(i3));
  }
  SW.CloseSub();

  // --- own field : curveForm ---

  SW.SendEnum(RWStepGeom_RWBSplineCurveForm::ConvertToString(ent->CurveForm()));

  // --- own field : closedCurve ---

  SW.SendLogical(ent->ClosedCurve());

  // --- own field : selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());
}

void RWStepGeom_RWBSplineCurve::Share(const occ::handle<StepGeom_BSplineCurve>& ent,
                                      Interface_EntityIterator&                 iter) const
{

  int nbElem1 = ent->NbControlPointsList();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ControlPointsListValue(is1));
  }
}
