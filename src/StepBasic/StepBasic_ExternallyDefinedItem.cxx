// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_SourceItem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ExternallyDefinedItem,Standard_Transient)

//=======================================================================
//function : StepBasic_ExternallyDefinedItem
//purpose  : 
//=======================================================================
StepBasic_ExternallyDefinedItem::StepBasic_ExternallyDefinedItem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::Init (const StepBasic_SourceItem &aItemId,
                                            const Handle(StepBasic_ExternalSource) &aSource)
{

  theItemId = aItemId;

  theSource = aSource;
}

//=======================================================================
//function : ItemId
//purpose  : 
//=======================================================================

StepBasic_SourceItem StepBasic_ExternallyDefinedItem::ItemId () const
{
  return theItemId;
}

//=======================================================================
//function : SetItemId
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::SetItemId (const StepBasic_SourceItem &aItemId)
{
  theItemId = aItemId;
}

//=======================================================================
//function : Source
//purpose  : 
//=======================================================================

Handle(StepBasic_ExternalSource) StepBasic_ExternallyDefinedItem::Source () const
{
  return theSource;
}

//=======================================================================
//function : SetSource
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::SetSource (const Handle(StepBasic_ExternalSource) &aSource)
{
  theSource = aSource;
}
