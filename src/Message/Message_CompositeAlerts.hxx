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

#ifndef _Message_CompositeAlerts_HeaderFile
#define _Message_CompositeAlerts_HeaderFile

#include <Message_Alert.hxx>
#include <Message_Gravity.hxx>
#include <Message_ListOfAlert.hxx>
#include <Standard_Transient.hxx>

//! Class providing container of alerts
class Message_CompositeAlerts : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_CompositeAlerts, Standard_Transient)
public:
  //! Empty constructor
  Message_CompositeAlerts() {}

  //! Returns list of collected alerts with specified gravity
  Standard_EXPORT const Message_ListOfAlert& Alerts (const Message_Gravity theGravity) const;

  //! Add alert with specified gravity. If the alert supports merge it will be merged.
  //! @param theGravity an alert gravity
  //! @param theAlert an alert to be added as a child alert
  //! @return true if the alert is added or merged
  Standard_EXPORT Standard_Boolean AddAlert (Message_Gravity theGravity,
                                             const Handle(Message_Alert)& theAlert);

  //! Removes alert with specified gravity.
  //! @param theGravity an alert gravity
  //! @param theAlert an alert to be removed from the children
  //! @return true if the alert is removed
  Standard_EXPORT Standard_Boolean RemoveAlert (Message_Gravity theGravity,
                                                const Handle(Message_Alert)& theAlert);

  //! Returns true if the alert belong the list of the child alerts.
  //! @param theAlert an alert to be checked as a child alert
  //! @return true if the alert is found in a container of children
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Message_Alert)& theAlert);

  //! Returns true if specific type of alert is recorded with specified gravity
  //! @param theType an alert type
  //! @param theGravity an alert gravity
  //! @return true if the alert is found in a container of children
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Standard_Type)& theType,
                                             Message_Gravity theGravity);

  //! Clears all collected alerts
  Standard_EXPORT void Clear();

  //! Clears collected alerts with specified gravity
  //! @param theGravity an alert gravity
  Standard_EXPORT void Clear (Message_Gravity theGravity);

  //! Clears collected alerts with specified type
  //! @param theType an alert type
  Standard_EXPORT void Clear (const Handle(Standard_Type)& theType);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
  // store messages in a lists sorted by gravity;
  // here we rely on knowledge that Message_Fail is the last element of the enum
  Message_ListOfAlert myAlerts[Message_Fail + 1]; //!< container of child alert for each type of gravity
};

DEFINE_STANDARD_HANDLE(Message_CompositeAlerts, Standard_Transient)

#endif // _Message_CompositeAlerts_HeaderFile
