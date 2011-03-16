//--------------------------------------------------------------------
//
//  File Name : IGESSolid_VertexList.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
