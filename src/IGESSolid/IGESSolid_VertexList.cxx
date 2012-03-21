// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_VertexList.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESSolid_VertexList::IGESSolid_VertexList ()    {  }


    void  IGESSolid_VertexList::Init
  (const Handle(TColgp_HArray1OfXYZ)& Vertices)
{
  if (Vertices.IsNull() || Vertices->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESSolid_VertexList : Init");
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
