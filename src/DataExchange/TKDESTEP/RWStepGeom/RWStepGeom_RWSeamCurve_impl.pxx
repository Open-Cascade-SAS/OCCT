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
#include "RWStepGeom_RWSeamCurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_SeamCurve.hxx>

#include "RWStepGeom_RWPreferredSurfaceCurveRepresentation.pxx"

RWStepGeom_RWSeamCurve::RWStepGeom_RWSeamCurve() {}

void RWStepGeom_RWSeamCurve::ReadStep(const Handle(StepData_StepReaderData)& data,
                                      const Standard_Integer                 num,
                                      Handle(Interface_Check)&               ach,
                                      const Handle(StepGeom_SeamCurve)&      ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "seam_curve"))
    return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : curve3d ---

  Handle(StepGeom_Curve) aCurve3d;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2, "curve_3d", ach, STANDARD_TYPE(StepGeom_Curve), aCurve3d);

  // --- inherited field : associatedGeometry ---

  Handle(StepGeom_HArray1OfPcurveOrSurface) aAssociatedGeometry;
  StepGeom_PcurveOrSurface                  aAssociatedGeometryItem;
  Handle(Standard_Transient)                assgeomval;
  Standard_Integer                          nsub3;
  if (data->ReadSubList(num, 3, "associated_geometry", ach, nsub3))
  {
    Standard_Integer nb3 = data->NbParams(nsub3);
    aAssociatedGeometry  = new StepGeom_HArray1OfPcurveOrSurface(1, nb3);
    for (Standard_Integer i3 = 1; i3 <= nb3; i3++)
    {
      // szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
      if (data->ReadEntity(nsub3, i3, "associated_geometry", ach, aAssociatedGeometryItem))
        aAssociatedGeometry->SetValue(i3, aAssociatedGeometryItem);
      if (i3 == 1)
        assgeomval = aAssociatedGeometryItem.Value();
      else if (assgeomval == aAssociatedGeometryItem.Value()) //: a9 abv
        ach->AddFail("Seam Curve with twice the same geom");
    }
  }

  // --- inherited field : masterRepresentation ---

  StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation = StepGeom_pscrCurve3d;
  if (data->ParamType(num, 4) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 4);
    if (!RWStepGeom_RWPreferredSurfaceCurveRepresentation::ConvertToEnum(text,
                                                                         aMasterRepresentation))
    {
      ach->AddFail("Enumeration preferred_surface_curve_representation has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #4 (master_representation) is not an enumeration");

  //--- Initialisation of the read entity ---

  ent->Init(aName, aCurve3d, aAssociatedGeometry, aMasterRepresentation);
}

void RWStepGeom_RWSeamCurve::WriteStep(StepData_StepWriter&              SW,
                                       const Handle(StepGeom_SeamCurve)& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field curve3d ---

  SW.Send(ent->Curve3d());

  // --- inherited field associatedGeometry ---

  SW.OpenSub();
  for (Standard_Integer i3 = 1; i3 <= ent->NbAssociatedGeometry(); i3++)
  {
    SW.Send(ent->AssociatedGeometryValue(i3).Value());
  }
  SW.CloseSub();

  // --- inherited field masterRepresentation ---

  SW.SendEnum(
    RWStepGeom_RWPreferredSurfaceCurveRepresentation::ConvertToString(ent->MasterRepresentation()));
}

void RWStepGeom_RWSeamCurve::Share(const Handle(StepGeom_SeamCurve)& ent,
                                   Interface_EntityIterator&         iter) const
{

  iter.GetOneItem(ent->Curve3d());

  Standard_Integer nbElem2 = ent->NbAssociatedGeometry();
  for (Standard_Integer is2 = 1; is2 <= nbElem2; is2++)
  {
    iter.GetOneItem(ent->AssociatedGeometryValue(is2).Value());
  }
}
