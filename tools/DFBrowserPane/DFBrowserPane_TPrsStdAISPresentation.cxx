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

#include <inspector/DFBrowserPane_TPrsStdAISPresentation.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TPrsStd_AISPresentation.hxx>

#include <QVariant>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TPrsStdAISPresentation::DFBrowserPane_TPrsStdAISPresentation()
 : DFBrowserPane_AttributePane()
{
}

// =======================================================================
// function : toString
// purpose :
// =======================================================================
QString toString (const Quantity_NameOfColor& theNameOfColor)
{
  Quantity_Color aColor(theNameOfColor);
  return QString ("(%1, %2, %3)").arg (aColor.Red()).arg (aColor.Green()).arg (aColor.Blue());
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TPrsStdAISPresentation::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TPrsStd_AISPresentation) anAttribute = Handle(TPrsStd_AISPresentation)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;
  Handle(AIS_InteractiveObject) anIO = anAttribute->GetAIS();

  char aStr[256];
  anAttribute->GetDriverGUID().ToCString(aStr);
  TCollection_AsciiString aString(aStr);

  theValues << "GetDriverGUID" << DFBrowserPane_Tools::ToString (aString)
            << "GetAIS" << (anIO.IsNull() ? "Null" : anAttribute->DynamicType()->Name())
            << "IsDisplayed" << DFBrowserPane_Tools::BoolToStr (anAttribute->IsDisplayed())
            << "GetContext()" << ((!anIO.IsNull() && !anIO->GetContext().IsNull()) ?
                                 DFBrowserPane_Tools::GetPointerInfo (anIO->GetContext()).ToCString() : "")
            << "HasOwnMaterial" << DFBrowserPane_Tools::BoolToStr (anAttribute->HasOwnMaterial())
            << "Material" << (anAttribute->HasOwnMaterial() ?
                             DFBrowserPane_Tools::ToName (DB_MATERIAL_TYPE, anAttribute->Material()) : "").ToCString()
            << "Transparency" << TCollection_AsciiString (anAttribute->Transparency()).ToCString()
            << "HasOwnColor" << DFBrowserPane_Tools::BoolToStr (anAttribute->HasOwnColor())
            << "Color" << (anAttribute->HasOwnColor() ? toString (anAttribute->Color()) : "")
            << "HasOwnWidth"<< DFBrowserPane_Tools::BoolToStr (anAttribute->HasOwnWidth())
            << "Width"<< (anAttribute->HasOwnWidth() ? QString::number (anAttribute->Width()) : "")
            << "HasOwnMode"<< DFBrowserPane_Tools::BoolToStr (anAttribute->HasOwnMode())
            << "Width"<< (anAttribute->HasOwnMode() ? DFBrowserPane_Tools::ToName (
                          DB_DISPLAY_MODE, anAttribute->Mode()) : "").ToCString()
            << "HasOwnSelectionMode" << DFBrowserPane_Tools::BoolToStr (anAttribute->HasOwnSelectionMode())
            << "SelectionMode" << (anAttribute->HasOwnSelectionMode() ?
                                   QString::number (anAttribute->SelectionMode()) : "");
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) DFBrowserPane_TPrsStdAISPresentation::GetPresentation (
                                                    const Handle(TDF_Attribute)& theAttribute)
{
  Handle(Standard_Transient) aPresentation;

  Handle(TPrsStd_AISPresentation) anAttribute = Handle(TPrsStd_AISPresentation)::DownCast (theAttribute);
  if (!anAttribute.IsNull())
    aPresentation = anAttribute->GetAIS();

  return aPresentation;
}
