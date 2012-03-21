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

#include  <IGESSelect_SelectSingleViewFrom.ixx>
#include <IGESSelect_ViewSorter.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Macros.hxx>



IGESSelect_SelectSingleViewFrom::IGESSelect_SelectSingleViewFrom ()    {  }

    Standard_Boolean  IGESSelect_SelectSingleViewFrom::HasUniqueResult () const
      {  return Standard_True;  }

    Interface_EntityIterator  IGESSelect_SelectSingleViewFrom::RootResult
  (const Interface_Graph& G) const
{
  Handle(IGESSelect_ViewSorter) sorter = new IGESSelect_ViewSorter;
  sorter->SetModel (GetCasted(IGESData_IGESModel,G.Model()));
  sorter->Clear();
  sorter->AddList (InputResult(G).Content());
  sorter->SortSingleViews(Standard_True);
  Interface_EntityIterator list;
  Standard_Integer nb = sorter->NbSets(Standard_True);
  for (Standard_Integer i = 1; i <= nb; i ++)
    list.GetOneItem(sorter->SetItem(i,Standard_True));
  return list;
}


    TCollection_AsciiString  IGESSelect_SelectSingleViewFrom::Label () const
      {  return TCollection_AsciiString ("Single Views attached");  }
