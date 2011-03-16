#include <IFSelect_SelectRoots.ixx>
#include <IFGraph_Cumulate.hxx>


    IFSelect_SelectRoots::IFSelect_SelectRoots ()    {  }


// Refait pour travailler en une fois

    Interface_EntityIterator  IFSelect_SelectRoots::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator input = InputResult(G);
  Interface_EntityIterator iter;
  IFGraph_Cumulate GC(G);

//  On note dans le graphe : le cumul de chaque ensemble (Entite + Shared tous
//  niveaux). Les Roots initiales comptees une seule fois sont bonnes
  for (input.Start(); input.More(); input.Next()) {
    Handle(Standard_Transient) ent = input.Value();
    GC.GetFromEntity(ent);
  }
//  A present, on retient, parmi les inputs, celles comptees une seule fois
  for (input.Start(); input.More(); input.Next()) {
    Handle(Standard_Transient) ent = input.Value();
    if ((Standard_Boolean)(GC.NbTimes(ent) <= 1) == IsDirect()) iter.GetOneItem(ent);
  }
  return iter;
}

    Standard_Boolean  IFSelect_SelectRoots::HasUniqueResult () const
      {  return Standard_True;  }

    Standard_Boolean  IFSelect_SelectRoots::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Handle(Interface_InterfaceModel)& ) const 
      {  return Standard_True;  }

    TCollection_AsciiString  IFSelect_SelectRoots::ExtractLabel () const 
      {  return TCollection_AsciiString("Local Roots");  }
