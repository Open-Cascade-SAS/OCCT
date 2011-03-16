#include <IFSelect_DispPerOne.ixx>
#include <IFSelect_Selection.hxx>
#include <IFGraph_SCRoots.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_EntityIterator.hxx>


//  Genere un paquet par racine (strong comp.) locale a la liste transmise


    IFSelect_DispPerOne::IFSelect_DispPerOne ()    {  }

    TCollection_AsciiString  IFSelect_DispPerOne::Label () const
{  return TCollection_AsciiString ("One File per Input Entity");  }


    Standard_Boolean  IFSelect_DispPerOne::LimitedMax
  (const Standard_Integer nbent, Standard_Integer& pcount) const 
{  pcount = nbent;  return Standard_True;  }

    Standard_Boolean  IFSelect_DispPerOne::PacketsCount
  (const Interface_Graph& G, Standard_Integer& pcount) const 
{  pcount = G.Size();  return Standard_True;  }

    void  IFSelect_DispPerOne::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
  IFGraph_SCRoots packsc(G,Standard_False);  // OK pour SubPartsIterator
  packsc.SetLoad();
  packsc.GetFromIter(FinalSelection()->UniqueResult(G));
//   SCRoots a initie la resolution : decoupage en StrongComponants + selection
//   des Racines. Chaque Racine correspond a un Packet. CQFD
  packs.GetParts(packsc);
}
