// File:	RWStepDimTol_RWDatumReference.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepDimTol_RWDatumReference.ixx>

//=======================================================================
//function : RWStepDimTol_RWDatumReference
//purpose  : 
//=======================================================================

RWStepDimTol_RWDatumReference::RWStepDimTol_RWDatumReference ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReference::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepDimTol_DatumReference) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"datum_reference") ) return;

  // Own fields of DatumReference

  Standard_Integer aPrecedence;
  data->ReadInteger (num, 1, "precedence", ach, aPrecedence);

  Handle(StepDimTol_Datum) aReferencedDatum;
  data->ReadEntity (num, 2, "referenced_datum", ach, STANDARD_TYPE(StepDimTol_Datum), aReferencedDatum);

  // Initialize entity
  ent->Init(aPrecedence,
            aReferencedDatum);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReference::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepDimTol_DatumReference) &ent) const
{

  // Own fields of DatumReference

  SW.Send (ent->Precedence());

  SW.Send (ent->ReferencedDatum());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReference::Share (const Handle(StepDimTol_DatumReference) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of DatumReference

  iter.AddItem (ent->ReferencedDatum());
}
