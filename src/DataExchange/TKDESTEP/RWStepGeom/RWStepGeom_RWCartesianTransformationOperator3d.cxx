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

#include <Interface_EntityIterator.hxx>
#include "RWStepGeom_RWCartesianTransformationOperator3d.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Direction.hxx>

RWStepGeom_RWCartesianTransformationOperator3d::RWStepGeom_RWCartesianTransformationOperator3d() = default;

void RWStepGeom_RWCartesianTransformationOperator3d::ReadStep(
  const occ::handle<StepData_StepReaderData>&                    data,
  const int                                                      num,
  occ::handle<Interface_Check>&                                  ach,
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& ent) const
{

  // 29 MAI 1997
  // PATCH CKY : functionally_defined_transformation est aussi supertype, avec
  //  deux champs STRING. Pour bien faire, les ajouter. Au minimum, les faire
  //  sauter. On attend 8 champs au lieu de 6 et on commence au champ 3

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 8, ach, "cartesian_transformation_operator_3d"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 3, "name", ach, aName);

  // --- inherited field : axis1 ---

  occ::handle<StepGeom_Direction> aAxis1;
  bool                            hasAaxis1 = true;
  if (data->IsParamDefined(num, 4))
  {
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadEntity(num, 4, "axis1", ach, STANDARD_TYPE(StepGeom_Direction), aAxis1);
  }
  else
  {
    hasAaxis1 = false;
    aAxis1.Nullify();
  }

  // --- inherited field : axis2 ---

  occ::handle<StepGeom_Direction> aAxis2;
  bool                            hasAaxis2 = true;
  if (data->IsParamDefined(num, 5))
  {
    // szv#4:S4163:12Mar99 `bool stat3 =` not needed
    data->ReadEntity(num, 5, "axis2", ach, STANDARD_TYPE(StepGeom_Direction), aAxis2);
  }
  else
  {
    hasAaxis2 = false;
    aAxis2.Nullify();
  }

  // --- inherited field : localOrigin ---

  occ::handle<StepGeom_CartesianPoint> aLocalOrigin;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data
    ->ReadEntity(num, 6, "local_origin", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aLocalOrigin);

  // --- inherited field : scale ---

  double aScale;
  bool   hasAscale = true;
  if (data->IsParamDefined(num, 7))
  {
    // szv#4:S4163:12Mar99 `bool stat5 =` not needed
    data->ReadReal(num, 7, "scale", ach, aScale);
  }
  else
  {
    hasAscale = false;
    aScale    = 0.;
  }

  // --- own field : axis3 ---

  occ::handle<StepGeom_Direction> aAxis3;
  bool                            hasAaxis3 = true;
  if (data->IsParamDefined(num, 8))
  {
    // szv#4:S4163:12Mar99 `bool stat6 =` not needed
    data->ReadEntity(num, 8, "axis3", ach, STANDARD_TYPE(StepGeom_Direction), aAxis3);
  }
  else
  {
    hasAaxis3 = false;
    aAxis3.Nullify();
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName,
            hasAaxis1,
            aAxis1,
            hasAaxis2,
            aAxis2,
            aLocalOrigin,
            hasAscale,
            aScale,
            hasAaxis3,
            aAxis3);
}

void RWStepGeom_RWCartesianTransformationOperator3d::WriteStep(
  StepData_StepWriter&                                           SW,
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& ent) const
{

  // --- inherited field name ---
  // PATCH CKY : name depuis geometric_representation_item
  //    et name + descr depuis functionally_defined_transformation

  SW.Send(ent->Name());
  SW.Send(ent->Name());
  SW.Send(ent->Name());

  // --- inherited field axis1 ---

  bool hasAaxis1 = ent->HasAxis1();
  if (hasAaxis1)
  {
    SW.Send(ent->Axis1());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field axis2 ---

  bool hasAaxis2 = ent->HasAxis2();
  if (hasAaxis2)
  {
    SW.Send(ent->Axis2());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field localOrigin ---

  SW.Send(ent->LocalOrigin());

  // --- inherited field scale ---

  bool hasAscale = ent->HasScale();
  if (hasAscale)
  {
    SW.Send(ent->Scale());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : axis3 ---

  bool hasAaxis3 = ent->HasAxis3();
  if (hasAaxis3)
  {
    SW.Send(ent->Axis3());
  }
  else
  {
    SW.SendUndef();
  }
}

void RWStepGeom_RWCartesianTransformationOperator3d::Share(
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& ent,
  Interface_EntityIterator&                                      iter) const
{
  if (ent->HasAxis1())
  {
    iter.GetOneItem(ent->Axis1());
  }

  if (ent->HasAxis2())
  {
    iter.GetOneItem(ent->Axis2());
  }

  iter.GetOneItem(ent->LocalOrigin());

  if (ent->HasAxis3())
  {
    iter.GetOneItem(ent->Axis3());
  }
}
