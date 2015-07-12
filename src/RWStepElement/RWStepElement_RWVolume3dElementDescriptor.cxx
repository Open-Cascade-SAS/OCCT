// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepElement_RWVolume3dElementDescriptor.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_HArray1OfVolumeElementPurposeMember.hxx>
#include <StepElement_Volume3dElementDescriptor.hxx>
#include <StepElement_VolumeElementPurposeMember.hxx>

//=======================================================================
//function : RWStepElement_RWVolume3dElementDescriptor
//purpose  : 
//=======================================================================
RWStepElement_RWVolume3dElementDescriptor::RWStepElement_RWVolume3dElementDescriptor ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWVolume3dElementDescriptor::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                          const Standard_Integer num,
                                                          Handle(Interface_Check)& ach,
                                                          const Handle(StepElement_Volume3dElementDescriptor) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"volume3d_element_descriptor") ) return;

  // Inherited fields of ElementDescriptor

  StepElement_ElementOrder aElementDescriptor_TopologyOrder = StepElement_Linear;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (!strcmp(text, ".LINEAR.")) aElementDescriptor_TopologyOrder = StepElement_Linear;
    else if (!strcmp(text, ".QUADRATIC.")) aElementDescriptor_TopologyOrder = StepElement_Quadratic;
    else if (!strcmp(text, ".CUBIC.")) aElementDescriptor_TopologyOrder = StepElement_Cubic;
    else ach->AddFail("Parameter #1 (element_descriptor.topology_order) has not allowed value");
  }
  else ach->AddFail("Parameter #1 (element_descriptor.topology_order) is not enumeration");

  Handle(TCollection_HAsciiString) aElementDescriptor_Description;
  data->ReadString (num, 2, "element_descriptor.description", ach, aElementDescriptor_Description);

  // Own fields of Volume3dElementDescriptor

  Handle(StepElement_HArray1OfVolumeElementPurposeMember) aPurpose;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "purpose", ach, sub3) ) {
    Standard_Integer nb0 = data->NbParams(sub3);
    aPurpose = new StepElement_HArray1OfVolumeElementPurposeMember (1, nb0);
    Standard_Integer num2 = sub3;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      //StepElement_VolumeElementPurpose anIt0;
      Handle(StepElement_VolumeElementPurposeMember) aMember = new StepElement_VolumeElementPurposeMember;
      //data->ReadEntity (num2, i0, "volume_element_purpose", ach, anIt0);
      data->ReadMember (num2, i0, "volume_element_purpose", ach, aMember);
      aPurpose->SetValue(i0, aMember);
    }
  }

  StepElement_Volume3dElementShape aShape = StepElement_Hexahedron;
  if (data->ParamType (num, 4) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 4);
    if      (!strcmp(text, ".HEXAHEDRON.")) aShape = StepElement_Hexahedron;
    else if (!strcmp(text, ".WEDGE.")) aShape = StepElement_Wedge;
    else if (!strcmp(text, ".TETRAHEDRON.")) aShape = StepElement_Tetrahedron;
    else if (!strcmp(text, ".PYRAMID.")) aShape = StepElement_Pyramid;
    else ach->AddFail("Parameter #4 (shape) has not allowed value");
  }
  else ach->AddFail("Parameter #4 (shape) is not enumeration");

  // Initialize entity
  ent->Init(aElementDescriptor_TopologyOrder,
            aElementDescriptor_Description,
            aPurpose,
            aShape);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWVolume3dElementDescriptor::WriteStep (StepData_StepWriter& SW,
                                                           const Handle(StepElement_Volume3dElementDescriptor) &ent) const
{

  // Inherited fields of ElementDescriptor

  switch (ent->StepElement_ElementDescriptor::TopologyOrder()) {
    case StepElement_Linear: SW.SendEnum (".LINEAR."); break;
    case StepElement_Quadratic: SW.SendEnum (".QUADRATIC."); break;
    case StepElement_Cubic: SW.SendEnum (".CUBIC."); break;
  }

  SW.Send (ent->StepElement_ElementDescriptor::Description());

  // Own fields of Volume3dElementDescriptor

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Purpose()->Length(); i2++ ) {
    Handle(StepElement_VolumeElementPurposeMember) Var0 = ent->Purpose()->Value(i2);
    SW.Send (Var0);
  }
  SW.CloseSub();

  switch (ent->Shape()) {
    case StepElement_Hexahedron: SW.SendEnum (".HEXAHEDRON."); break;
    case StepElement_Wedge: SW.SendEnum (".WEDGE."); break;
    case StepElement_Tetrahedron: SW.SendEnum (".TETRAHEDRON."); break;
    case StepElement_Pyramid: SW.SendEnum (".PYRAMID."); break;
  }
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWVolume3dElementDescriptor::Share (const Handle(StepElement_Volume3dElementDescriptor) &,
                                                       Interface_EntityIterator&) const
{

  // Inherited fields of ElementDescriptor

  // Own fields of Volume3dElementDescriptor
/*  CKY 17JUN04 : content is made of STRINGS and ENUMS. No entity !
  for (Standard_Integer i1=1; i1 <= ent->Purpose()->Length(); i1++ ) {
    Handle(StepElement_VolumeElementPurposeMember) Var0 = ent->Purpose()->Value(i1);
    iter.AddItem (Var0);
  }
*/
}
