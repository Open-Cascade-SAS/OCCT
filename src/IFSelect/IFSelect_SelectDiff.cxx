#include <IFSelect_SelectDiff.ixx>
#include <IFGraph_Compare.hxx>


    IFSelect_SelectDiff::IFSelect_SelectDiff ()    {  }


    Interface_EntityIterator  IFSelect_SelectDiff::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Compare GC(G);
  GC.GetFromIter(MainInput()->RootResult(G),Standard_True);    // first
  if (HasSecondInput())
    GC.GetFromIter(SecondInput()->RootResult(G),Standard_False);   // second
  return GC.FirstOnly();
}

    Standard_Boolean  IFSelect_SelectDiff::HasUniqueResult () const 
      {  return Standard_True;  }    // naturellement unique

    TCollection_AsciiString  IFSelect_SelectDiff::Label () const 
      {  return TCollection_AsciiString("Differences");  }
