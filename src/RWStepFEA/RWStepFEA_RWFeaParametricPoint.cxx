// File:	RWStepFEA_RWFeaParametricPoint.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaParametricPoint.ixx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepFEA_RWFeaParametricPoint
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaParametricPoint::RWStepFEA_RWFeaParametricPoint ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaParametricPoint::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepFEA_FeaParametricPoint) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_parametric_point") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaParametricPoint

  Handle(TColStd_HArray1OfReal) aCoordinates;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "coordinates", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aCoordinates = new TColStd_HArray1OfReal (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Standard_Real anIt0;
      data->ReadReal (num2, i0, "real", ach, anIt0);
      aCoordinates->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aCoordinates);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaParametricPoint::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepFEA_FeaParametricPoint) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaParametricPoint

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Coordinates()->Length(); i1++ ) {
    Standard_Real Var0 = ent->Coordinates()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaParametricPoint::Share (const Handle(StepFEA_FeaParametricPoint) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaParametricPoint
}
