// File:      OpenGl_GraphicDriver_3.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <TColStd_HArray1OfByte.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Structure.hxx>

void OpenGl_GraphicDriver::ClearGroup (const Graphic3d_CGroup& theCGroup)
{
  if (theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->Clear();
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::CloseGroup (const Graphic3d_CGroup& )
{
  // Do nothing
}

void OpenGl_GraphicDriver::FaceContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextFillArea.IsDef || theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectFace (theCGroup.ContextFillArea, theNoInsert);
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::Group (Graphic3d_CGroup& theCGroup)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCGroup.Struct->ptrStructure;
  if (aStructure)
  {
    theCGroup.ptrGroup = aStructure->AddGroup();
    InvalidateAllWorkspaces();
  }
}

void OpenGl_GraphicDriver::LineContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextLine.IsDef || theCGroup.ptrGroup == NULL) return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectLine (theCGroup.ContextLine, theNoInsert);
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& theCGroup,
                                               const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextMarker.IsDef || theCGroup.ptrGroup == NULL) return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectMarker (theCGroup.ContextMarker, theNoInsert);
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& theCGroup, 
                                               const Standard_Integer  theNoInsert,
                                               const Standard_Integer  theMarkWidth,
                                               const Standard_Integer  theMarkHeight,
                                               const Handle(TColStd_HArray1OfByte)& theTexture)
{
  if(!theCGroup.ContextMarker.IsDef)
    return;

  if (!openglDisplay.IsNull())
    openglDisplay->AddUserMarker ((int )theCGroup.ContextMarker.Scale, theMarkWidth, theMarkHeight, theTexture);

  if (theCGroup.ptrGroup != NULL)
  {
    ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectMarker (theCGroup.ContextMarker, theNoInsert);
    InvalidateAllWorkspaces();
  }
}

void OpenGl_GraphicDriver::OpenGroup (const Graphic3d_CGroup& )
{
  // Do nothing
}

void OpenGl_GraphicDriver::RemoveGroup (const Graphic3d_CGroup& theCGroup)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCGroup.Struct->ptrStructure;
  if (aStructure == NULL)
    return;

  aStructure->RemoveGroup ((const OpenGl_Group* )theCGroup.ptrGroup);
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::TextContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextText.IsDef || theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectText (theCGroup.ContextText, theNoInsert);
  InvalidateAllWorkspaces();
}
