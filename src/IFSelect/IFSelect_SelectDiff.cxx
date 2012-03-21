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

#include <IFSelect_SelectDiff.ixx>
#include <IFGraph_Compare.hxx>


IFSelect_SelectDiff::IFSelect_SelectDiff ()    {  }


    Interface_EntityIterator  IFSelect_SelectDiff::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Compare GC(G);
  GC.GetFromIter(MainInput()->RootResult(G),Standard_True);    // first
  if (HasSecondInput())
    GC.GetFromIter(SecondInput()->RootResult(G),Standard_False);   // second
  return GC.FirstOnly();
}

    Standard_Boolean  IFSelect_SelectDiff::HasUniqueResult () const 
      {  return Standard_True;  }    // naturellement unique

    TCollection_AsciiString  IFSelect_SelectDiff::Label () const 
      {  return TCollection_AsciiString("Differences");  }
