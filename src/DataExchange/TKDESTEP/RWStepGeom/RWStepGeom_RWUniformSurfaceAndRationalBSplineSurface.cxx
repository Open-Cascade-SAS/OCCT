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
#include "RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

#include "RWStepGeom_RWBSplineSurfaceForm.pxx"

RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface::
  RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface()
{
}

void RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface::ReadStep(
  const occ::handle<StepData_StepReaderData>&                          data,
  const int                                          num0,
  occ::handle<Interface_Check>&                                        ach,
  const occ::handle<StepGeom_UniformSurfaceAndRationalBSplineSurface>& ent) const
{

  int num = num0;

  // --- Instance of plex component BoundedSurface ---

  if (!data->CheckNbParams(num, 0, ach, "bounded_surface"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of common supertype BSplineSurface ---

  if (!data->CheckNbParams(num, 7, ach, "b_spline_surface"))
    return;
  // --- field : uDegree ---

  int aUDegree;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadInteger(num, 1, "u_degree", ach, aUDegree);
  // --- field : vDegree ---

  int aVDegree;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadInteger(num, 2, "v_degree", ach, aVDegree);
  // --- field : controlPointsList ---

  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aControlPointsList;
  occ::handle<StepGeom_CartesianPoint>          anent3;
  int                         nsub3;
  if (data->ReadSubList(num, 3, "control_points_list", ach, nsub3))
  {
    int nbi3 = data->NbParams(nsub3);
    int nbj3 = data->NbParams(data->ParamNumber(nsub3, 1));
    aControlPointsList    = new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, nbi3, 1, nbj3);
    for (int i3 = 1; i3 <= nbi3; i3++)
    {
      int nsi3;
      if (data->ReadSubList(nsub3, i3, "sub-part(control_points_list)", ach, nsi3))
      {
        for (int j3 = 1; j3 <= nbj3; j3++)
        {
          // szv#4:S4163:12Mar99 `bool stat3 =` not needed
          if (data->ReadEntity(nsi3,
                               j3,
                               "cartesian_point",
                               ach,
                               STANDARD_TYPE(StepGeom_CartesianPoint),
                               anent3))
            aControlPointsList->SetValue(i3, j3, anent3);
        }
      }
    }
  }

  // --- field : surfaceForm ---

  StepGeom_BSplineSurfaceForm aSurfaceForm = StepGeom_bssfPlaneSurf;
  if (data->ParamType(num, 4) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 4);
    if (!RWStepGeom_RWBSplineSurfaceForm::ConvertToEnum(text, aSurfaceForm))
    {
      ach->AddFail("Enumeration b_spline_surface_form has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #4 (surface_form) is not an enumeration");
  // --- field : uClosed ---

  StepData_Logical aUClosed;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadLogical(num, 5, "u_closed", ach, aUClosed);
  // --- field : vClosed ---

  StepData_Logical aVClosed;
  // szv#4:S4163:12Mar99 `bool stat6 =` not needed
  data->ReadLogical(num, 6, "v_closed", ach, aVClosed);
  // --- field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat7 =` not needed
  data->ReadLogical(num, 7, "self_intersect", ach, aSelfIntersect);

  num = data->NextForComplex(num);

  // --- Instance of plex component GeometricRepresentationItem ---

  if (!data->CheckNbParams(num, 0, ach, "geometric_representation_item"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of plex component RationalBSplineSurface ---

  if (!data->CheckNbParams(num, 1, ach, "rational_b_spline_surface"))
    return;

  // --- field : weightsData ---

  occ::handle<NCollection_HArray2<double>> aWeightsData;
  double                 aWeightsDataItem;
  int              nsub8;
  if (data->ReadSubList(num, 1, "weights_data", ach, nsub8))
  {
    int nbi8 = data->NbParams(nsub8);
    int nbj8 = data->NbParams(data->ParamNumber(nsub8, 1));
    aWeightsData          = new NCollection_HArray2<double>(1, nbi8, 1, nbj8);
    for (int i8 = 1; i8 <= nbi8; i8++)
    {
      int nsi8;
      if (data->ReadSubList(nsub8, i8, "sub-part(weights_data)", ach, nsi8))
      {
        for (int j8 = 1; j8 <= nbj8; j8++)
        {
          // szv#4:S4163:12Mar99 `bool stat8 =` not needed
          if (data->ReadReal(nsi8, j8, "weights_data", ach, aWeightsDataItem))
            aWeightsData->SetValue(i8, j8, aWeightsDataItem);
        }
      }
    }
  }

  num = data->NextForComplex(num);

  // --- Instance of plex component RepresentationItem ---

  if (!data->CheckNbParams(num, 1, ach, "representation_item"))
    return;

  // --- field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat9 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  num = data->NextForComplex(num);

  // --- Instance of plex component Surface ---

  if (!data->CheckNbParams(num, 0, ach, "surface"))
    return;

  num = data->NextForComplex(num);

  // --- Instance of plex component UniformSurface ---

  if (!data->CheckNbParams(num, 0, ach, "uniform_surface"))
    return;

  //--- Initialisation of the red entity ---

  ent->Init(aName,
            aUDegree,
            aVDegree,
            aControlPointsList,
            aSurfaceForm,
            aUClosed,
            aVClosed,
            aSelfIntersect,
            aWeightsData);
}

void RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface::WriteStep(
  StepData_StepWriter&                                            SW,
  const occ::handle<StepGeom_UniformSurfaceAndRationalBSplineSurface>& ent) const
{

  // --- Instance of plex component BoundedSurface ---

  SW.StartEntity("BOUNDED_SURFACE");

  // --- Instance of common supertype BSplineSurface ---

  SW.StartEntity("B_SPLINE_SURFACE");
  // --- field : uDegree ---

  SW.Send(ent->UDegree());
  // --- field : vDegree ---

  SW.Send(ent->VDegree());
  // --- field : controlPointsList ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbControlPointsListI(); i3++)
  {
    SW.NewLine(false);
    SW.OpenSub();
    for (int j3 = 1; j3 <= ent->NbControlPointsListJ(); j3++)
    {
      SW.Send(ent->ControlPointsListValue(i3, j3));
      SW.JoinLast(false);
    }
    SW.CloseSub();
  }
  SW.CloseSub();
  // --- field : surfaceForm ---

  SW.SendEnum(RWStepGeom_RWBSplineSurfaceForm::ConvertToString(ent->SurfaceForm()));

  // --- field : uClosed ---

  SW.SendLogical(ent->UClosed());
  // --- field : vClosed ---

  SW.SendLogical(ent->VClosed());
  // --- field : selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());

  // --- Instance of plex component GeometricRepresentationItem ---

  SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

  // --- Instance of plex component RationalBSplineSurface ---

  SW.StartEntity("RATIONAL_B_SPLINE_SURFACE");
  // --- field : weightsData ---

  SW.OpenSub();
  for (int i8 = 1; i8 <= ent->NbWeightsDataI(); i8++)
  {
    SW.NewLine(false);
    SW.OpenSub();
    for (int j8 = 1; j8 <= ent->NbWeightsDataJ(); j8++)
    {
      SW.Send(ent->WeightsDataValue(i8, j8));
      SW.JoinLast(false);
    }
    SW.CloseSub();
  }
  SW.CloseSub();

  // --- Instance of plex component RepresentationItem ---

  SW.StartEntity("REPRESENTATION_ITEM");
  // --- field : name ---

  SW.Send(ent->Name());

  // --- Instance of plex component Surface ---

  SW.StartEntity("SURFACE");

  // --- Instance of plex component UniformSurface ---

  SW.StartEntity("UNIFORM_SURFACE");
}

void RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface::Share(
  const occ::handle<StepGeom_UniformSurfaceAndRationalBSplineSurface>& ent,
  Interface_EntityIterator&                                       iter) const
{

  int nbiElem1 = ent->NbControlPointsListI();
  int nbjElem1 = ent->NbControlPointsListJ();
  for (int is1 = 1; is1 <= nbiElem1; is1++)
  {
    for (int js1 = 1; js1 <= nbjElem1; js1++)
    {
      iter.GetOneItem(ent->ControlPointsListValue(is1, js1));
    }
  }
}
