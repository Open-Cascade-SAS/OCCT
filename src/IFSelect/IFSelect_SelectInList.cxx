// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <IFSelect_SelectInList.hxx>
#include <Interface_EntityIterator.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectInList,IFSelect_SelectAnyList)

// ....    Specialisation de SelectAnyList dans laquelle on traite une liste
//         dont chaque item est une Entite
void IFSelect_SelectInList::FillResult
  (const Standard_Integer n1, const Standard_Integer n2,
   const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& result) const
{
  for (Standard_Integer i = n1; i <= n2; i ++)
    result.GetOneItem (ListedEntity(i,ent));
}
