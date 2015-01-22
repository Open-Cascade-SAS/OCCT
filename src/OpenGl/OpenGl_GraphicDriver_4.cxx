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

#include <NCollection_DataMap.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>

void OpenGl_GraphicDriver::DisplayStructure (const Graphic3d_CView&             theCView,
                                             const Handle(Graphic3d_Structure)& theStructure,
                                             const Standard_Integer             thePriority)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView == NULL)
    return;

  aCView->View->DisplayStructure (theStructure, thePriority);
}

void OpenGl_GraphicDriver::EraseStructure (const Graphic3d_CView&             theCView,
                                           const Handle(Graphic3d_Structure)& theStructure)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView == NULL)
    return;

  aCView->View->EraseStructure (theStructure);
}

void OpenGl_GraphicDriver::RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure)
{
  OpenGl_Structure* aStructure = NULL;
  if (!myMapOfStructure.Find (theCStructure->Id, aStructure))
  {
    return;
  }

  myMapOfStructure.UnBind (theCStructure->Id);
  aStructure->Release (GetSharedContext());
  theCStructure.Nullify();
}

Handle(Graphic3d_CStructure) OpenGl_GraphicDriver::Structure (const Handle(Graphic3d_StructureManager)& theManager)
{
  Handle(OpenGl_Structure) aStructure = new OpenGl_Structure (theManager);
  myMapOfStructure.Bind (aStructure->Id, aStructure.operator->());
  return aStructure;
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::ChangeZLayer (const Graphic3d_CStructure& theCStructure,
                                         const Graphic3d_CView&      theCView,
                                         const Graphic3d_ZLayerId    theNewLayerId)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;

  if (!myMapOfStructure.IsBound (theCStructure.Id) || !aCView)
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aCView->View->ChangeZLayer (aStructure, theNewLayerId);
}

//=======================================================================
//function : UnsetZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::UnsetZLayer (const Graphic3d_ZLayerId theLayerId)
{
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
  for( ; aStructIt.More (); aStructIt.Next ())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue ();
    if (aStruct->ZLayer() == theLayerId)
      aStruct->SetZLayer (Graphic3d_ZLayerId_Default);
  }
}

//=======================================================================
//function : ChangePriority
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::ChangePriority (const Graphic3d_CStructure& theCStructure,
                                           const Graphic3d_CView&      theCView,
                                           const Standard_Integer      theNewPriority)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;

  if (!myMapOfStructure.IsBound (theCStructure.Id) || !aCView)
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aCView->View->ChangePriority (aStructure, theNewPriority);
}
