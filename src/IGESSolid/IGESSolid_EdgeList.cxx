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

#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_EdgeList,IGESData_IGESEntity)

IGESSolid_EdgeList::IGESSolid_EdgeList ()    {  }


    void  IGESSolid_EdgeList::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& Curves,
   const Handle(IGESSolid_HArray1OfVertexList)& startVertexList,
   const Handle(TColStd_HArray1OfInteger)& startVertexIndex,
   const Handle(IGESSolid_HArray1OfVertexList)& endVertexList,
   const Handle(TColStd_HArray1OfInteger)& endVertexIndex)
{
  Standard_Integer nb = (Curves.IsNull() ? 0 : Curves->Length());

  if (nb == 0 || Curves->Lower() != 1 || 
      startVertexList->Lower()  != 1 || startVertexList->Length()  != nb ||
      startVertexIndex->Lower() != 1 || startVertexIndex->Length() != nb ||
      endVertexList->Lower()    != 1 || endVertexList->Length()    != nb ||
      endVertexIndex->Lower()   != 1 || endVertexIndex->Length()   != nb  )
    throw Standard_DimensionError("IGESSolid_EdgeList : Init");

  theCurves = Curves;
  theStartVertexList  = startVertexList;
  theStartVertexIndex = startVertexIndex;
  theEndVertexList    = endVertexList;
  theEndVertexIndex   = endVertexIndex;

  InitTypeAndForm(504,1);
}

    Standard_Integer  IGESSolid_EdgeList::NbEdges () const
{
  return (theCurves.IsNull() ? 0 : theCurves->Length());
}

    Handle(IGESData_IGESEntity)  IGESSolid_EdgeList::Curve
  (const Standard_Integer num) const
{
  return theCurves->Value(num);
}

    Handle(IGESSolid_VertexList)  IGESSolid_EdgeList::StartVertexList
  (const Standard_Integer num) const
{
  return theStartVertexList->Value(num);
}

    Standard_Integer  IGESSolid_EdgeList::StartVertexIndex
  (const Standard_Integer num) const
{
  return theStartVertexIndex->Value(num);
}

    Handle(IGESSolid_VertexList)  IGESSolid_EdgeList::EndVertexList
  (const Standard_Integer num) const
{
  return theEndVertexList->Value(num);
}

    Standard_Integer  IGESSolid_EdgeList::EndVertexIndex
  (const Standard_Integer num) const
{
  return theEndVertexIndex->Value(num);
}
