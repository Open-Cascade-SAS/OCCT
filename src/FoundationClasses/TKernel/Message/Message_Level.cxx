// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Message_Level.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_Report.hxx>

#include <OSD_Chronometer.hxx>

//=================================================================================================

Message_Level::Message_Level(const TCollection_AsciiString& theName)
{
  const Handle(Message_Report)& aDefaultReport = Message::DefaultReport();
  if (!aDefaultReport.IsNull() && aDefaultReport->IsActiveInMessenger())
  {
    aDefaultReport->AddLevel(this, theName);
  }
}

//=================================================================================================

Message_Level::~Message_Level()
{
  remove();
}

//=================================================================================================

void Message_Level::SetRootAlert(const Handle(Message_AlertExtended)& theAlert,
                                 const Standard_Boolean               isRequiredToStart)
{
  myRootAlert = theAlert;
  if (isRequiredToStart)
  {
    Message_AttributeMeter::StartAlert(myRootAlert);
  }
}

//=================================================================================================

Standard_Boolean Message_Level::AddAlert(const Message_Gravity        theGravity,
                                         const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anAlertExtended = Handle(Message_AlertExtended)::DownCast(theAlert);
  if (anAlertExtended.IsNull())
  {
    return Standard_False;
  }

  // looking for the parent of the parameter alert to release the previous alert
  Handle(Message_AlertExtended)   aRootAlert      = myRootAlert;
  Handle(Message_CompositeAlerts) aCompositeAlert = aRootAlert->CompositeAlerts(Standard_True);

  // update metrics of the previous alert
  Message_AttributeMeter::StopAlert(myLastAlert);

  myLastAlert = anAlertExtended;
  // set start metrics of the new alert
  Message_AttributeMeter::StartAlert(myLastAlert);

  // add child alert
  aCompositeAlert->AddAlert(theGravity, theAlert);

  return Standard_True;
}

//=================================================================================================

void Message_Level::remove()
{
  const Handle(Message_Report)& aDefaultReport = Message::DefaultReport();
  if (aDefaultReport.IsNull() || !aDefaultReport->IsActiveInMessenger())
  {
    return;
  }

  Message_AttributeMeter::StopAlert(myLastAlert);

  if (!Message::DefaultReport().IsNull())
  {
    Message::DefaultReport()->RemoveLevel(this);
  }
}
