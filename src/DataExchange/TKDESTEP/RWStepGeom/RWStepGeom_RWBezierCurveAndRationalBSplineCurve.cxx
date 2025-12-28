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
#include "RWStepGeom_RWBezierCurveAndRationalBSplineCurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BezierCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include "RWStepGeom_RWBSplineCurveForm.pxx"

RWStepGeom_RWBezierCurveAndRationalBSplineCurve::RWStepGeom_RWBezierCurveAndRationalBSplineCurve() =
  default;

void RWStepGeom_RWBezierCurveAndRationalBSplineCurve::ReadStep(
  const occ::handle<StepData_StepReaderData>&                     data,
  const int                                                       num0,
  occ::handle<Interface_Check>&                                   ach,
  const occ::handle<StepGeom_BezierCurveAndRationalBSplineCurve>& ent) const
{

  int num = num0;

  // --- Instance of plex component BezierCurve ---

  if (!data->CheckNbParams(num, 0, ach, "bezier_curve"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of plex component BoundedCurve ---

  if (!data->CheckNbParams(num, 0, ach, "bounded_curve"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of common supertype BSplineCurve ---

  if (!data->CheckNbParams(num, 5, ach, "b_spline_curve"))
    return;
  // --- field : degree ---

  int aDegree;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadInteger(num, 1, "degree", ach, aDegree);
  // --- field : controlPointsList ---

  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aControlPointsList;
  occ::handle<StepGeom_CartesianPoint>                                   anent2;
  int                                                                    nsub2;
  if (data->ReadSubList(num, 2, "control_points_list", ach, nsub2))
  {
    int nb2            = data->NbParams(nsub2);
    aControlPointsList = new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2,
                           i2,
                           "cartesian_point",
                           ach,
                           STANDARD_TYPE(StepGeom_CartesianPoint),
                           anent2))
        aControlPointsList->SetValue(i2, anent2);
    }
  }

  // --- field : curveForm ---

  StepGeom_BSplineCurveForm aCurveForm = StepGeom_bscfPolylineForm;
  if (data->ParamType(num, 3) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 3);
    if (!RWStepGeom_RWBSplineCurveForm::ConvertToEnum(text, aCurveForm))
    {
      ach->AddFail("Enumeration b_spline_curve_form has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #3 (curve_form) is not an enumeration");
  // --- field : closedCurve ---

  StepData_Logical aClosedCurve;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadLogical(num, 4, "closed_curve", ach, aClosedCurve);
  // --- field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadLogical(num, 5, "self_intersect", ach, aSelfIntersect);

  num = data->NextForComplex(num);

  // --- Instance of plex component Curve ---

  if (!data->CheckNbParams(num, 0, ach, "curve"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of plex component GeometricRepresentationItem ---

  if (!data->CheckNbParams(num, 0, ach, "geometric_representation_item"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of plex component RationalBSplineCurve ---

  if (!data->CheckNbParams(num, 1, ach, "rational_b_spline_curve"))
    return;

  // --- field : weightsData ---

  occ::handle<NCollection_HArray1<double>> aWeightsData;
  double                                   aWeightsDataItem;
  int                                      nsub6;
  if (data->ReadSubList(num, 1, "weights_data", ach, nsub6))
  {
    int nb6      = data->NbParams(nsub6);
    aWeightsData = new NCollection_HArray1<double>(1, nb6);
    for (int i6 = 1; i6 <= nb6; i6++)
    {
      // szv#4:S4163:12Mar99 `bool stat6 =` not needed
      if (data->ReadReal(nsub6, i6, "weights_data", ach, aWeightsDataItem))
        aWeightsData->SetValue(i6, aWeightsDataItem);
    }
  }

  num = data->NextForComplex(num);

  // --- Instance of plex component RepresentationItem ---

  if (!data->CheckNbParams(num, 1, ach, "representation_item"))
    return;

  // --- field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat7 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  //--- Initialisation of the red entity ---

  ent->Init(aName,
            aDegree,
            aControlPointsList,
            aCurveForm,
            aClosedCurve,
            aSelfIntersect,
            aWeightsData);
}

void RWStepGeom_RWBezierCurveAndRationalBSplineCurve::WriteStep(
  StepData_StepWriter&                                            SW,
  const occ::handle<StepGeom_BezierCurveAndRationalBSplineCurve>& ent) const
{

  // --- Instance of plex component BezierCurve ---

  SW.StartEntity("BEZIER_CURVE");

  // --- Instance of plex component BoundedCurve ---

  SW.StartEntity("BOUNDED_CURVE");

  // --- Instance of common supertype BSplineCurve ---

  SW.StartEntity("B_SPLINE_CURVE");
  // --- field : degree ---

  SW.Send(ent->Degree());
  // --- field : controlPointsList ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbControlPointsList(); i2++)
  {
    SW.Send(ent->ControlPointsListValue(i2));
  }
  SW.CloseSub();
  // --- field : curveForm ---
  SW.SendEnum(RWStepGeom_RWBSplineCurveForm::ConvertToString(ent->CurveForm()));
  // --- field : closedCurve ---

  SW.SendLogical(ent->ClosedCurve());
  // --- field : selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());

  // --- Instance of plex component Curve ---

  SW.StartEntity("CURVE");

  // --- Instance of plex component GeometricRepresentationItem ---

  SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

  // --- Instance of plex component RationalBSplineCurve ---

  SW.StartEntity("RATIONAL_B_SPLINE_CURVE");
  // --- field : weightsData ---

  SW.OpenSub();
  for (int i6 = 1; i6 <= ent->NbWeightsData(); i6++)
  {
    SW.Send(ent->WeightsDataValue(i6));
  }
  SW.CloseSub();

  // --- Instance of plex component RepresentationItem ---

  SW.StartEntity("REPRESENTATION_ITEM");
  // --- field : name ---

  SW.Send(ent->Name());
}

void RWStepGeom_RWBezierCurveAndRationalBSplineCurve::Share(
  const occ::handle<StepGeom_BezierCurveAndRationalBSplineCurve>& ent,
  Interface_EntityIterator&                                       iter) const
{

  int nbElem1 = ent->NbControlPointsList();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ControlPointsListValue(is1));
  }
}
