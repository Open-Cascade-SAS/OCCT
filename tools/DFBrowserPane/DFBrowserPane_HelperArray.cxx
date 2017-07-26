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

#include <DFBrowserPane_HelperArray.hxx>
#include <DFBrowserPane_TableView.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>

#include <QList>
#include <QVariant>
#include <QGridLayout>
#include <QWidget>

//#define REQUIRE_OCAF_REVIEW:6 : start
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_HelperArray::DFBrowserPane_HelperArray (DFBrowserPane_AttributePaneModel* theValuesModel)
 : myValuesModel (theValuesModel)
{
  myBoundsModel = new DFBrowserPane_AttributePaneModel();

  QList<QVariant> aHeaderValues;
  aHeaderValues << "Bounds";
  myBoundsModel->SetHeaderValues (aHeaderValues, Qt::Horizontal);
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
void DFBrowserPane_HelperArray::CreateWidget (QWidget* theParent, QWidget* theValuesView)
{
  myArrayBounds = new DFBrowserPane_TableView (theParent);
  myArrayBounds->SetModel (myBoundsModel);
  DFBrowserPane_TableView::SetFixedRowCount (2, myArrayBounds->GetTableView());

  QGridLayout* aLay = new QGridLayout (theParent);
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
  aTmpValues << theValues[0] << theValues[1];
  myBoundsModel->Init (aTmpValues);

  aTmpValues.clear();
  QList<QVariant> aHeaderValues;
  int aLower = theValues[0].toInt();
  for (int aValuesIt = 2, aSize = theValues.size(); aValuesIt < aSize; aValuesIt++)
  {
    aTmpValues << theValues[aValuesIt];
    aHeaderValues << aLower + (aValuesIt - 2);
  }
  myValuesModel->SetHeaderValues (aHeaderValues, Qt::Vertical);
  myValuesModel->Init (aTmpValues);
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_HelperArray::GetShortAttributeInfo (const Handle(TDF_Attribute)& /*theAttribute*/,
                                                       QList<QVariant>& theValues)
{
  for (int aRowId = 0, aRows = myValuesModel->rowCount(); aRowId < aRows; aRowId++)
    theValues.append (myValuesModel->data (myValuesModel->index (aRowId, 0)));
}
//#define REQUIRE_OCAF_REVIEW:6 : end
