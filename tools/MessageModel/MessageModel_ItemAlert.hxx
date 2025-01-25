// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageModel_ItemAlert_H
#define MessageModel_ItemAlert_H

#include <inspector/MessageModel_ItemBase.hxx>

#include <Message_Alert.hxx>
#include <Message_ListOfAlert.hxx>
#include <Message_Report.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class QAbstractTableModel;
class MessageModel_ItemAlert;

typedef QExplicitlySharedDataPointer<MessageModel_ItemAlert> MessageModel_ItemAlertPtr;

//! \class MessageModel_ItemAlert
//! This item is connected to Message_Alert.
//! Parent is either MessageModel_ItemRoot or MessageModel_ItemAlert, children are
//! MessageModel_ItemAlert or no children
class MessageModel_ItemAlert : public MessageModel_ItemBase
{
public:
  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  //! \return the pointer to the created item
  static MessageModel_ItemAlertPtr CreateItem(TreeModel_ItemBasePtr theParent,
                                              const int             theRow,
                                              const int             theColumn)
  {
    return MessageModel_ItemAlertPtr(new MessageModel_ItemAlert(theParent, theRow, theColumn));
  }

  //! Destructor
  virtual ~MessageModel_ItemAlert() Standard_OVERRIDE {};

  //! Returns the current shape
  const Handle(Message_Alert)& GetAlert() const { return myAlert; }

  //! Returns alert of the report for the parameter row
  Standard_Boolean GetChildAlerts(const int theRow, Message_ListOfAlert& theAlerts) const
  {
    return myChildAlerts.Find(theRow, theAlerts);
  }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Return data value for the role.
  //! \param theRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue(const int theRole) const Standard_OVERRIDE;

  //! \return number of children.
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual bool SetStream(const Standard_SStream& theSStream,
                                         Standard_Integer&       theStartPos,
                                         Standard_Integer& theLastPos) const Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \thePresentations[out]  container of presentation handles to be visualized
  virtual void Presentations(NCollection_List<Handle(Standard_Transient)>& thePresentations)
    Standard_OVERRIDE
  {
    TreeModel_ItemBase::Presentations(thePresentations);
    thePresentations.Append(myPresentation);
  }

protected:
  //! Initialize the current item.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream(Standard_OStream& theOStream) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild(int theRow, int theColumn) Standard_OVERRIDE;

  //! Returns current alert, initialized item if it has not been initialized yet
  //! \return alert value
  const Handle(Message_Alert)& getAlert() const;

private:
  //! Constructor
  MessageModel_ItemAlert(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
      : MessageModel_ItemBase(theParent, theRow, theColumn)
  {
  }

private:
  Handle(Message_Alert) myAlert; //!< current alert item
  NCollection_DataMap<Standard_Integer, Message_ListOfAlert>
                             myChildAlerts;  //!< container of child alerts
  Handle(Standard_Transient) myPresentation; //!< item presentation
};

#endif
