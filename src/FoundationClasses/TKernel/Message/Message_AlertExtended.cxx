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

#include <Message_AlertExtended.hxx>

#include <Message_Attribute.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Report.hxx>

#include <Precision.hxx>
#include <Standard_Assert.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AlertExtended, Message_Alert)

//=================================================================================================

occ::handle<Message_Alert> Message_AlertExtended::AddAlert(const occ::handle<Message_Report>&    theReport,
                                                      const occ::handle<Message_Attribute>& theAttribute,
                                                      const Message_Gravity            theGravity)
{
  occ::handle<Message_AlertExtended> anAlert = new Message_AlertExtended();
  anAlert->SetAttribute(theAttribute);
  theReport->AddAlert(theGravity, anAlert);
  return anAlert;
}

//=================================================================================================

const char* Message_AlertExtended::GetMessageKey() const
{
  if (myAttribute.IsNull())
  {
    return Message_Alert::GetMessageKey();
  }
  return myAttribute->GetMessageKey();
}

//=================================================================================================

occ::handle<Message_CompositeAlerts> Message_AlertExtended::CompositeAlerts(
  const bool theToCreate)
{
  if (myCompositAlerts.IsNull() && theToCreate)
  {
    myCompositAlerts = new Message_CompositeAlerts();
  }
  return myCompositAlerts;
}

//=================================================================================================

bool Message_AlertExtended::SupportsMerge() const
{
  if (myCompositAlerts.IsNull())
  {
    return true;
  }

  // hierarchical alerts can not be merged
  for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter)
  {
    if (!myCompositAlerts->Alerts((Message_Gravity)aGravIter).IsEmpty())
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool Message_AlertExtended::Merge(const occ::handle<Message_Alert>& /*theTarget*/)
{
  // by default, merge trivially
  return false;
}

//=================================================================================================

void Message_AlertExtended::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  if (!myCompositAlerts.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCompositAlerts.get())
  }
  if (!myAttribute.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAttribute.get())
  }
}
