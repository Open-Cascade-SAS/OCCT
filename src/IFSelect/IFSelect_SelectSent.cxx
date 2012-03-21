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

#include <IFSelect_SelectSent.ixx>
#include <stdio.h>


IFSelect_SelectSent::IFSelect_SelectSent
  (const Standard_Integer sentcount, const Standard_Boolean atleast)
      {  thecnt = sentcount;  thelst = atleast;  }

    Standard_Integer  IFSelect_SelectSent::SentCount () const
      {  return thecnt;  }

    Standard_Boolean  IFSelect_SelectSent::AtLeast () const
      {  return thelst;  }

    Interface_EntityIterator IFSelect_SelectSent::RootResult
  (const Interface_Graph& G) const
{
  Standard_Boolean direct = IsDirect();
  Interface_EntityIterator res;
  Interface_EntityIterator inp = InputResult(G);

  for (inp.Start(); inp.More(); inp.Next()) {
    Standard_Integer num = G.EntityNumber(inp.Value());
    if (num == 0) continue;
    Standard_Integer nb = G.Status(num);   // nb sent
    Standard_Boolean ok;
    if (thecnt == 0)  ok = (nb == 0);
    else if (thelst)  ok = (nb >= thecnt);
    else              ok = (nb == thecnt);
    if (ok == direct) res.AddItem (G.Entity(num));
  }
  return res;
}

    Standard_Boolean  IFSelect_SelectSent::Sort
  (const Standard_Integer ,
   const Handle(Standard_Transient)&,
   const Handle(Interface_InterfaceModel)& ) const
      {  return Standard_False;  }


    TCollection_AsciiString  IFSelect_SelectSent::ExtractLabel () const
{
  char lb[80];
  TCollection_AsciiString lab;
  if (thecnt == 0) lab.AssignCat ("Remaining (non-sent) entities");
  if (thecnt == 1 &&  thelst) lab.AssignCat ("Sent entities");
  if (thecnt == 1 && !thelst) lab.AssignCat ("Sent once (non-duplicated) entities");
  if (thecnt == 2 &&  thelst) lab.AssignCat ("Sent several times entities");
  if (thecnt == 2 && !thelst) lab.AssignCat ("Sent just twice entities");
  if (thecnt > 2) {
    if (thelst) sprintf (lb,"Sent at least %d times entities", thecnt);
    else        sprintf (lb,"Sent just %d times entities", thecnt);
    lab.AssignCat (lb);
  }
  return lab;
}
