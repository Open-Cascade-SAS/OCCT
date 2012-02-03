// File:      OpenGl_GraphicDriver_705.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_Polygon.hxx>

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ACGroup,
                                   const Graphic3d_Array1OfVertex& AListVertex,
                                   const Graphic3d_TypeOfPolygon AType,
                                   const Standard_Boolean )
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Polygon *apolygon = new OpenGl_Polygon( AListVertex, AType );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolygon, apolygon );
  }
}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ,
                                   const Graphic3d_Array1OfVertex& ,
                                   const Graphic3d_Vector& ,
                                   const Graphic3d_TypeOfPolygon ,
                                   const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ,
                                   const Graphic3d_Array1OfVertexN& ,
                                   const Graphic3d_TypeOfPolygon ,
                                   const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ,
                                   const Graphic3d_Array1OfVertexN& ,
                                   const Graphic3d_Vector& ,
                                   const Graphic3d_TypeOfPolygon ,
                                   const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ,
                                   const Graphic3d_Array1OfVertexNT& ,
                                   const Graphic3d_TypeOfPolygon ,
                                   const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ,
                                        const TColStd_Array1OfInteger& ,
                                        const Graphic3d_Array1OfVertex& ,
                                        const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ,
                                        const TColStd_Array1OfInteger& ,
                                        const Graphic3d_Array1OfVertex& ,
                                        const Graphic3d_Vector& ,
                                        const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ,
                                        const TColStd_Array1OfInteger& ,
                                        const Graphic3d_Array1OfVertexN& ,
                                        const Standard_Boolean )
{
  // Do nothing
}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ,
                                        const TColStd_Array1OfInteger& ,
                                        const Graphic3d_Array1OfVertexN& ,
                                        const Graphic3d_Vector& ,
                                        const Standard_Boolean )
{
  // Do nothing
}
