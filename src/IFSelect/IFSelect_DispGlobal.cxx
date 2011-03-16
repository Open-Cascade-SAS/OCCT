#include <IFSelect_DispGlobal.ixx>
#include <IFSelect_Selection.hxx>
#include <Interface_EntityIterator.hxx>

// Genere un seul paquet avec la sortie finale


    IFSelect_DispGlobal::IFSelect_DispGlobal ()    {  }

    TCollection_AsciiString  IFSelect_DispGlobal::Label () const
{  return TCollection_AsciiString ("One File for All Input");  }

    Standard_Boolean  IFSelect_DispGlobal::LimitedMax
  (const Standard_Integer /* nbent */, Standard_Integer& pcount) const 
      {  pcount = 1;  return Standard_True;  }

    Standard_Boolean  IFSelect_DispGlobal::PacketsCount
  (const Interface_Graph& /* G */, Standard_Integer& pcount) const 
      {  pcount = 1;  return Standard_True;  }

// 1 packet ( a partir de UniqueResult)
      void IFSelect_DispGlobal::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
  packs.AddPart();
  packs.GetFromIter(FinalSelection()->UniqueResult(G));
}
