// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

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
