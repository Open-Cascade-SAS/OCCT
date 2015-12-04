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


#include <IFSelect_GraphCounter.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Type.hxx>

#include <stdio.h>
#include <string.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_GraphCounter,IFSelect_SignCounter)

IFSelect_GraphCounter::IFSelect_GraphCounter
  (const Standard_Boolean withmap, const Standard_Boolean withlist)
  : IFSelect_SignCounter (withmap,withlist)  {  }

    Handle(IFSelect_SelectDeduct)  IFSelect_GraphCounter::Applied () const
      {  return theapplied;  }

    void  IFSelect_GraphCounter::SetApplied
  (const Handle(IFSelect_SelectDeduct)& applied)
      {  theapplied = applied;  }

    void  IFSelect_GraphCounter::AddWithGraph
  (const Handle(TColStd_HSequenceOfTransient)& list,
   const Interface_Graph& graph)
{
  if (theapplied.IsNull())  {  AddList (list,graph.Model());  return;  }
  if (list.IsNull()) return;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++) {
    char val[12];
    Handle(Standard_Transient) ent = list->Value(i);
    theapplied->Alternate()->SetEntity (ent);
    Interface_EntityIterator iter = theapplied->UniqueResult(graph);
    Standard_Integer n = iter.NbEntities();
    switch (n) {
    case 0: Add (ent,"0"); break;
    case 1: Add (ent,"1"); break;
    case 2: Add (ent,"2"); break;
    case 3: Add (ent,"3"); break;
    case 4: Add (ent,"4"); break;
    case 5: Add (ent,"5"); break;
    case 6: Add (ent,"6"); break;
    case 7: Add (ent,"7"); break;
    case 8: Add (ent,"8"); break;
    case 9: Add (ent,"9"); break;
    default :
      sprintf (val,"%d",n);
      Add (ent,val);
      break;
    }
  }
}
