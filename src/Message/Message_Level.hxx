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

#ifndef _Message_Level_HeaderFile
#define _Message_Level_HeaderFile

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Gravity.hxx>
#include <Message_Messenger.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard.hxx>

//! This class is an instance of Sentry to create a level in a message report
//! Constructor of the class add new (active) level in the report, destructor removes it
//! While the level is active in the report, new alerts are added below the level root alert.
//!
//! The first added alert is a root alert, other are added below the root alert
//!
//! If alert has Message_AttributeMeter attribute, active metrics of the default report are stored in
//! the attribute: start value of metric on adding alert, stop on adding another alert or closing (delete) the level
//! in the report.
//!
//! Processing of this class is implemented in Message_Report, it is used only inside it.
//! Levels using should be only through using OCCT_ADD_MESSAGE_LEVEL_SENTRY only. No other code is required outside.
class Message_Level
{
public:
  //! Constructor.
  //! One string key is used for all alert meters.
  //! The perf meter is not started automatically, it will be done in AddAlert() method
  Standard_EXPORT Message_Level (const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Assures stopping upon destruction
  Standard_EXPORT ~Message_Level();

  //! Returns root alert of the level
  //! @return alert instance or NULL
  const Handle(Message_AlertExtended)& RootAlert() const { return myRootAlert; }

  //! Sets the root alert. Starts collects alert metrics if active.
  //! @param theAlert an alert  
  Standard_EXPORT void SetRootAlert (const Handle(Message_AlertExtended)& theAlert,
                                     const Standard_Boolean isRequiredToStart);

  //! Adds new alert on the level. Stops the last alert metric, appends the alert and starts the alert metrics collecting.
  //! Sets root alert beforehand this method using, if the root is NULL, it does nothing.
  //! @param theGravity an alert gravity
  //! @param theAlert an alert  
  //! @return true if alert is added
  Standard_EXPORT Standard_Boolean AddAlert (const Message_Gravity theGravity,
                                             const Handle(Message_Alert)& theAlert);

private:
  //! Remove the current level from the report. It stops metric collecting for the last and the root alerts.
  Standard_EXPORT void remove();

private:
  Handle(Message_AlertExtended) myRootAlert; //!< root alert
  Handle(Message_AlertExtended) myLastAlert; //!< last added alert on the root alert
};

//! @def MESSAGE_NEW_LEVEL
//! Creates a new level instance of Sentry. This row should be inserted before messages using in the method.
#define OCCT_ADD_MESSAGE_LEVEL_SENTRY(theMessage) \
  Message_Level aLevel(theMessage);

#endif // _Message_Level_HeaderFile
