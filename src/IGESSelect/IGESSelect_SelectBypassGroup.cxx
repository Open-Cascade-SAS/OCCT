// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
  (const Standard_Integer /*level*/, const Handle(Standard_Transient)& ent,
   const Interface_Graph& /*G*/,  Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESBasic_Group,gr,ent);    // Group les regroupe tous
  if (gr.IsNull()) return Standard_True;

  Standard_Integer i, nb = gr->NbEntities();
  for (i = 1; i <= nb; i ++)  explored.AddItem (gr->Entity(i));
  return Standard_True;
}


    TCollection_AsciiString IGESSelect_SelectBypassGroup::ExploreLabel () const
      {  return TCollection_AsciiString ("Content of Groups");  }
