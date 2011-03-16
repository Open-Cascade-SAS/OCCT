// File:	RWStepFEA_RWFeaAxis2Placement3d.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaAxis2Placement3d.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaAxis2Placement3d
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaAxis2Placement3d::RWStepFEA_RWFeaAxis2Placement3d ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAxis2Placement3d::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepFEA_FeaAxis2Placement3d) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"fea_axis2_placement3d") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of Placement

  Handle(StepGeom_CartesianPoint) aPlacement_Location;
  data->ReadEntity (num, 2, "placement.location", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aPlacement_Location);

  // Inherited fields of Axis2Placement3d

  Handle(StepGeom_Direction) aAxis2Placement3d_Axis;
  Standard_Boolean hasAxis2Placement3d_Axis = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadEntity (num, 3, "axis2_placement3d.axis", ach, STANDARD_TYPE(StepGeom_Direction), aAxis2Placement3d_Axis);
  }
  else {
    hasAxis2Placement3d_Axis = Standard_False;
  }

  Handle(StepGeom_Direction) aAxis2Placement3d_RefDirection;
  Standard_Boolean hasAxis2Placement3d_RefDirection = Standard_True;
  if ( data->IsParamDefined (num,4) ) {
    data->ReadEntity (num, 4, "axis2_placement3d.ref_direction", ach, STANDARD_TYPE(StepGeom_Direction), aAxis2Placement3d_RefDirection);
  }
  else {
    hasAxis2Placement3d_RefDirection = Standard_False;
  }

  // Own fields of FeaAxis2Placement3d

  StepFEA_CoordinateSystemType aSystemType;
  if (data->ParamType (num, 5) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 5);
    if      (strcmp(text, ".CARTESIAN.")) aSystemType = StepFEA_Cartesian;
    else if (strcmp(text, ".CYLINDRICAL.")) aSystemType = StepFEA_Cylindrical;
    else if (strcmp(text, ".SPHERICAL.")) aSystemType = StepFEA_Spherical;
    else ach->AddFail("Parameter #5 (system_type) has not allowed value");
  }
  else ach->AddFail("Parameter #5 (system_type) is not enumeration");

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 6, "description", ach, aDescription);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aPlacement_Location,
            hasAxis2Placement3d_Axis,
            aAxis2Placement3d_Axis,
            hasAxis2Placement3d_RefDirection,
            aAxis2Placement3d_RefDirection,
            aSystemType,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAxis2Placement3d::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepFEA_FeaAxis2Placement3d) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Inherited fields of Placement

  SW.Send (ent->StepGeom_Placement::Location());

  // Inherited fields of Axis2Placement3d

  if ( ent->StepGeom_Axis2Placement3d::HasAxis() ) {
    SW.Send (ent->StepGeom_Axis2Placement3d::Axis());
  }
  else SW.SendUndef();

  if ( ent->StepGeom_Axis2Placement3d::HasRefDirection() ) {
    SW.Send (ent->StepGeom_Axis2Placement3d::RefDirection());
  }
  else SW.SendUndef();

  // Own fields of FeaAxis2Placement3d

  switch (ent->SystemType()) {
    case StepFEA_Cartesian: SW.SendEnum (".CARTESIAN."); break;
    case StepFEA_Cylindrical: SW.SendEnum (".CYLINDRICAL."); break;
    case StepFEA_Spherical: SW.SendEnum (".SPHERICAL."); break;
  }

  SW.Send (ent->Description());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAxis2Placement3d::Share (const Handle(StepFEA_FeaAxis2Placement3d) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of Placement

  iter.AddItem (ent->StepGeom_Placement::Location());

  // Inherited fields of Axis2Placement3d

  if ( ent->StepGeom_Axis2Placement3d::HasAxis() ) {
    iter.AddItem (ent->StepGeom_Axis2Placement3d::Axis());
  }

  if ( ent->StepGeom_Axis2Placement3d::HasRefDirection() ) {
    iter.AddItem (ent->StepGeom_Axis2Placement3d::RefDirection());
  }

  // Own fields of FeaAxis2Placement3d
}
