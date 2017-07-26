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

#include <DFBrowserPane_TDataStdNamedData.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_HelperGroupContent.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_Tools.hxx>

#include <TColStd_DataMapOfStringInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfStringInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <TDataStd_DataMapIteratorOfDataMapOfStringByte.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_HDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_NamedData.hxx>

#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

static const QString VALUES_INTEGER =    "values_integer";
static const QString VALUES_REAL =       "values_real";
static const QString VALUES_STRING =     "values_string";
static const QString VALUES_BYTE =       "values_byte";
static const QString VALUES_INT_ARRAY =  "values_int_array";
static const QString VALUES_REAL_ARRAY = "values_real_array";

//#define REQUIRE_OCAF_REVIEW:9 : start
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TDataStdNamedData::DFBrowserPane_TDataStdNamedData()
 : DFBrowserPane_AttributePane()
{
  myPaneModel = createPaneModel();
  myRealValuesModel = createPaneModel();
  myStringValuesModel = createPaneModel();
  myByteValuesModel = createPaneModel();
  myIntArrayValuesModel = createPaneModel();
  myRealArrayModel = createPaneModel();
}

// =======================================================================
// function : createPaneModel
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneModel* DFBrowserPane_TDataStdNamedData::createPaneModel()
{
  DFBrowserPane_AttributePaneModel* aTableModel = new DFBrowserPane_AttributePaneModel();

  aTableModel->SetOrientation (Qt::Horizontal);

  QList<QVariant> aHeaderValues;
  aHeaderValues << "Name" << "Value";
  aTableModel->SetHeaderValues (aHeaderValues, Qt::Vertical);
  aTableModel->SetColumnCount (2); // indeed these are rows as table orientation is Horizontal

  return aTableModel;
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TDataStdNamedData::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  QVBoxLayout* aLay = new QVBoxLayout (aMainWidget);

  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->SetModel (getPaneModel());
  myIntValuesContent = new DFBrowserPane_HelperGroupContent ("Named integers", aMainWidget, myTableView);
  aLay->addWidget (myIntValuesContent);

  myRealValues = new DFBrowserPane_TableView (aMainWidget);
  myRealValues->SetModel (myRealValuesModel);
  myRealValuesContent = new DFBrowserPane_HelperGroupContent ("Named reals", aMainWidget, myRealValues);
  aLay->addWidget (myRealValuesContent);

  myStringValues = new DFBrowserPane_TableView (aMainWidget);
  myStringValues->SetModel (myStringValuesModel);
  myStringValuesContent = new DFBrowserPane_HelperGroupContent ("Named strings", aMainWidget, myStringValues);
  aLay->addWidget (myStringValuesContent);

  myByteValues = new DFBrowserPane_TableView (aMainWidget);
  myByteValues->SetModel (myByteValuesModel);
  myByteValuesContent = new DFBrowserPane_HelperGroupContent ("Named bytes", aMainWidget, myByteValues);
  aLay->addWidget (myByteValuesContent);

  myIntArrayValues = new DFBrowserPane_TableView (aMainWidget);
  myIntArrayValues->SetModel (myIntArrayValuesModel);
  myIntArrayValuesContent = new DFBrowserPane_HelperGroupContent ("Named integer arrays", aMainWidget, myIntArrayValues);
  aLay->addWidget (myIntArrayValuesContent);

  myRealArrayValues = new DFBrowserPane_TableView (aMainWidget);
  myRealArrayValues->SetModel (myRealArrayModel);
  myRealArrayValuesContent = new DFBrowserPane_HelperGroupContent ("Named real arrays", aMainWidget, myRealArrayValues);
  aLay->addWidget (myRealArrayValuesContent);

  aLay->addStretch (1);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdNamedData::Init (const Handle(TDF_Attribute)& theAttribute)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);

  getPaneModel()->Init (getPartOfValues (VALUES_INTEGER, VALUES_REAL, aValues));
  myRealValuesModel->Init (getPartOfValues (VALUES_REAL, VALUES_STRING, aValues));
  myStringValuesModel->Init (getPartOfValues (VALUES_STRING, VALUES_BYTE, aValues));
  myByteValuesModel->Init (getPartOfValues (VALUES_BYTE, VALUES_INT_ARRAY, aValues));
  myIntArrayValuesModel->Init (getPartOfValues (VALUES_INT_ARRAY, VALUES_REAL_ARRAY, aValues));
  myRealArrayModel->Init (getPartOfValues (VALUES_REAL_ARRAY, "", aValues));
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdNamedData::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_NamedData) anAttribute = Handle(TDataStd_NamedData)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  theValues.append (VALUES_INTEGER);
  for (TColStd_DataMapIteratorOfDataMapOfStringInteger anIntIter(anAttribute->GetIntegersContainer());
       anIntIter.More(); anIntIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (anIntIter.Key()));
    theValues.append (anIntIter.Value());
  }
  theValues.append (VALUES_REAL);
  for (TDataStd_DataMapIteratorOfDataMapOfStringReal aRealIter(anAttribute->GetRealsContainer());
       aRealIter.More(); aRealIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (aRealIter.Key()));
    theValues.append (aRealIter.Value());
  }
  theValues.append (VALUES_STRING);
  for (TDataStd_DataMapIteratorOfDataMapOfStringString aStrIter(anAttribute->GetStringsContainer());
       aStrIter.More(); aStrIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (aStrIter.Key()));
    theValues.append (DFBrowserPane_Tools::ToString (aStrIter.Value()));
  }
  theValues.append (VALUES_BYTE);
  for (TDataStd_DataMapIteratorOfDataMapOfStringByte aByteIter(anAttribute->GetBytesContainer());
       aByteIter.More(); aByteIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (aByteIter.Key()));
    theValues.append (aByteIter.Value());
  }
  theValues.append (VALUES_INT_ARRAY);
  QStringList anArrayValues;
  for (TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger anIntArrayIter(anAttribute->GetArraysOfIntegersContainer());
       anIntArrayIter.More(); anIntArrayIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (anIntArrayIter.Key()));
    anArrayValues.clear();
    const Handle(TColStd_HArray1OfInteger)& aSubIt = anIntArrayIter.Value();
    if (!aSubIt.IsNull())
    {
      for (Standard_Integer aLowerId = aSubIt->Lower(), i = aLowerId, anUpperId = aSubIt->Upper(); i <= anUpperId; i++)
        anArrayValues.append (QString::number (aSubIt->Value (i)));
    }
    theValues.append (anArrayValues.join (QString (',')));
  }
  theValues.append (VALUES_REAL_ARRAY);
  for (TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal aRealArrayIter (anAttribute->GetArraysOfRealsContainer());
       aRealArrayIter.More(); aRealArrayIter.Next())
  {
    theValues.append (DFBrowserPane_Tools::ToString (aRealArrayIter.Key()));
    anArrayValues.clear();
    const Handle(TColStd_HArray1OfReal)& aSubIt = aRealArrayIter.Value();
    if (!aSubIt.IsNull())
    {
      for (Standard_Integer aLowerId = aSubIt->Lower(), i = aLowerId, anUpperId = aSubIt->Upper(); i <= anUpperId; i++)
        anArrayValues.append (QString::number (aSubIt->Value (i)));
    }
    theValues.append (anArrayValues.join (QString (',')));
  }
}

// =======================================================================
// function : getPartOfValues
// purpose :
// =======================================================================
QList<QVariant> DFBrowserPane_TDataStdNamedData::getPartOfValues (const QString& theKey1, const QString& theKey2,
                                                                  const QList<QVariant>& theValues) const
{
  QList<QVariant> aValues;

  bool aFoundKey1 = false, aFoundKey2 = false;
  for (int aValueId = 0; aValueId < theValues.size() && !aFoundKey2; aValueId++)
  {
    QString aValue = theValues[aValueId].toString();
    if (!aFoundKey1)
      aFoundKey1 = aValue == theKey1;
    else
    {
      aFoundKey2 = aValue == theKey2;
      if (!aFoundKey2)
        aValues.append (aValue);
    }
  }
  return aValues;
}
//#define REQUIRE_OCAF_REVIEW:9 : end
