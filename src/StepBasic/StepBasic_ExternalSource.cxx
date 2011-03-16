// File:	StepBasic_ExternalSource.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_ExternalSource.ixx>

//=======================================================================
//function : StepBasic_ExternalSource
//purpose  : 
//=======================================================================

StepBasic_ExternalSource::StepBasic_ExternalSource ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ExternalSource::Init (const StepBasic_SourceItem &aSourceId)
{

  theSourceId = aSourceId;
}

//=======================================================================
//function : SourceId
//purpose  : 
//=======================================================================

StepBasic_SourceItem StepBasic_ExternalSource::SourceId () const
{
  return theSourceId;
}

//=======================================================================
//function : SetSourceId
//purpose  : 
//=======================================================================

void StepBasic_ExternalSource::SetSourceId (const StepBasic_SourceItem &aSourceId)
{
  theSourceId = aSourceId;
}
