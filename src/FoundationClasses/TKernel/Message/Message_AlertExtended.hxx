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

#ifndef _Message_AlertExtended_HeaderFile
#define _Message_AlertExtended_HeaderFile

#include <Message_Alert.hxx>
#include <Message_Gravity.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Attribute;
class Message_Report;

class Message_CompositeAlerts;

//! Inherited class of Message_Alert with some additional information.
//! It has Message_Attributes to provide the alert name, and other custom information
//! It has a container of composite alerts, if the alert might provide
//! sub-alerts collecting.
class Message_AlertExtended : public Message_Alert
{
public:
  //! Creates new instance of the alert and put it into report with Message_Info gravity.
  //! It does nothing if such kind of gravity is not active in the report
  //! @param theReport the message report where new alert is placed
  //! @param theAttribute container of additional values of the alert
  //! @return created alert or NULL if Message_Info is not active in report
  Standard_EXPORT static Handle(Message_Alert) AddAlert(
    const Handle(Message_Report)&    theReport,
    const Handle(Message_Attribute)& theAttribute,
    const Message_Gravity            theGravity);

public:
  //! Empty constructor
  Message_AlertExtended()
      : Message_Alert()
  {
  }

  //! Return a C string to be used as a key for generating text user messages describing this alert.
  //! The messages are generated with help of Message_Msg class, in Message_Report::Dump().
  //! Base implementation returns dynamic type name of the instance.
  Standard_EXPORT virtual Standard_CString GetMessageKey() const Standard_OVERRIDE;

  //! Returns container of the alert attributes
  const Handle(Message_Attribute)& Attribute() const { return myAttribute; }

  //! Sets container of the alert attributes
  //! @param theAttributes an attribute values
  void SetAttribute(const Handle(Message_Attribute)& theAttribute) { myAttribute = theAttribute; }

  //! Returns class provided hierarchy of alerts if created or create if the parameter is true
  //! @param theToCreate if composite alert has not been created for this alert, it should be
  //! created
  //! @return instance or NULL
  Standard_EXPORT Handle(Message_CompositeAlerts) CompositeAlerts(
    const Standard_Boolean theToCreate = Standard_False);

  //! Return true if this type of alert can be merged with other
  //! of the same type to avoid duplication.
  //! Hierarchical alerts can not be merged
  //! Basis implementation returns true.
  Standard_EXPORT virtual Standard_Boolean SupportsMerge() const Standard_OVERRIDE;

  //! If possible, merge data contained in this alert to theTarget.
  //! Base implementation always returns false.
  //! @return True if merged
  Standard_EXPORT virtual Standard_Boolean Merge(const Handle(Message_Alert)& theTarget)
    Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Message_AlertExtended, Message_Alert)

protected:
  // clang-format off
  Handle(Message_CompositeAlerts) myCompositAlerts; //!< class provided hierarchical structure of alerts
  // clang-format on
  Handle(Message_Attribute) myAttribute; //!< container of the alert attributes
};

DEFINE_STANDARD_HANDLE(Message_AlertExtended, Message_Alert)

#endif // _Message_Alert_HeaderFile
