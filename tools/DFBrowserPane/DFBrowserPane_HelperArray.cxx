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

#include <inspector/DFBrowserPane_HelperArray.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <qheaderview.h>
#include <QGridLayout>
#include <QList>
#include <QTableView>
#include <QVariant>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_HelperArray::DFBrowserPane_HelperArray (DFBrowserPane_AttributePaneModel* theValuesModel)
 : myValuesModel (theValuesModel), myArrayBounds (0), myValuesView (0)
{
  myBoundsModel = new DFBrowserPane_AttributePaneModel();
  myBoundsModel->SetColumnCount (2);
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
void DFBrowserPane_HelperArray::CreateWidget (QWidget* theParent, DFBrowserPane_TableView* theValuesView)
{
  myValuesView = theValuesView;
  myArrayBounds = new DFBrowserPane_TableView (theParent);
  myArrayBounds->SetModel (myBoundsModel);
  DFBrowserPane_TableView::SetFixedRowCount (2, myArrayBounds->GetTableView());

  QGridLayout* aLay = new QGridLayout (theParent);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->addWidget (myArrayBounds);
  aLay->addWidget (theValuesView);
  aLay->setRowStretch (1, 1);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_HelperArray::Init (const QList<QVariant>& theValues)
{
  QList<QVariant> aTmpValues;
  aTmpValues << "Lower" << theValues[0] << "Upper" << theValues[1];
  myBoundsModel->Init (aTmpValues);

  aTmpValues.clear();
  int aLower = theValues[0].toInt();
  for (int aValueIt = 2, aSize = theValues.size(); aValueIt < aSize; aValueIt++)
    aTmpValues << QString ("Value (%1)").arg(aValueIt-2 + aLower) << theValues[aValueIt];
  myValuesModel->Init (aTmpValues);

  if (myArrayBounds)
    myArrayBounds->GetTableView()->resizeColumnToContents (0);
  if (myValuesView)
    myValuesView->GetTableView()->resizeColumnToContents (0);
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_HelperArray::GetShortAttributeInfo (const Handle(TDF_Attribute)& /*theAttribute*/,
                                                       QList<QVariant>& theValues)
{
  for (int aRowId = 0, aRows = myValuesModel->rowCount(); aRowId < aRows; aRowId++)
    theValues.append (myValuesModel->data (myValuesModel->index (aRowId, 1)));
}
