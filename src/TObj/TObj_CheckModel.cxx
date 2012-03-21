// Created on: 2007-04-17
// Created by: Michael Sazonov
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_CheckModel.hxx>

#include <TObj_ObjectIterator.hxx>
#include <Message_Msg.hxx>
#include <Message_Status.hxx>
#include <Message_Messenger.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_CheckModel,Message_Algorithm)
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
