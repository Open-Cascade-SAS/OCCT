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
#include "RWStepGeom_RWUniformSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_UniformSurface.hxx>

#include "RWStepGeom_RWBSplineSurfaceForm.pxx"

RWStepGeom_RWUniformSurface::RWStepGeom_RWUniformSurface() {}

void RWStepGeom_RWUniformSurface::ReadStep(const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer                 num,
                                           Handle(Interface_Check)&               ach,
                                           const Handle(StepGeom_UniformSurface)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 8, ach, "uniform_surface"))
    return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : uDegree ---

  Standard_Integer aUDegree;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadInteger(num, 2, "u_degree", ach, aUDegree);

  // --- inherited field : vDegree ---

  Standard_Integer aVDegree;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadInteger(num, 3, "v_degree", ach, aVDegree);

  // --- inherited field : controlPointsList ---

  Handle(StepGeom_HArray2OfCartesianPoint) aControlPointsList;
  Handle(StepGeom_CartesianPoint)          anent4;
  Standard_Integer                         nsub4;
  if (data->ReadSubList(num, 4, "control_points_list", ach, nsub4))
  {
    Standard_Integer nbi4 = data->NbParams(nsub4);
    Standard_Integer nbj4 = data->NbParams(data->ParamNumber(nsub4, 1));
    aControlPointsList    = new StepGeom_HArray2OfCartesianPoint(1, nbi4, 1, nbj4);
    for (Standard_Integer i4 = 1; i4 <= nbi4; i4++)
    {
      Standard_Integer nsi4;
      if (data->ReadSubList(nsub4, i4, "sub-part(control_points_list)", ach, nsi4))
      {
        for (Standard_Integer j4 = 1; j4 <= nbj4; j4++)
        {
          // szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
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

  // --- inherited field : surfaceForm ---

  StepGeom_BSplineSurfaceForm aSurfaceForm = StepGeom_bssfPlaneSurf;
  if (data->ParamType(num, 5) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 5);
    if (!RWStepGeom_RWBSplineSurfaceForm::ConvertToEnum(text, aSurfaceForm))
    {
      ach->AddFail("Enumeration b_spline_surface_form has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #5 (surface_form) is not an enumeration");

  // --- inherited field : uClosed ---

  StepData_Logical aUClosed;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
  data->ReadLogical(num, 6, "u_closed", ach, aUClosed);

  // --- inherited field : vClosed ---

  StepData_Logical aVClosed;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
  data->ReadLogical(num, 7, "v_closed", ach, aVClosed);

  // --- inherited field : selfIntersect ---

  StepData_Logical aSelfIntersect;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
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

void RWStepGeom_RWUniformSurface::WriteStep(StepData_StepWriter&                   SW,
                                            const Handle(StepGeom_UniformSurface)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field uDegree ---

  SW.Send(ent->UDegree());

  // --- inherited field vDegree ---

  SW.Send(ent->VDegree());

  // --- inherited field controlPointsList ---

  SW.OpenSub();
  for (Standard_Integer i4 = 1; i4 <= ent->NbControlPointsListI(); i4++)
  {
    SW.NewLine(Standard_False);
    SW.OpenSub();
    for (Standard_Integer j4 = 1; j4 <= ent->NbControlPointsListJ(); j4++)
    {
      SW.Send(ent->ControlPointsListValue(i4, j4));
      SW.JoinLast(Standard_False);
    }
    SW.CloseSub();
  }
  SW.CloseSub();

  // --- inherited field surfaceForm ---

  SW.SendEnum(RWStepGeom_RWBSplineSurfaceForm::ConvertToString(ent->SurfaceForm()));

  // --- inherited field uClosed ---

  SW.SendLogical(ent->UClosed());

  // --- inherited field vClosed ---

  SW.SendLogical(ent->VClosed());

  // --- inherited field selfIntersect ---

  SW.SendLogical(ent->SelfIntersect());
}

void RWStepGeom_RWUniformSurface::Share(const Handle(StepGeom_UniformSurface)& ent,
                                        Interface_EntityIterator&              iter) const
{

  Standard_Integer nbiElem1 = ent->NbControlPointsListI();
  Standard_Integer nbjElem1 = ent->NbControlPointsListJ();
  for (Standard_Integer is1 = 1; is1 <= nbiElem1; is1++)
  {
    for (Standard_Integer js1 = 1; js1 <= nbjElem1; js1++)
    {
      iter.GetOneItem(ent->ControlPointsListValue(is1, js1));
    }
  }
}
