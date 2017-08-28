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

#include <inspector/DFBrowserPane_TDataStdIntPackedMap.hxx>

#include <TDataStd_IntPackedMap.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#include <QVariant>
#include <QWidget>

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdIntPackedMap::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_IntPackedMap) anAttribute = Handle(TDataStd_IntPackedMap)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  int anIndexInMap = 0;
  for (TColStd_MapIteratorOfPackedMapOfInteger aValueIt(anAttribute->GetMap()); aValueIt.More(); aValueIt.Next(), anIndexInMap++)
  {
    theValues.append (QString ("GetMap [%1]").arg(anIndexInMap));
    theValues.append (aValueIt.Key());
  }
}
