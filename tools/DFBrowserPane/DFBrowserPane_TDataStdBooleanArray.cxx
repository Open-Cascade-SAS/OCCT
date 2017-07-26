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

#include <DFBrowserPane_TDataStdBooleanArray.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_HelperArray.hxx>

#include <TDataStd_BooleanArray.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#include <QVariant>
#include <QWidget>
#include <QGridLayout>

//#define REQUIRE_OCAF_REVIEW:5 : start
// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TDataStdBooleanArray::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->SetModel (getPaneModel());

  myArrayTableHelper.CreateWidget (aMainWidget, myTableView);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdBooleanArray::Init (const Handle(TDF_Attribute)& theAttribute)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);
  myArrayTableHelper.Init (aValues);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdBooleanArray::GetValues (const Handle(TDF_Attribute)& theAttribute,
                                                    QList<QVariant>& theValues)
{
  Handle(TDataStd_BooleanArray) anAttribute = Handle(TDataStd_BooleanArray)::DownCast (theAttribute);
  if (!anAttribute.IsNull())
  {
    theValues.append (anAttribute->Lower());
    theValues.append (anAttribute->Upper());
    for (int aValuesId = anAttribute->Lower(); aValuesId <= anAttribute->Upper(); aValuesId++)
      theValues.append (anAttribute->Value (aValuesId));
  }
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdBooleanArray::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                                QList<QVariant>& theValues)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);
  myArrayTableHelper.Init (aValues);
  return myArrayTableHelper.GetShortAttributeInfo (theAttribute, theValues);
}
//#define REQUIRE_OCAF_REVIEW:5 : end
