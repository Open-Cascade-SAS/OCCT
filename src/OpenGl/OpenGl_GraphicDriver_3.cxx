// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_GraphicDriver.hxx>

#include <TColStd_HArray1OfByte.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Structure.hxx>

void OpenGl_GraphicDriver::ClearGroup (const Graphic3d_CGroup& theCGroup)
{
  if (theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->Release (GetSharedContext());
}

void OpenGl_GraphicDriver::FaceContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextFillArea.IsDef || theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectFace (theCGroup.ContextFillArea, theNoInsert);
}

void OpenGl_GraphicDriver::Group (Graphic3d_CGroup& theCGroup)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCGroup.Struct->ptrStructure;
  if (aStructure)
  {
    theCGroup.ptrGroup = aStructure->AddGroup();
  }
}

void OpenGl_GraphicDriver::LineContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextLine.IsDef || theCGroup.ptrGroup == NULL) return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectLine (theCGroup.ContextLine, theNoInsert);
}

void OpenGl_GraphicDriver::MarkerContextGroup (const Graphic3d_CGroup& theCGroup,
                                               const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextMarker.IsDef || theCGroup.ptrGroup == NULL) return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectMarker (theCGroup.ContextMarker, theNoInsert);
}

void OpenGl_GraphicDriver::RemoveGroup (const Graphic3d_CGroup& theCGroup)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCGroup.Struct->ptrStructure;
  if (aStructure == NULL)
    return;

  aStructure->RemoveGroup (GetSharedContext(), (const OpenGl_Group* )theCGroup.ptrGroup);
}

void OpenGl_GraphicDriver::TextContextGroup (const Graphic3d_CGroup& theCGroup,
                                             const Standard_Integer  theNoInsert)
{
  if (!theCGroup.ContextText.IsDef || theCGroup.ptrGroup == NULL)
    return;

  ((OpenGl_Group* )theCGroup.ptrGroup)->SetAspectText (theCGroup.ContextText, theNoInsert);
}
