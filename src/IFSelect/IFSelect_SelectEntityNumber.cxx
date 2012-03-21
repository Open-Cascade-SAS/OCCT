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

#include <IFSelect_SelectEntityNumber.ixx>
#include <Interface_InterfaceModel.hxx>


IFSelect_SelectEntityNumber::IFSelect_SelectEntityNumber ()    {  }

    void  IFSelect_SelectEntityNumber::SetNumber
  (const Handle(IFSelect_IntParam)& num)
      {  thenum = num;  }

    Handle(IFSelect_IntParam)  IFSelect_SelectEntityNumber::Number () const 
      {  return thenum;  }

    Interface_EntityIterator  IFSelect_SelectEntityNumber::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  if (num < 1) return iter;        // vide si num < 1 ou num > NbEntities
  if (num <= G.Size()) iter.GetOneItem(G.Entity(num));
  return iter;
}

    TCollection_AsciiString  IFSelect_SelectEntityNumber::Label () const 
{
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  TCollection_AsciiString labl(num);
  labl.InsertBefore (1,"Entity Number ");
  return labl;
}
