// File:      OpenGl_GraphicDriver_5.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

void OpenGl_GraphicDriver::DumpGroup (const Graphic3d_CGroup& ACGroup)
{
  // Do nothing
}

void OpenGl_GraphicDriver::DumpStructure (const Graphic3d_CStructure& ACStructure)
{
  // Do nothing
}

void OpenGl_GraphicDriver::DumpView (const Graphic3d_CView& ACView)
{
  // Do nothing
}

Standard_Boolean OpenGl_GraphicDriver::ElementExploration (const Graphic3d_CStructure & ACStructure, const Standard_Integer ElementNumber, Graphic3d_VertexNC& AVertex, Graphic3d_Vector& AVector)
{
  // Do nothing
  return (Standard_False);
}

Graphic3d_TypeOfPrimitive OpenGl_GraphicDriver::ElementType (const Graphic3d_CStructure & ACStructure, const Standard_Integer ElementNumber)
{
  // Do nothing
  return Graphic3d_TOP_UNDEFINED;
}
