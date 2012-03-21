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

#include <IFSelect_SelectSharing.ixx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>


IFSelect_SelectSharing::IFSelect_SelectSharing ()    {  }


// Entites en partageant d autres (a 1 niveau et au sens Strict)

    Interface_EntityIterator  IFSelect_SelectSharing::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter = InputResult(G);
  Interface_Graph GG(G);
  for (iter.Start(); iter.More(); iter.Next()) {
    GG.GetFromIter(G.Sharings(iter.Value()),0);
  }
  return Interface_GraphContent(GG);
}

    TCollection_AsciiString  IFSelect_SelectSharing::Label () const
      {  return TCollection_AsciiString("Sharing Entities (one level)");  }
