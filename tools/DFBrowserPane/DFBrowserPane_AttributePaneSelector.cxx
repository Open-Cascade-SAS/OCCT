// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/DFBrowserPane_AttributePaneSelector.hxx>

#include <QItemSelectionModel>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneSelector::DFBrowserPane_AttributePaneSelector (QObject* theParent)
: QObject (theParent), mySendSelectionChangeBlocked (false)
{
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneSelector::~DFBrowserPane_AttributePaneSelector()
{
  SetCurrentSelectionModels (std::list<QItemSelectionModel*>());
}

// =======================================================================
// function : SetCurrentSelectionModels
// purpose :
// =======================================================================
void DFBrowserPane_AttributePaneSelector::SetCurrentSelectionModels (const std::list<QItemSelectionModel*>& theModels)
{
  for (std::list<QItemSelectionModel*>::const_iterator anModelsIt = mySelectionModels.begin(),
       aLast = mySelectionModels.end(); anModelsIt != aLast; anModelsIt++)
    disconnect (*anModelsIt, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
                this, SLOT (onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));

  mySelectionModels = theModels;
  for (std::list<QItemSelectionModel*>::const_iterator anModelsIt = mySelectionModels.begin(),
       aLast = mySelectionModels.end(); anModelsIt != aLast; anModelsIt++)
    connect (*anModelsIt, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
             this, SLOT (onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));
}

// =======================================================================
// function : ClearSelected
// purpose :
// =======================================================================
void DFBrowserPane_AttributePaneSelector::ClearSelected()
{
  mySendSelectionChangeBlocked = true;
  for (std::list<QItemSelectionModel*>::const_iterator anModelsIt = mySelectionModels.begin(),
       aLast = mySelectionModels.end(); anModelsIt != aLast; anModelsIt++)
    (*anModelsIt)->clearSelection();
  mySendSelectionChangeBlocked = false;
}

// =======================================================================
// function : 
// purpose :
// =======================================================================
void DFBrowserPane_AttributePaneSelector::onTableSelectionChanged (const QItemSelection& theSelected,
                                                                   const QItemSelection& theDeselected)
{
  if (mySendSelectionChangeBlocked)
    return;

  QItemSelectionModel* aModel = (QItemSelectionModel*)sender();
  emit tableSelectionChanged (theSelected, theDeselected, aModel);
}
