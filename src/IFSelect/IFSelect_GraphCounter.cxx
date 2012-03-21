// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IFSelect_GraphCounter.ixx>
#include <Interface_EntityIterator.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <stdio.h>
#include <string.h>


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
