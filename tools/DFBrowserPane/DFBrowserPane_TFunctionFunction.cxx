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

#include <inspector/DFBrowserPane_TFunctionFunction.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TFunction_Function.hxx>

#include <QVariant>
// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TFunctionFunction::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TFunction_Function) anAttribute = Handle(TFunction_Function)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;
  char aStr[256];
  anAttribute->GetDriverGUID().ToCString (aStr);
  TCollection_AsciiString aString(aStr);

  theValues.append ("GetDriverGUID");
  theValues.append (DFBrowserPane_Tools::ToString(aString));
}

