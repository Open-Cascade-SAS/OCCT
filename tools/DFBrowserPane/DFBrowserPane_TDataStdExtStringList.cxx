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

#include <inspector/DFBrowserPane_TDataStdExtStringList.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

#include <QVariant>
#include <QWidget>

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdExtStringList::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_ExtStringList) anAttribute = Handle(TDataStd_ExtStringList)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  for (TDataStd_ListIteratorOfListOfExtendedString aValuesIt(anAttribute->List()); aValuesIt.More(); aValuesIt.Next())
  {
    theValues.append ("Value");
    theValues.append (DFBrowserPane_Tools::ToString (aValuesIt.Value()));
  }
}
