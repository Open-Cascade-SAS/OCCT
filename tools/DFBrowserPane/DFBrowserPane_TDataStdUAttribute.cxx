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

#include <inspector/DFBrowserPane_TDataStdUAttribute.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_UAttribute.hxx>

#include <QVariant>

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdUAttribute::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_UAttribute) anAttribute = Handle(TDataStd_UAttribute)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;
  char aStr[256];
  theAttribute->ID().ToCString(aStr);
  TCollection_AsciiString aString(aStr);

  theValues.append ("ID");
  theValues.append (DFBrowserPane_Tools::ToString (aString));
}

