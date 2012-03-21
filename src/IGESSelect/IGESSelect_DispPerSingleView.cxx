// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
