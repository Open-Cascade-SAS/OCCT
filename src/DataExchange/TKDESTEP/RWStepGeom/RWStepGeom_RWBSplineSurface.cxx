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
#include "RWStepGeom_RWBSplineSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepGeom_RWBSplineSurfaceForm.pxx"

RWStepGeom_RWBSplineSurface::RWStepGeom_RWBSplineSurface() {}

void RWStepGeom_RWBSplineSurface::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                 num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepGeom_BSplineSurface>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 8, ach, "b_spline_surface"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : uDegree ---

  int aUDegree;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadInteger(num, 2, "u_degree", ach, aUDegree);

  // --- own field : vDegree ---

  int aVDegree;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadInteger(num, 3, "v_degree", ach, aVDegree);

  // --- own field : controlPointsList ---

  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aControlPointsList;
  occ::handle<StepGeom_CartesianPoint>          anent4;
  int                         nsub4;
  if (data->ReadSubList(num, 4, "control_points_list", ach, nsub4))
  {
    int nbi4 = data->NbParams(nsub4);
    int nbj4 = data->NbParams(data->ParamNumber(nsub4, 1));
    aControlPointsList    = new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, nbi4, 1, nbj4);
    for (int i4 = 1; i4 <= nbi4; i4++)
    {
      int nsi4;
      if (data->ReadSubList(nsub4, i4, "sub-part(control_points_list)", ach, nsi4))
      {
        for (int j4 = 1; j4 <= nbj4; j4++)
        {
          // szv#4:S4163:12Mar99 `bool stat4 =` not needed
          if (data->ReadEntity(nsi4,
                               j4,
                               "cartesian_point",
                               ach,
                               STANDARD_TYPE(StepGeom_CartesianPoint),
                               anent4))
            aControlPointsList->SetValue(i4, j4, anent4);
        }
      }
    }
  }

  // --- own field : surfaceForm ---

  StepGeom_BSplineSurfaceForm aSurfaceForm = StepGeom_bssfPlaneSurf;
  if (data->ParamType(num, 5) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 5);
    if (!RWStepGeom_RWBSplineSurfaceForm::ConvertToEnum(text, aSurfaceForm))
    {
      ach->AddFail("Enumeration b_spline_surface_form has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #5 (surface_form) is not an enumeration");

  // --- own field : uClosed ---

  StepData_Logical aUClosed;
  // szv#4:S4163:12Mar99 `bool stat6 =` not needed
  data->ReadLogical(num, 6, "u_closed", ach, aUClosed);

  // --- own field : vClosed ---

  StepData_Logical aVClosed;
  // szv#4:S4163:12Mar99 `bool stat7 =` not needed
  data->ReadLogical(num, 7, "v_closed", ach, aVClosed);

  // --- own field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `bool stat8 =` not needed
  data->ReadLogical(num, 8, "self_intersect", ach, aSelfIntersect);

  //--- Initialisation of the read entity ---

  ent->Init(aName,
            aUDegree,
            aVDegree,
            aControlPointsList,
            aSurfaceForm,
            aUClosed,
            aVClosed,
            aSelfIntersect);
}

void RWStepGeom_RWBSplineSurface::WriteStep(StepData_StepWriter&                   SW,
                                            const occ::handle<StepGeom_BSplineSurface>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : uDegree ---

  SW.Send(ent->UDegree());

  // --- own field : vDegree ---

  SW.Send(ent->VDegree());

  // --- own field : controlPointsList ---

  SW.OpenSub();
  for (int i4 = 1; i4 <= ent->NbControlPointsListI(); i4++)
  {
    SW.NewLine(false);
    SW.OpenSub();
    for (int j4 = 1; j4 <= ent->NbControlPointsListJ(); j4++)
    {
      SW.Send(ent->ControlPointsListValue(i4, j4));
      SW.JoinLast(false);
    }
    SW.CloseSub();
  }
  SW.CloseSub();

  // --- own field : surfaceForm ---

  SW.SendEnum(RWStepGeom_RWBSplineSurfaceForm::ConvertToString(ent->SurfaceForm()));

  // --- own field : uClosed ---

  SW.SendLogical(ent->UClosed());

  // --- own field : vClosed ---

  SW.SendLogical(ent->VClosed());

  // --- own field : selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());
}

void RWStepGeom_RWBSplineSurface::Share(const occ::handle<StepGeom_BSplineSurface>& ent,
                                        Interface_EntityIterator&              iter) const
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
