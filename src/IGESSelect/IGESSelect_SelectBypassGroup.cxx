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

#include <IGESSelect_SelectBypassGroup.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <Interface_Macros.hxx>


#define TypePourGroup 402


IGESSelect_SelectBypassGroup::IGESSelect_SelectBypassGroup
  (const Standard_Integer level)
  : IFSelect_SelectExplore (level)    {  }


    Standard_Boolean  IGESSelect_SelectBypassGroup::Explore
  (const Standard_Integer level, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G,  Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESBasic_Group,gr,ent);    // Group les regroupe tous
  if (gr.IsNull()) return Standard_True;

  Standard_Integer i, nb = gr->NbEntities();
  for (i = 1; i <= nb; i ++)  explored.AddItem (gr->Entity(i));
  return Standard_True;
}


    TCollection_AsciiString IGESSelect_SelectBypassGroup::ExploreLabel () const
      {  return TCollection_AsciiString ("Content of Groups");  }
