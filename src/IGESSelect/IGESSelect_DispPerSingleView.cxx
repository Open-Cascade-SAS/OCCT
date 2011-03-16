#include <IGESSelect_DispPerSingleView.ixx>
#include <IFSelect_Selection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IFSelect_PacketList.hxx>
#include <Interface_Macros.hxx>


    IGESSelect_DispPerSingleView::IGESSelect_DispPerSingleView ()
      {  thesorter = new IGESSelect_ViewSorter;  }

    TCollection_AsciiString  IGESSelect_DispPerSingleView::Label () const
{
  return TCollection_AsciiString("One File per single View or Drawing Frame");
}


    void  IGESSelect_DispPerSingleView::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const
{
  if (FinalSelection().IsNull()) return;
  Interface_EntityIterator list = FinalSelection()->UniqueResult(G);
  thesorter->SetModel (GetCasted(IGESData_IGESModel,G.Model()));
  thesorter->Clear();
  thesorter->AddList (list.Content());
  thesorter->SortSingleViews(Standard_True);
  Handle(IFSelect_PacketList) sets = thesorter->Sets(Standard_True);

  packs.SetLoad();
  Standard_Integer nb = sets->NbPackets();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    packs.AddPart();
    packs.GetFromIter (sets->Entities(i));
  }
}


    Standard_Boolean  IGESSelect_DispPerSingleView::CanHaveRemainder () const
      {  return Standard_True;  }

    Interface_EntityIterator  IGESSelect_DispPerSingleView::Remainder
  (const Interface_Graph& G) const
{
  if (thesorter->NbEntities() == 0) {
    Interface_EntityIterator list;
    if (FinalSelection().IsNull()) return list;
    list = FinalSelection()->UniqueResult(G);
    thesorter->Clear();
    thesorter->Add (list.Content());
    thesorter->SortSingleViews(Standard_True);
  }
  return thesorter->Sets(Standard_True)->Duplicated (0,Standard_False);
}
