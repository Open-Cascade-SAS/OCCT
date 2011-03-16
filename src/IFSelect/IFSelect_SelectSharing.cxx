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
