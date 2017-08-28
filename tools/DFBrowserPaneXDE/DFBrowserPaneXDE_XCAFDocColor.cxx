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

#include <inspector/DFBrowserPaneXDE_XCAFDocColor.hxx>
#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <XCAFDoc_Color.hxx>

#include <Quantity_Color.hxx>

#include <QColor>
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_XCAFDocColor::DFBrowserPaneXDE_XCAFDocColor()
: DFBrowserPane_AttributePane()
{
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocColor::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Color) anAttr = Handle(XCAFDoc_Color)::DownCast (theAttribute);
  const Quantity_Color& aColor = anAttr->GetColor();

  theValues << "GetColor: Red" << aColor.Red();
  theValues << "GetColor: Green" << aColor.Green();
  theValues << "GetColor: Blue" << aColor.Blue();

  theValues << "GetColor: Name" << aColor.Name();
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocColor::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                           QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Color) anAttr = Handle(XCAFDoc_Color)::DownCast (theAttribute);
  const Quantity_Color& aColor = anAttr->GetColor();
  theValues.append (QString ("(%1, %2, %3)").arg (aColor.Red()).arg (aColor.Green()).arg (aColor.Blue()));
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPaneXDE_XCAFDocColor::GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute, int theRole, int theColumnId)
{
  if (theColumnId != 0)
    return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);

  switch (theRole)
  {
    case Qt::BackgroundRole:
    {
      const Quantity_Color& aColor = Handle(XCAFDoc_Color)::DownCast (theAttribute)->GetColor();
      return QColor ((int)(aColor.Red()*255.), (int)(aColor.Green()*255.), (int)(aColor.Blue()*255.));
    }
    default:
    break;
  }
  return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);
}
