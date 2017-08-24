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

                                                          
#include <inspector/View_ToolBar.hxx>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

const int DEFAULT_COMBO_WIDTH_MINIMUM = 80;
const int DEFAULT_SPACING = 3;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_ToolBar::View_ToolBar (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QWidget (theParent);

  QHBoxLayout* aLay = new QHBoxLayout (myMainWindow);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->setSpacing (DEFAULT_SPACING);

  QWidget* aViewSelectorWidget = new QWidget (myMainWindow);
  QHBoxLayout* aViewSelectorLay = new QHBoxLayout (aViewSelectorWidget);
  aViewSelectorLay->setContentsMargins (0, 0, 0, 0);
  aViewSelectorLay->setContentsMargins (0, 0, 0, 0);
  aViewSelectorLay->addWidget (new QLabel (tr ("Context: "), aViewSelectorWidget));
  myViewSelector = new QComboBox (aViewSelectorWidget);
  myViewSelector->setMinimumWidth (DEFAULT_COMBO_WIDTH_MINIMUM);
  aViewSelectorLay->addWidget (myViewSelector);
  aLay->addWidget (aViewSelectorWidget);
  connect (myViewSelector, SIGNAL (activated (int)), this, SIGNAL (contextChanged()));

  myViewContextNames[View_ContextType_None] = tr ("None");
  myViewContextNames[View_ContextType_Own] = tr ("Own");
  myViewContextNames[View_ContextType_External] = tr ("External");

  myViewSelector->insertItem(View_ContextType_None, myViewContextNames[View_ContextType_None]);
  myViewSelector->insertItem(View_ContextType_Own, myViewContextNames[View_ContextType_Own]);

  myViewSelector->setCurrentIndex(View_ContextType_Own);
  myViewContexts[View_ContextType_None] = Handle(AIS_InteractiveContext)();
  myViewContexts[View_ContextType_Own] = Handle(AIS_InteractiveContext)();
  myViewContexts[View_ContextType_External] = Handle(AIS_InteractiveContext)();

  myActionsMap[View_ToolActionType_KeepViewId] = new QToolButton (theParent);
  myActionsMap[View_ToolActionType_KeepViewId]->setIcon (QIcon (":/icons/keep_view_on.png"));
  myActionsMap[View_ToolActionType_KeepViewId]->setText (tr ("Multi"));
  myActionsMap[View_ToolActionType_KeepViewId]->setToolTip (tr ("Keep View On: does not clear previously shown presentation"));
  myActionsMap[View_ToolActionType_KeepViewId]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  myActionsMap[View_ToolActionType_KeepViewId]->setCheckable (true);
  myActionsMap[View_ToolActionType_KeepViewId]->setChecked (false);

  myActionsMap[View_ToolActionType_KeepViewOffId] = new QToolButton (theParent);
  myActionsMap[View_ToolActionType_KeepViewOffId]->setIcon (QIcon (":/icons/keep_view_off.png"));
  myActionsMap[View_ToolActionType_KeepViewOffId]->setText (QObject::tr ("Single"));
  myActionsMap[View_ToolActionType_KeepViewOffId]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  myActionsMap[View_ToolActionType_KeepViewOffId]->setToolTip (tr ("Keep View Off: clear previously shown presentation"));
  myActionsMap[View_ToolActionType_KeepViewOffId]->setCheckable (true);
  myActionsMap[View_ToolActionType_KeepViewOffId]->setChecked (true);
  
  myActionsMap[View_ToolActionType_ClearViewId] = new QToolButton (theParent);
  myActionsMap[View_ToolActionType_ClearViewId]->setIcon (QIcon (":/icons/view_clear.png"));
  myActionsMap[View_ToolActionType_ClearViewId]->setText (tr ( "Clear View"));
  myActionsMap[View_ToolActionType_ClearViewId]->setToolTip (tr ("Remove all visualized presentations from view context"));

  for (QMap<View_ToolActionType, QToolButton*>::ConstIterator anActionsIt = myActionsMap.begin(),
       anActionsLast = myActionsMap.end(); anActionsIt != anActionsLast; anActionsIt++)
  {
    QToolButton* aBtn = anActionsIt.value();
    connect (aBtn, SIGNAL (clicked()), this, SLOT (onActionClicked()));
    aLay->addWidget (aBtn);
  }
  aLay->addStretch (1);
}

TCollection_AsciiString getPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  std::ostringstream aPtrStr;
  aPtrStr << thePointer.operator->();
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString(aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend("0x");
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void View_ToolBar::SetContext (View_ContextType theType, const Handle(AIS_InteractiveContext)& theContext)
{
  myViewContexts[theType] = theContext;

  QString aViewContextName = myViewContextNames[theType];
  if (!theContext.IsNull())
    aViewContextName = QString ("%1 : [%2]").arg (myViewContextNames[theType])
                                            .arg (getPointerInfo (theContext, true).ToCString());
  // there are only "Own" and "None" items
  if (!theContext.IsNull() && theType == View_ContextType_External && myViewSelector->count() == 2)
    myViewSelector->insertItem (View_ContextType_External, aViewContextName);
  else
    myViewSelector->setItemText (theType, aViewContextName);
}

// =======================================================================
// function : getCurrentContextType
// purpose :
// =======================================================================
View_ContextType View_ToolBar::GetCurrentContextType() const
{
  return (View_ContextType)myViewSelector->currentIndex();
}

// =======================================================================
// function : GetCurrentContext
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) View_ToolBar::GetCurrentContext() const
{
  View_ContextType aCurrentType = (View_ContextType)myViewSelector->currentIndex();
  return myViewContexts[aCurrentType];
}

// =======================================================================
// function : IsActionChecked
// purpose :
// =======================================================================
bool View_ToolBar::IsActionChecked (const int theActionId) const
{
  View_ToolActionType anActionId = (View_ToolActionType)theActionId;
  return myActionsMap.contains (anActionId) ? myActionsMap[anActionId]->isChecked() : false;
}

// =======================================================================
// function : onActionClicked
// purpose :
// =======================================================================
void View_ToolBar::onActionClicked()
{
  int anId = -1;
  QToolButton* aSenderBtn = (QToolButton*)sender();

  for (QMap<View_ToolActionType, QToolButton*>::ConstIterator anActionsIt = myActionsMap.begin(),
       anActionsLast = myActionsMap.end(); anActionsIt != anActionsLast; anActionsIt++)
  {
    if (anActionsIt.value() == aSenderBtn)
    {
      anId = anActionsIt.key();
      break;
    }
  }
  if (anId != -1)
    emit actionClicked (anId);

  if (anId == View_ToolActionType_KeepViewId || anId == View_ToolActionType_KeepViewOffId)
  {
    if (anId == View_ToolActionType_KeepViewId)
      myActionsMap[View_ToolActionType_KeepViewOffId]->setChecked (!aSenderBtn->isChecked());
    else
      myActionsMap[View_ToolActionType_KeepViewId]->setChecked (!aSenderBtn->isChecked());
  }
}
