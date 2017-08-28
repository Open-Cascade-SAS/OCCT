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

#include <inspector/DFBrowserPaneXDE_XCAFDocDatum.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <Standard_Version.hxx>
#include <TCollection_HAsciiString.hxx>

#include <XCAFDoc_Datum.hxx>
#if OCC_VERSION_HEX > 0x060901
#include <XCAFDimTolObjects_DatumObject.hxx>
#endif

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_XCAFDocDatum::DFBrowserPaneXDE_XCAFDocDatum()
: DFBrowserPane_AttributePane()
{
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocDatum::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Datum) anAttr = Handle(XCAFDoc_Datum)::DownCast (theAttribute);

  Handle(TCollection_HAsciiString) aName = anAttr->GetName();
  Handle(TCollection_HAsciiString) aDescription = anAttr->GetDescription();
  Handle(TCollection_HAsciiString) anIndentification = anAttr->GetIdentification();

  theValues << "Name" << (!aName.IsNull() ? aName->ToCString() : QString (""))
            << "Description" << (!aDescription.IsNull() ? aDescription->ToCString() : QString (""))
            << "Indentification" << (!anIndentification.IsNull() ? anIndentification->ToCString() : QString (""));

#if OCC_VERSION_HEX > 0x060901
  Handle(XCAFDimTolObjects_DatumObject) anObject = anAttr->GetObject();
  Handle(TCollection_HAsciiString) anObjectName;
  if (!anObject.IsNull())
    anObjectName = anObject->GetName();
  theValues << "Object" << (!anObjectName.IsNull() ? anObjectName->ToCString() : "Empty Name");
#endif
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocDatum::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(XCAFDoc_Datum) anAttr = Handle(XCAFDoc_Datum)::DownCast (theAttribute);

  Handle(TCollection_HAsciiString) aName = anAttr->GetName();
  theValues << (!aName.IsNull() ? aName->ToCString() : QString (""));
}
