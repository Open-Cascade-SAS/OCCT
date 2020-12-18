// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Message_CompositeAlerts.hxx>

#include <Message_AlertExtended.hxx>
#include <Standard_Assert.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_CompositeAlerts, Standard_Transient)

//=======================================================================
//function : Alerts
//purpose  :
//=======================================================================
const Message_ListOfAlert& Message_CompositeAlerts::Alerts (const Message_Gravity theGravity) const
{
  static const Message_ListOfAlert anEmptyList;
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", anEmptyList);
  return myAlerts[theGravity];
}

//=======================================================================
//function : AddAlert
//purpose  :
//=======================================================================
Standard_Boolean Message_CompositeAlerts::AddAlert (Message_Gravity theGravity, const Handle(Message_Alert)& theAlert)
{
  Standard_ASSERT_RETURN (! theAlert.IsNull(), "Attempt to add null alert", Standard_False);
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Adding alert with gravity not in valid range", Standard_False);

  Message_ListOfAlert& aList = myAlerts[theGravity];
  if (theAlert->SupportsMerge() && ! aList.IsEmpty())
  {
    // merge is performed only for alerts of exactly same type
    const Handle(Standard_Type)& aType = theAlert->DynamicType();
    for (Message_ListOfAlert::Iterator anIt(aList); anIt.More(); anIt.Next())
    {
      // if merged successfully, just return
      if (aType == anIt.Value()->DynamicType() && theAlert->Merge (anIt.Value()))
        return Standard_False;
    }
  }

  // if not merged, just add to the list
  aList.Append (theAlert);
  return Standard_True;
}

//=======================================================================
//function : RemoveAlert
//purpose  :
//=======================================================================
Standard_Boolean Message_CompositeAlerts::RemoveAlert (Message_Gravity theGravity,
                                                       const Handle(Message_Alert)& theAlert)
{
  Standard_ASSERT_RETURN (! theAlert.IsNull(), "Attempt to add null alert", Standard_False);
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Adding alert with gravity not in valid range", Standard_False);

  Message_ListOfAlert& anAlerts = myAlerts[theGravity];
  if (!anAlerts.Contains (theAlert))
  {
    return Standard_False;
  }

  return anAlerts.Remove (theAlert);
}

//=======================================================================
//function : HasAlerts
//purpose  :
//=======================================================================
Standard_Boolean Message_CompositeAlerts::HasAlert (const Handle(Message_Alert)& theAlert)
{
  for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter)
  {
    const Message_ListOfAlert& anAlerts = Alerts ((Message_Gravity)aGravIter);
    if (anAlerts.Contains (theAlert))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : HasAlerts
//purpose  :
//=======================================================================
Standard_Boolean Message_CompositeAlerts::HasAlert (const Handle(Standard_Type)& theType, Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", Standard_False);

  for (Message_ListOfAlert::Iterator anIt (myAlerts[theGravity]); anIt.More(); anIt.Next())
  {
    if (anIt.Value()->IsInstance(theType))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void Message_CompositeAlerts::Clear()
{
  for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  {
    myAlerts[i].Clear();
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void Message_CompositeAlerts::Clear (Message_Gravity theGravity)
{
  Standard_ASSERT_RETURN (theGravity >= 0 && size_t(theGravity) < sizeof(myAlerts)/sizeof(myAlerts[0]), 
                          "Requesting alerts for gravity not in valid range", );
  myAlerts[theGravity].Clear();
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void Message_CompositeAlerts::Clear (const Handle(Standard_Type)& theType)
{
  for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  {
    for (Message_ListOfAlert::Iterator anIt (myAlerts[i]); anIt.More(); )
    {
      if (anIt.Value().IsNull() || anIt.Value()->IsInstance (theType))
      {
        myAlerts[i].Remove (anIt);
      }
      else
      {
        anIt.More();
      }
    }
  }
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void Message_CompositeAlerts::DumpJson (Standard_OStream& theOStream,
                                        Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  Standard_Integer anInc = 1;
  for (unsigned int i = 0; i < sizeof(myAlerts)/sizeof(myAlerts[0]); ++i)
  {
    if (myAlerts[i].IsEmpty())
      continue;

    for (Message_ListOfAlert::Iterator anIt (myAlerts[i]); anIt.More(); anIt.Next(), anInc++)
    {
      const Handle(Message_Alert)& anAlert = anIt.Value();
      OCCT_DUMP_FIELD_VALUES_DUMPED_INC (theOStream, theDepth, anAlert.get(), anInc)
    }
  }
}
