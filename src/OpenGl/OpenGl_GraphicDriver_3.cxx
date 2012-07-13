// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GraphicDriver.hxx>

#include <TColStd_HArray1OfByte.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Structure.hxx>

void OpenGl_GraphicDriver::ClearGroup (const Graphic3d_CGroup& theCGroup)
{
  if (theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->Release (GetSharedContext());
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

  aStructure->RemoveGroup (GetSharedContext(), (const OpenGl_Group* )theCGroup.ptrGroup);
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
