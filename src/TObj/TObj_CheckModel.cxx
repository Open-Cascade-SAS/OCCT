// Created on: 2007-04-17
// Created by: Michael Sazonov
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_CheckModel.hxx>

#include <TObj_ObjectIterator.hxx>
#include <Message_Msg.hxx>
#include <Message_Status.hxx>
#include <Message_Messenger.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_CheckModel,Message_Algorithm)

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean TObj_CheckModel::Perform()
{
  ClearStatus();
  if (myModel.IsNull() || myModel->GetLabel().IsNull())
  {
    SetStatus (Message_Fail1);
    return Standard_False;
  }
  return checkReferences();
}

//=======================================================================
//function : checkReferences
//purpose  : 
//=======================================================================

Standard_Boolean TObj_CheckModel::checkReferences()
{
  // iterate by all objects in the model
  Handle(TObj_ObjectIterator) anIt;
  for(anIt = myModel->GetObjects(); anIt->More(); anIt->Next()) 
  {
    Handle(TObj_Object) anObj = anIt->Value();
    if (anObj.IsNull()) 
    {
      SetStatus (Message_Alarm1, anIt->DynamicType()->Name());
      continue;
    }
    
    // Check references    
    Handle(TObj_ObjectIterator) aRefIter;
    for ( aRefIter = anObj->GetReferences(); aRefIter->More(); aRefIter->Next()) 
    {
      Handle(TObj_Object) aReferred = aRefIter->Value();
      if (aReferred.IsNull() || ! aReferred->IsAlive())
      {
        SetStatus (Message_Alarm2, anObj->GetName());
        continue;
      }

      // check availability of corresponding back reference
      Handle(TObj_ObjectIterator) aBackIter = aReferred->GetBackReferences();
      if ( aBackIter.IsNull() )
        continue; // object does not support back references

      for ( ; aBackIter->More(); aBackIter->Next() ) 
        if ( aBackIter->Value() == anObj ) break;
      if ( aBackIter->More() ) 
        continue; // ok, back reference found

      if ( IsToFix() ) 
      {
        SetStatus (Message_Warn1, anObj->GetName());
        aReferred->AddBackReference (anObj);
      }
      else
        SetStatus (Message_Alarm4, anObj->GetName());
    }
      
    // Checking back references 
    aRefIter = anObj->GetBackReferences();
    if ( aRefIter.IsNull() )
      continue; // object does not support back references
    TObj_SequenceOfObject aBadBackRefs;
    for ( ; aRefIter->More(); aRefIter->Next()) 
    {
      Handle(TObj_Object) aReferring = aRefIter->Value();
      if (aReferring.IsNull() || ! aReferring->IsAlive())
      {
        SetStatus (Message_Alarm3, anObj->GetName());
        continue;
      }

      Handle(TObj_ObjectIterator) aForwIter = aReferring->GetReferences();
      for ( ; aForwIter->More(); aForwIter->Next()) 
        if ( aForwIter->Value() == anObj ) break;
      if ( aForwIter->More() ) 
        continue; // ok, reference found

      if ( IsToFix() ) 
      {
        SetStatus (Message_Warn2, anObj->GetName());
        aBadBackRefs.Append (aReferring);
      }
      else
        SetStatus (Message_Alarm5, anObj->GetName());
    }

    // remove back references to objects that are not referenced actually
    for ( int i=1; i <= aBadBackRefs.Length(); i++ )
      anObj->RemoveBackReference (aBadBackRefs(i));
  }

  return ! GetStatus().IsAlarm() && ! GetStatus().IsFail();
}
