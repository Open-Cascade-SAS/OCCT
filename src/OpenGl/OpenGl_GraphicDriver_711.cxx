// File:      OpenGl_GraphicDriver_711.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_TriangleStrip.hxx>
#include <OpenGl_Mesh.hxx>

void OpenGl_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertex& ListVertex,
                                        const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_TriangleStrip *atstrip = new OpenGl_TriangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelTriangleMesh, atstrip );
  }
}

void OpenGl_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertexN& ListVertex,
                                        const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_TriangleStrip *atstrip = new OpenGl_TriangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelTriangleMesh, atstrip );
  }
}

void OpenGl_GraphicDriver::TriangleMesh(const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertexNT& ListVertex,
                                       const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_TriangleStrip *atstrip = new OpenGl_TriangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelTriangleMesh, atstrip );
  }
}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertex& ListVertex,
                                       const Aspect_Array1OfEdge& ListEdge,
                                       const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Mesh *amesh = new OpenGl_Mesh (ListVertex, ListEdge);
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygonIndices, amesh );
  }
}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertexN& ListVertex,
                                       const Aspect_Array1OfEdge& ListEdge,
                                       const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Mesh *amesh = new OpenGl_Mesh (ListVertex, ListEdge);
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygonIndices, amesh );
  }
}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertexC& ListVertex,
                                       const Aspect_Array1OfEdge& ListEdge,
                                       const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Mesh *amesh = new OpenGl_Mesh (ListVertex, ListEdge);
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygonIndices, amesh );
  }
}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertexNC& ListVertex,
                                       const Aspect_Array1OfEdge& ListEdge,
                                       const Standard_Boolean )
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Mesh *amesh = new OpenGl_Mesh (ListVertex, ListEdge);
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygonIndices, amesh );
  }
}

void OpenGl_GraphicDriver::TriangleSet(const Graphic3d_CGroup& ACGroup,
                                      const Graphic3d_Array1OfVertexNT& ListVertex,
                                      const Aspect_Array1OfEdge& ListEdge,
                                      const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Mesh *amesh = new OpenGl_Mesh (ListVertex, ListEdge);
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygonIndices, amesh );
  }
}
