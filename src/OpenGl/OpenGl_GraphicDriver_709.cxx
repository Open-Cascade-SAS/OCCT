// File:      OpenGl_GraphicDriver_709.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_QuadrangleStrip.hxx>

void OpenGl_GraphicDriver::QuadrangleMesh (const Graphic3d_CGroup& ACGroup,
                                          const Graphic3d_Array2OfVertex& ListVertex,
                                          const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_QuadrangleStrip *aqstrip = new OpenGl_QuadrangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelQuadrangle, aqstrip );
  }
}

void OpenGl_GraphicDriver::QuadrangleMesh (const Graphic3d_CGroup& ACGroup,
                                          const Graphic3d_Array2OfVertexN& ListVertex,
                                          const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_QuadrangleStrip *aqstrip = new OpenGl_QuadrangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelQuadrangle, aqstrip );
  }
}

void OpenGl_GraphicDriver::QuadrangleMesh(const Graphic3d_CGroup& ACGroup,
                                         const Graphic3d_Array2OfVertexNT& ListVertex,
                                         const Standard_Boolean ) 
{
  if (ACGroup.ptrGroup)
  {
    OpenGl_QuadrangleStrip *aqstrip = new OpenGl_QuadrangleStrip( ListVertex );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelQuadrangle, aqstrip );
  }
}
