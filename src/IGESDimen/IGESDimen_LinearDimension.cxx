// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_LinearDimension.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_LinearDimension,IGESData_IGESEntity)

IGESDimen_LinearDimension::IGESDimen_LinearDimension ()    {  }


    void  IGESDimen_LinearDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader,
   const Handle(IGESDimen_WitnessLine)& aWitness,
   const Handle(IGESDimen_WitnessLine)& anotherWitness)
{
  theNote          = aNote;
  theFirstLeader   = aLeader;
  theSecondLeader  = anotherLeader;
  theFirstWitness  = aWitness;        
  theSecondWitness = anotherWitness;        
  InitTypeAndForm(216,FormNumber());
//  FormNumber : Nature of the Dimension 0-1-2
}


    void  IGESDimen_LinearDimension::SetFormNumber (const Standard_Integer fm)
{
  if (fm < 0 || fm > 2) throw Standard_OutOfRange("IGESDimen_LinearDimension : SetFormNumber");
  InitTypeAndForm(216,fm);
}


    Handle(IGESDimen_GeneralNote)  IGESDimen_LinearDimension::Note () const 
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_LinearDimension::FirstLeader () const 
{
  return theFirstLeader;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_LinearDimension::SecondLeader () const 
{
  return theSecondLeader;
}

    Standard_Boolean  IGESDimen_LinearDimension::HasFirstWitness () const 
{
  return (! theFirstWitness.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_LinearDimension::FirstWitness () const 
{
  return theFirstWitness;
}

    Standard_Boolean  IGESDimen_LinearDimension::HasSecondWitness () const 
{
  return (! theSecondWitness.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_LinearDimension::SecondWitness () const 
{
  return theSecondWitness;
}
