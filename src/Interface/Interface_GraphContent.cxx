#include <Interface_GraphContent.ixx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntList.hxx>



    Interface_GraphContent::Interface_GraphContent ()    {  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph)
      {  GetFromGraph(agraph);  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph, const Standard_Integer stat)
      {  GetFromGraph(agraph,stat);  }

    Interface_GraphContent::Interface_GraphContent
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
{
  Interface_IntList list =
    agraph.SharedNums(agraph.EntityNumber(ent));
  Standard_Integer nb = list.Length();
  if (nb == 0) return;                             // Liste redefinie a VIDE
  Handle(Interface_InterfaceModel) mod = agraph.Model();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Standard_Integer num = list.Value(i);
    if (agraph.IsPresent(num)) GetOneItem (agraph.Entity(num));
  }
}


    void  Interface_GraphContent::GetFromGraph (const Interface_Graph& agraph)
{
  Standard_Integer nb = agraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i)) GetOneItem (agraph.Entity(i));
  }
}

    void  Interface_GraphContent::GetFromGraph
  (const Interface_Graph& agraph, const Standard_Integer stat)
{
  Standard_Integer nb = agraph.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i) && agraph.Status(i) == stat)
      GetOneItem (agraph.Entity(i));
  }
}

    Interface_EntityIterator Interface_GraphContent::Result ()
{
  Interface_EntityIterator iter;    // On transvase ...
  for (Begin(); More(); Next()) iter.GetOneItem(Value());
  return iter;
}


    void Interface_GraphContent::Begin ()
{
  Evaluate();
  Interface_EntityIterator::Start();
}

    void Interface_GraphContent::Evaluate ()
{  }    // par defaut, Evaluate ne fait rien
