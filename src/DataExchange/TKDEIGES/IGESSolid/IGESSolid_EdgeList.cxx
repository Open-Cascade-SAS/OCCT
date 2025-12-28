// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_EdgeList, IGESData_IGESEntity)

IGESSolid_EdgeList::IGESSolid_EdgeList() {}

void IGESSolid_EdgeList::Init(const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&  Curves,
                              const occ::handle<NCollection_HArray1<occ::handle<IGESSolid_VertexList>>>& startVertexList,
                              const occ::handle<NCollection_HArray1<int>>&      startVertexIndex,
                              const occ::handle<NCollection_HArray1<occ::handle<IGESSolid_VertexList>>>& endVertexList,
                              const occ::handle<NCollection_HArray1<int>>&      endVertexIndex)
{
  int nb = (Curves.IsNull() ? 0 : Curves->Length());

  if (nb == 0 || Curves->Lower() != 1 || startVertexList->Lower() != 1
      || startVertexList->Length() != nb || startVertexIndex->Lower() != 1
      || startVertexIndex->Length() != nb || endVertexList->Lower() != 1
      || endVertexList->Length() != nb || endVertexIndex->Lower() != 1
      || endVertexIndex->Length() != nb)
    throw Standard_DimensionError("IGESSolid_EdgeList : Init");

  theCurves           = Curves;
  theStartVertexList  = startVertexList;
  theStartVertexIndex = startVertexIndex;
  theEndVertexList    = endVertexList;
  theEndVertexIndex   = endVertexIndex;

  InitTypeAndForm(504, 1);
}

int IGESSolid_EdgeList::NbEdges() const
{
  return (theCurves.IsNull() ? 0 : theCurves->Length());
}

occ::handle<IGESData_IGESEntity> IGESSolid_EdgeList::Curve(const int num) const
{
  return theCurves->Value(num);
}

occ::handle<IGESSolid_VertexList> IGESSolid_EdgeList::StartVertexList(const int num) const
{
  return theStartVertexList->Value(num);
}

int IGESSolid_EdgeList::StartVertexIndex(const int num) const
{
  return theStartVertexIndex->Value(num);
}

occ::handle<IGESSolid_VertexList> IGESSolid_EdgeList::EndVertexList(const int num) const
{
  return theEndVertexList->Value(num);
}

int IGESSolid_EdgeList::EndVertexIndex(const int num) const
{
  return theEndVertexIndex->Value(num);
}
