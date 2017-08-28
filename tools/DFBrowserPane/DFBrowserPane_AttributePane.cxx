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

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>
#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <AIS_InteractiveObject.hxx>

#include <QGridLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QTableView>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePane::DFBrowserPane_AttributePane()
: DFBrowserPane_AttributePaneAPI(), myMainWidget (0), myTableView (0)
{
  myPaneModel = new DFBrowserPane_AttributePaneModel();

  getPaneModel()->SetColumnCount (getColumnCount());
  getPaneModel()->SetHeaderValues (getHeaderValues(Qt::Horizontal), Qt::Horizontal);

  mySelectionModels.push_back (new QItemSelectionModel (myPaneModel));
}

// =======================================================================
// function : GetWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_AttributePane::GetWidget (QWidget* theParent, const bool isToCreate)
{
  if (!myMainWidget && isToCreate)
    myMainWidget = CreateWidget (theParent);
  return myMainWidget;
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_AttributePane::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  aMainWidget->setVisible (false);

  myTableView = new DFBrowserPane_TableView (aMainWidget, getTableColumnWidths());
  myTableView->SetModel (myPaneModel);
  QTableView* aTableView = myTableView->GetTableView();
  DFBrowserPane_AttributePaneModel* aPaneModel = dynamic_cast<DFBrowserPane_AttributePaneModel*>(myPaneModel);
  if (aPaneModel)
  {
    if (aPaneModel->GetOrientation() == Qt::Vertical)
      aTableView->horizontalHeader()->setVisible (!aPaneModel->HeaderValues (Qt::Horizontal).isEmpty());
    else
      aTableView->verticalHeader()->setVisible (!aPaneModel->HeaderValues (Qt::Vertical).isEmpty());
  }
  aTableView->setSelectionModel (mySelectionModels.front());
  aTableView->setSelectionBehavior (QAbstractItemView::SelectRows);

  QGridLayout* aLay = new QGridLayout (aMainWidget);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->addWidget (myTableView);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_AttributePane::Init (const Handle(TDF_Attribute)& theAttribute)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);
  getPaneModel()->Init (aValues);

  if (myTableView)
    myTableView->GetTableView()->resizeColumnToContents (0);
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPane_AttributePane::GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                        int theRole, int theColumnId)
{
  switch (theRole)
  {
    case DFBrowserPane_ItemRole_ShortInfo:
    {
      QList<QVariant> aValues;
      GetShortAttributeInfo (theAttribute, aValues);
      QStringList anInfoList;
      for (QList<QVariant>::const_iterator aValuesIt = aValues.begin(); aValuesIt != aValues.end(); aValuesIt++)
        anInfoList.append (aValuesIt->toString());
      return QVariant (anInfoList.join (", "));
    }
    default:
      return DFBrowserPane_AttributePane::GetAttributeInfoByType (theAttribute.IsNull() ? ""
                                            : theAttribute->DynamicType()->Name(), theRole, theColumnId);
  }
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_AttributePane::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  QList<QVariant> aValues;
  GetValues(theAttribute, aValues);

  for (int aValuesEvenId  = 1; aValuesEvenId < aValues.size(); aValuesEvenId = aValuesEvenId + 2)
    theValues.append (aValues[aValuesEvenId]);
}

// =======================================================================
// function : GetAttributeInfoByType
// purpose :
// =======================================================================
QVariant DFBrowserPane_AttributePane::GetAttributeInfoByType (const Standard_CString& theAttributeName,
                                                              int theRole, int theColumnId)
{
  if (theColumnId != 0)
    return QVariant();

  switch (theRole)
  {
    case Qt::DisplayRole:
    case Qt::ToolTipRole:    return QVariant (theAttributeName);
    case Qt::DecorationRole: return QIcon (":/icons/attribute.png");
    case DFBrowserPane_ItemRole_Decoration_40x40: return QIcon (":/icons/attribute_40x40.png");
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : getPaneModel
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneModel* DFBrowserPane_AttributePane::getPaneModel() const
{
  return dynamic_cast<DFBrowserPane_AttributePaneModel*> (myPaneModel);
}

// =======================================================================
// function : getTableColumnWidths
// purpose :
// =======================================================================
QMap<int, int> DFBrowserPane_AttributePane::getTableColumnWidths() const
{
  QMap<int, int> aValues;
  for (int aColumnId = 0, aCount = getPaneModel()->columnCount(); aColumnId < aCount; aColumnId++)
    aValues.insert (aColumnId, DFBrowserPane_Tools::DefaultPanelColumnWidth (aColumnId));
  return aValues;
}
