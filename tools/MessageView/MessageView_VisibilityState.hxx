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

#ifndef MessageView_VisibilityState_H
#define MessageView_VisibilityState_H

#include <inspector/TreeModel_VisibilityState.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/View_Displayer.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>

class TreeModel_ModelBase;

//! \class MessageView_VisibilityState
//! \brief Class provides connection between model and visualization control
class MessageView_VisibilityState : public QObject, public TreeModel_VisibilityState
{
  Q_OBJECT
public:
  //! Constructor
  MessageView_VisibilityState (TreeModel_ModelBase* theModel)
  : TreeModel_VisibilityState (theModel), myPresentationType (View_PresentationType_Main) {}

  //! Destructor
  ~MessageView_VisibilityState() {}

  //! Sets current displayer
  //! \theDisplayer class that provides connection to visualized objects
  void SetDisplayer (View_Displayer* theDisplayer) { myDisplayer = theDisplayer; }

  //! Sets presentation type for displayer
  //! \param theType type value
  void SetPresentationType (const View_PresentationType theType) { myPresentationType = theType; }

  //! Returns true if visibility of the item can be changed
  //! \param theIndex tree model index
  //! \return boolean value
  Standard_EXPORT virtual bool CanBeVisible (const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Sets visibility state
  //! \theIndex tree model index
  //! \param theState visibility state
  //! \param toEmitDataChanged boolean flag whether emit of the model should be done immediatelly
  //! \return true if state is changed
  Standard_EXPORT virtual bool SetVisible (const QModelIndex& theIndex, const bool theState, const bool toEmitDataChanged) Standard_OVERRIDE;

  //! Returns visibility state value
  Standard_EXPORT virtual bool IsVisible (const QModelIndex& theIndex) const Standard_OVERRIDE;

public slots:
  //! Processes the mouse clicked on the index.
  //! It changes the item visibility if model allows to change it.
  //! \theIndex tree model index
  void OnClicked (const QModelIndex& theIndex);

signals:
  //! Signal after OnClicked is performed
  //! \theIndex tree model index
  void itemClicked (const QModelIndex& theIndex);

protected:
  //! Gets the alert item
  //! \theIndex tree model index
  //! \return item or NULL
  MessageModel_ItemAlertPtr getAlertItem (const QModelIndex& theIndex) const;

private:
  View_Displayer* myDisplayer; //! view displayer
  View_PresentationType myPresentationType; //! presentation type
};

#endif
