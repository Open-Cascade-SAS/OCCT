#include <IFSelect_DispPerCount.ixx>
#include <IFSelect_Selection.hxx>
#include <IFGraph_SCRoots.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_EntityIterator.hxx>


    IFSelect_DispPerCount::IFSelect_DispPerCount ()    {  }

    Handle(IFSelect_IntParam)  IFSelect_DispPerCount::Count () const 
      {  return thecount;  }

    void  IFSelect_DispPerCount::SetCount
  (const Handle(IFSelect_IntParam)& pcount)
      {  thecount = pcount;  }

    Standard_Integer  IFSelect_DispPerCount::CountValue () const
{
  Standard_Integer pcount = 0;
  if (!thecount.IsNull()) pcount = thecount->Value();
  if (pcount <= 0) pcount = 1;    // option prise par defaut
  return pcount;
}

    TCollection_AsciiString  IFSelect_DispPerCount::Label () const
{
  TCollection_AsciiString lab(CountValue());
  lab.Insert(1,"One File per ");
  lab.AssignCat(" Entities");
  return lab;
}


    Standard_Boolean  IFSelect_DispPerCount::LimitedMax
  (const Standard_Integer nbent, Standard_Integer& pcount) const 
{
  pcount = 1 + nbent / CountValue();
  return Standard_True;
}

    Standard_Boolean  IFSelect_DispPerCount::PacketsCount
  (const Interface_Graph& G, Standard_Integer& /*count*/) const 
      {  return 1 + G.Size() / CountValue();  }

    void  IFSelect_DispPerCount::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
//  Ressemble a DispPerOne, mais fait un AddPart tous les "count" racines
  Standard_Integer pcount = CountValue();

  IFGraph_SCRoots roots(G,Standard_False);
  roots.SetLoad();
  roots.GetFromIter(FinalSelection()->UniqueResult(G));
//   SCRoots a initie la resolution : decoupage en StrongComponants + selection
//   des racines. Un paquet correspond des lors a <count> racines
//   Donc, il faut iterer sur les Parts de roots et les prendre par <count>

  Standard_Integer i = 0;
  for (roots.Start(); roots.More(); roots.Next()) {
    if (i == 0) packs.AddPart();
    i ++;  if (i >= pcount) i = 0;  // regroupement selon "count"
    packs.GetFromIter(roots.Entities());
  }
}
