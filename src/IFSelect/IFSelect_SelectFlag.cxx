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

#include <IFSelect_SelectFlag.ixx>


IFSelect_SelectFlag::IFSelect_SelectFlag (const Standard_CString flagname)
    : thename (flagname)    {  }

    Standard_CString  IFSelect_SelectFlag::FlagName () const
      {  return thename.ToCString();  }


    Interface_EntityIterator  IFSelect_SelectFlag::RootResult
  (const Interface_Graph& G) const
{
  Standard_Boolean direct = IsDirect();
  Interface_EntityIterator res;
  const Interface_BitMap& bm = G.BitMap();
  Standard_Integer flag = bm.FlagNumber (thename.ToCString());
  if (flag == 0) return res;
  Interface_EntityIterator inp = InputResult(G);

  for (inp.Start(); inp.More(); inp.Next()) {
    Standard_Integer num = G.EntityNumber(inp.Value());
    if (num == 0) continue;
    if (direct == bm.Value(num,flag)) res.AddItem (G.Entity(num));
  }
  return res;
}

    Standard_Boolean  IFSelect_SelectFlag::Sort
  (const Standard_Integer ,
   const Handle(Standard_Transient)&,
   const Handle(Interface_InterfaceModel)& ) const
      {  return Standard_False;  }

    TCollection_AsciiString  IFSelect_SelectFlag::ExtractLabel () const
{
  TCollection_AsciiString lab ("Entities Flagged by ");
  lab.AssignCat (thename);
  return lab;
}
