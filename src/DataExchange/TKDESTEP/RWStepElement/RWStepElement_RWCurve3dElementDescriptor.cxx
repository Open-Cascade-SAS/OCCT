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
#include "RWStepElement_RWCurve3dElementDescriptor.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_Curve3dElementDescriptor.hxx>
#include <StepElement_CurveElementPurposeMember.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepElement_RWCurve3dElementDescriptor::RWStepElement_RWCurve3dElementDescriptor() {}

//=================================================================================================

void RWStepElement_RWCurve3dElementDescriptor::ReadStep(
  const occ::handle<StepData_StepReaderData>&              data,
  const int                                                num,
  occ::handle<Interface_Check>&                            ach,
  const occ::handle<StepElement_Curve3dElementDescriptor>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 3, ach, "curve3d_element_descriptor"))
    return;

  // Inherited fields of ElementDescriptor

  StepElement_ElementOrder aElementDescriptor_TopologyOrder = StepElement_Linear;
  if (data->ParamType(num, 1) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 1);
    if (strcmp(text, ".LINEAR.") == 0)
      aElementDescriptor_TopologyOrder = StepElement_Linear;
    else if (strcmp(text, ".QUADRATIC.") == 0)
      aElementDescriptor_TopologyOrder = StepElement_Quadratic;
    else if (strcmp(text, ".CUBIC.") == 0)
      aElementDescriptor_TopologyOrder = StepElement_Cubic;
    else
      ach->AddFail("Parameter #1 (element_descriptor.topology_order) has not allowed value");
  }
  else
    ach->AddFail("Parameter #1 (element_descriptor.topology_order) is not enumeration");

  occ::handle<TCollection_HAsciiString> aElementDescriptor_Description;
  data->ReadString(num, 2, "element_descriptor.description", ach, aElementDescriptor_Description);

  // Own fields of Curve3dElementDescriptor

  occ::handle<NCollection_HArray1<
    occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>>
      aPurpose;
  int sub3 = 0;
  if (data->ReadSubList(num, 3, "purpose", ach, sub3))
  {
    int nb0 = data->NbParams(sub3);
    // int nbj0 = data->NbParams(data->ParamNumber(sub3,1));
    aPurpose = new NCollection_HArray1<
      occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>>(1,
                                                                                              nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>>
          HSCEPM = new NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>;
      int subj3  = 0;
      if (data->ReadSubList(sub3, i0, "sub-part(purpose)", ach, subj3))
      {
        int num4 = subj3;
        int nbj0 = data->NbParams(data->ParamNumber(sub3, i0));
        for (int j0 = 1; j0 <= nbj0; j0++)
        {
          occ::handle<StepElement_CurveElementPurposeMember> aMember =
            new StepElement_CurveElementPurposeMember;
          data->ReadMember(num4, j0, "curve_element_purpose", ach, aMember);
          // data->ReadEntity (num4, j0, "curve_element_purpose", ach, anIt0);
          HSCEPM->Append(aMember);
        }
      }
      aPurpose->SetValue(i0, HSCEPM);
    }
  }

  // Initialize entity
  ent->Init(aElementDescriptor_TopologyOrder, aElementDescriptor_Description, aPurpose);
}

//=================================================================================================

void RWStepElement_RWCurve3dElementDescriptor::WriteStep(
  StepData_StepWriter&                                     SW,
  const occ::handle<StepElement_Curve3dElementDescriptor>& ent) const
{

  // Inherited fields of ElementDescriptor

  switch (ent->StepElement_ElementDescriptor::TopologyOrder())
  {
    case StepElement_Linear:
      SW.SendEnum(".LINEAR.");
      break;
    case StepElement_Quadratic:
      SW.SendEnum(".QUADRATIC.");
      break;
    case StepElement_Cubic:
      SW.SendEnum(".CUBIC.");
      break;
  }

  SW.Send(ent->StepElement_ElementDescriptor::Description());

  // Own fields of Curve3dElementDescriptor

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->Purpose()->Length(); i2++)
  {
    SW.NewLine(false);
    SW.OpenSub();
    occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>> HSCEPM =
      ent->Purpose()->Value(i2);
    for (int j2 = 1; j2 <= HSCEPM->Length(); j2++)
    {
      occ::handle<StepElement_CurveElementPurposeMember> Var0 = HSCEPM->Value(j2);
      SW.Send(Var0);
    }
    SW.CloseSub();
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepElement_RWCurve3dElementDescriptor::Share(
  const occ::handle<StepElement_Curve3dElementDescriptor>&,
  Interface_EntityIterator&) const
{

  // Inherited fields of ElementDescriptor

  // Own fields of Curve3dElementDescriptor
  /* CKY  17JUN04. Content is made of strings and enums. No entity !
    for (int i1=1; i1 <= ent->Purpose()->Length(); i1++ ) {
      occ::handle<NCollection_HSequence<occ::handle<StepElement_CurveElementPurposeMember>>> HSCEPM
    = ent->Purpose()->Value(i1); for (int i2=1; i2 <= HSCEPM->Length(); i2++ ) {
        occ::handle<StepElement_CurveElementPurposeMember> Var1 = HSCEPM->Value(i2);
        iter.AddItem (Var1);
      }
    }
  */
}
