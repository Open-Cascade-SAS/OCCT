#include <IFSelect_SelectRootComps.ixx>
#include <IFGraph_StrongComponants.hxx>
#include <IFGraph_Cumulate.hxx>


    IFSelect_SelectRootComps::IFSelect_SelectRootComps ()    {  }


// Refait pour travailler en une fois
// ATTENTION, il ne faut pas s interesser aux ENTITES mais aux COMPOSANTS
// c-a-d gerer les CYCLES s il y en a

    Interface_EntityIterator  IFSelect_SelectRootComps::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator IEIinput = InputResult(G);
  Interface_EntityIterator iter;
//  ICI, extraire les Componants, puis considerer une Entite de chacun
  IFGraph_StrongComponants comps(G,Standard_False);
  comps.SetLoad();
  comps.GetFromIter(IEIinput);
  Interface_EntityIterator inp1;  // IEIinput reduit a une Entite par Composant

  IFGraph_Cumulate GC(G);

//  On note dans le graphe : le cumul de chaque ensemble (Entite + Shared tous
//  niveaux). Les Roots initiales comptees une seule fois sont bonnes
//  Pour Entite : une par Componant (peu importe)
  for (comps.Start(); comps.More(); comps.Next()) {
    Handle(Standard_Transient) ent = comps.FirstEntity();
    GC.GetFromEntity(ent);
    inp1.GetOneItem(ent);
  }
//  A present, on retient, parmi les inputs, celles comptees une seule fois
//  (N.B.: on prend inp1, qui donne UNE entite par composant, simple ou cycle)
  for (inp1.Start(); inp1.More(); inp1.Next()) {
    Handle(Standard_Transient) ent = inp1.Value();
    if ((Standard_Boolean)(GC.NbTimes(ent) <= 1) == IsDirect()) iter.GetOneItem(ent);
  }
  return iter;
}

    Standard_Boolean  IFSelect_SelectRootComps::HasUniqueResult () const
      {  return Standard_True;  }


    Standard_Boolean  IFSelect_SelectRootComps::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Handle(Interface_InterfaceModel)& ) const 
      {  return Standard_True;  }

    TCollection_AsciiString  IFSelect_SelectRootComps::ExtractLabel () const 
{  return TCollection_AsciiString("Local Root Componants");  }
