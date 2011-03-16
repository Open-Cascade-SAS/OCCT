#include <IFSelect_SelectShared.ixx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>


    IFSelect_SelectShared::IFSelect_SelectShared ()    {  }


// Entites partagees par d autres (a 1 niveau et au sens Strict)

    Interface_EntityIterator  IFSelect_SelectShared::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter = InputResult(G);
  Interface_Graph GG(G);
  for (iter.Start(); iter.More(); iter.Next()) {
    GG.GetFromIter(G.Shareds(iter.Value()),0);
  }
  return Interface_GraphContent(GG);
}

    TCollection_AsciiString  IFSelect_SelectShared::Label () const
      {  return TCollection_AsciiString("Shared Entities (one level)");  }
