// File:      OpenGl_GraphicDriver_707.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_Polyline.hxx>

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertex& AListVertex,
                                    const Standard_Boolean )
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Polyline *apolyline = new OpenGl_Polyline( AListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolyline, apolyline );
  }
}

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertexC& AListVertex,
                                    const Standard_Boolean )
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Polyline *apolyline = new OpenGl_Polyline( AListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolyline, apolyline );
  }
}

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                    const Standard_Real X1,
                                    const Standard_Real Y1,
                                    const Standard_Real Z1,
                                    const Standard_Real X2,
                                    const Standard_Real Y2,
                                    const Standard_Real Z2,
                                    const Standard_Boolean )
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_Polyline *apolyline = new OpenGl_Polyline( X1, Y1, Z1, X2, Y2, Z2 );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelPolyline, apolyline );
  }
}
