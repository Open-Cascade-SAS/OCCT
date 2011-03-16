// File:	StepDimTol_DatumReference.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_DatumReference.ixx>

//=======================================================================
//function : StepDimTol_DatumReference
//purpose  : 
//=======================================================================

StepDimTol_DatumReference::StepDimTol_DatumReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_DatumReference::Init (const Standard_Integer aPrecedence,
                                      const Handle(StepDimTol_Datum) &aReferencedDatum)
{

  thePrecedence = aPrecedence;

  theReferencedDatum = aReferencedDatum;
}

//=======================================================================
//function : Precedence
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_DatumReference::Precedence () const
{
  return thePrecedence;
}

//=======================================================================
//function : SetPrecedence
//purpose  : 
//=======================================================================

void StepDimTol_DatumReference::SetPrecedence (const Standard_Integer aPrecedence)
{
  thePrecedence = aPrecedence;
}

//=======================================================================
//function : ReferencedDatum
//purpose  : 
//=======================================================================

Handle(StepDimTol_Datum) StepDimTol_DatumReference::ReferencedDatum () const
{
  return theReferencedDatum;
}

//=======================================================================
//function : SetReferencedDatum
//purpose  : 
//=======================================================================

void StepDimTol_DatumReference::SetReferencedDatum (const Handle(StepDimTol_Datum) &aReferencedDatum)
{
  theReferencedDatum = aReferencedDatum;
}
