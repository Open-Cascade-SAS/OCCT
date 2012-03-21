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

#include <IFSelect_SelectIntersection.ixx>
#include <IFGraph_Compare.hxx>


IFSelect_SelectIntersection::IFSelect_SelectIntersection ()    {  }


    Interface_EntityIterator  IFSelect_SelectIntersection::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Compare GC(G);
  Standard_Integer nb = NbInputs();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    GC.GetFromIter(Input(i)->RootResult(G), (i==1));
    if (i > 1 && i < nb) {
      Interface_EntityIterator comm = GC.Common();
      GC.ResetData();
      GC.GetFromIter (comm,Standard_True);
    }
  }
  return GC.Common();
}

    TCollection_AsciiString  IFSelect_SelectIntersection::Label () const 
      {  return TCollection_AsciiString("Intersection (AND)");  }
