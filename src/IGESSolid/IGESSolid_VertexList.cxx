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

#include <gp_Pnt.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_VertexList,IGESData_IGESEntity)

IGESSolid_VertexList::IGESSolid_VertexList ()    {  }


    void  IGESSolid_VertexList::Init
  (const Handle(TColgp_HArray1OfXYZ)& Vertices)
{
  if (Vertices.IsNull() || Vertices->Lower() != 1)
    throw Standard_DimensionMismatch("IGESSolid_VertexList : Init");
  theVertices = Vertices;
  InitTypeAndForm(502,1);
}

    Standard_Integer  IGESSolid_VertexList::NbVertices () const
{
  return (theVertices.IsNull() ? 0 : theVertices->Length());
}

    gp_Pnt  IGESSolid_VertexList::Vertex (const Standard_Integer Index) const
{
  return gp_Pnt(theVertices->Value(Index));
}
