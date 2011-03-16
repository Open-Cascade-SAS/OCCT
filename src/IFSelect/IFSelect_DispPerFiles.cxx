#include <IFSelect_DispPerFiles.ixx>
#include <IFSelect_Selection.hxx>
#include <IFGraph_SCRoots.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_EntityIterator.hxx>


    IFSelect_DispPerFiles::IFSelect_DispPerFiles ()    {  }

    Handle(IFSelect_IntParam)  IFSelect_DispPerFiles::Count () const 
      {  return thecount;  }

    void  IFSelect_DispPerFiles::SetCount
  (const Handle(IFSelect_IntParam)& pcount)
      {  thecount = pcount;  }

    Standard_Integer  IFSelect_DispPerFiles::CountValue () const
{
  Standard_Integer pcount = 0;
  if (!thecount.IsNull()) pcount = thecount->Value();
  if (pcount <= 0) pcount = 1;    // option prise par defaut
  return pcount;
}

    TCollection_AsciiString  IFSelect_DispPerFiles::Label () const
{
  TCollection_AsciiString lab(CountValue());
  lab.Insert(1,"Maximum ");
  lab.AssignCat(" Files");
  return lab;
}


    Standard_Boolean  IFSelect_DispPerFiles::LimitedMax
  (const Standard_Integer /* nbent */, Standard_Integer& pcount) const 
{
  pcount = CountValue();
  return Standard_True;
}

    Standard_Boolean  IFSelect_DispPerFiles::PacketsCount
  (const Interface_Graph& G, Standard_Integer& /*count*/) const 
      {  return (G.Size() < CountValue() ? G.Size() : CountValue());  }

    void  IFSelect_DispPerFiles::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
//  Ressemble a DispPerOne, mais fait "count" AddPart racines
  Standard_Integer pcount = CountValue();

  IFGraph_SCRoots roots(G,Standard_False);
  roots.SetLoad();
  roots.GetFromIter(FinalSelection()->UniqueResult(G));
//   SCRoots a initie la resolution : decoupage en StrongComponants + selection
//   des racines. Un paquet correspond des lors a <count> racines
//   Donc, il faut iterer sur les Parts de roots et les prendre par <count>
  roots.Start();                            // Start fait Evaluate specifique
  Standard_Integer nb = roots.NbParts();
  if (pcount > 0) pcount = (nb-1) / pcount +1;    // par packet

  Standard_Integer i = 0;
  for (; roots.More(); roots.Next()) {      // Start deja fait
    if (i == 0) packs.AddPart();
    i ++;  if (i >= pcount) i = 0;  // regroupement selon "count"
    packs.GetFromIter(roots.Entities());
  }
}
