// File:	RWStepShape_RWConnectedFaceSubSet.cxx
// Created:	Fri Jan  4 17:42:43 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepShape_RWConnectedFaceSubSet.ixx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_Face.hxx>

//=======================================================================
//function : RWStepShape_RWConnectedFaceSubSet
//purpose  : 
//=======================================================================

RWStepShape_RWConnectedFaceSubSet::RWStepShape_RWConnectedFaceSubSet ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedFaceSubSet::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepShape_ConnectedFaceSubSet) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"connected_face_sub_set") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of ConnectedFaceSet

  Handle(StepShape_HArray1OfFace) aConnectedFaceSet_CfsFaces;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "connected_face_set.cfs_faces", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    aConnectedFaceSet_CfsFaces = new StepShape_HArray1OfFace (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepShape_Face) anIt0;
      data->ReadEntity (num2, i0, "connected_face_set.cfs_faces", ach, STANDARD_TYPE(StepShape_Face), anIt0);
      aConnectedFaceSet_CfsFaces->SetValue(i0, anIt0);
    }
  }

  // Own fields of ConnectedFaceSubSet

  Handle(StepShape_ConnectedFaceSet) aParentFaceSet;
  data->ReadEntity (num, 3, "parent_face_set", ach, STANDARD_TYPE(StepShape_ConnectedFaceSet), aParentFaceSet);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aConnectedFaceSet_CfsFaces,
            aParentFaceSet);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedFaceSubSet::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepShape_ConnectedFaceSubSet) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Inherited fields of ConnectedFaceSet

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->StepShape_ConnectedFaceSet::CfsFaces()->Length(); i1++ ) {
    Handle(StepShape_Face) Var0 = ent->StepShape_ConnectedFaceSet::CfsFaces()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();

  // Own fields of ConnectedFaceSubSet

  SW.Send (ent->ParentFaceSet());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedFaceSubSet::Share (const Handle(StepShape_ConnectedFaceSubSet) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ConnectedFaceSet

  for (Standard_Integer i1=1; i1 <= ent->StepShape_ConnectedFaceSet::CfsFaces()->Length(); i1++ ) {
    Handle(StepShape_Face) Var0 = ent->StepShape_ConnectedFaceSet::CfsFaces()->Value(i1);
    iter.AddItem (Var0);
  }

  // Own fields of ConnectedFaceSubSet

  iter.AddItem (ent->ParentFaceSet());
}
