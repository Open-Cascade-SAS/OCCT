// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem.ixx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_Unit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepShape_HArray1OfValueQualifier.hxx>
#include <StepShape_ValueQualifier.hxx>


RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem () {}

void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent) const
{

//  Complex Entity : MeasureReprItem + QualifiedreprItem : so, add ReprItem

  //  --- Instance of plex componant : MeasureReprItem

  Standard_Integer num = 0;
  data->NamedForComplex("MEASURE_REPRESENTATION_ITEM",num0,num,ach);

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,2,ach,"measure_representation_item")) return;

  // --- inherited from measure_with_unit : value_component ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num, 1, "value_component", ach, mvc);

  // --- inherited from measure_with_unit : unit_component ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity (num, 2, "unit_component", ach, aUnitComponent);


  //  --- Instance of plex componant : QualifiedReprItem

  data->NamedForComplex("QUALIFIED_REPRESENTATION_ITEM",num0,num,ach);

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,1,ach,"qualified_representation_item")) return;

  // --- own field : qualifiers ---

  Handle(StepShape_HArray1OfValueQualifier) quals;
  Standard_Integer nsub1;
  if (data->ReadSubList (num,1,"qualifiers",ach,nsub1)) {
    Standard_Integer nb1 = data->NbParams(nsub1);
    quals = new StepShape_HArray1OfValueQualifier (1,nb1);
    for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
      StepShape_ValueQualifier VQ;
      if (data->ReadEntity (nsub1,i1,"qualifier",ach,VQ))
	quals->SetValue (i1,VQ);
    }
  }


  //  --- Instance of plex componant : RepresentationItem

  data->NamedForComplex("REPRESENTATION_ITEM",num0,num,ach);

  if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

  // --- inherited field from this component : name ---

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);

  //--- Initialisation of the read entity ---

  ent->Init(aName, mvc,aUnitComponent,quals);
}


void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent) const
{
//  Complex Entity : MeasureReprItem + QualifiedreprItem : so, add ReprItem

  //  --- Instance of plex componant : MeasureReprItem

  SW.StartEntity ("MEASURE_REPRESENTATION_ITEM");

  // --- inherited from measure_with_unit : value_component ---
  SW.Send(ent->Measure()->ValueComponentMember());
  
  // --- inherited from measure_with_unit : unit_component ---
  SW.Send(ent->Measure()->UnitComponent().Value());

  //  --- Instance of plex componant : QualifiedReprItem

  SW.StartEntity ("QUALIFIED_REPRESENTATION_ITEM");

  // --- own field : qualifiers ---
  Standard_Integer i, nbq = ent->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i ++) SW.Send (ent->QualifiersValue(i).Value());
  SW.CloseSub();

  //  --- Instance of plex componant : ReprsentationItem

  SW.StartEntity ("REPRESENTATION_ITEM");

  // --- inherited field name ---

  SW.Send(ent->Name());
}


void RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem::Share(const Handle(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->Measure()->UnitComponent().Value());

  Standard_Integer i, nbq = ent->NbQualifiers();
  for (i = 1; i <= nbq; i ++) iter.AddItem (ent->QualifiersValue(i).Value());
}

