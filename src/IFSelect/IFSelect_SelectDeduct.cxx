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

#include <IFSelect_SelectDeduct.ixx>


void  IFSelect_SelectDeduct::SetInput (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    Handle(IFSelect_Selection)  IFSelect_SelectDeduct::Input () const 
      {  return thesel;  }

    Standard_Boolean  IFSelect_SelectDeduct::HasInput () const 
      {  return (!thesel.IsNull());  }

    Standard_Boolean  IFSelect_SelectDeduct::HasAlternate () const 
{  if (!thealt.IsNull()) return thealt->IsSet();  return Standard_False;  }

    Handle(IFSelect_SelectPointed)&  IFSelect_SelectDeduct::Alternate ()
{
  if (thealt.IsNull()) thealt = new IFSelect_SelectPointed;
  return thealt;
}


    Interface_EntityIterator  IFSelect_SelectDeduct::InputResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator res;
  if (!thealt.IsNull()) {
    if (thealt->IsSet()) {
      res = thealt->UniqueResult (G);
      thealt->Clear();
      return res;
    }
  }
  if (thesel.IsNull()) return res;
  return  thesel->UniqueResult(G);
}

    void  IFSelect_SelectDeduct::FillIterator
  (IFSelect_SelectionIterator& iter) const 
      {  iter.AddItem(thesel);  }
