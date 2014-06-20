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

void OpenGl_GraphicDriver::DisplayStructure (const Graphic3d_CView& theCView,
                                             Graphic3d_CStructure&  theCStructure,
                                             const Standard_Integer thePriority)
{
  const OpenGl_CView* aCView     = (const OpenGl_CView* )theCView.ptrView;
  OpenGl_Structure*   aStructure = (OpenGl_Structure* )&theCStructure;
  if (aCView == NULL)
    return;

  aCView->View->DisplayStructure (aStructure, thePriority);
}

void OpenGl_GraphicDriver::EraseStructure (const Graphic3d_CView& theCView,
                                           Graphic3d_CStructure&  theCStructure)
{
  const OpenGl_CView* aCView     = (const OpenGl_CView* )theCView.ptrView;
  OpenGl_Structure*   aStructure = (OpenGl_Structure* )&theCStructure;
  if (aCView == NULL || aStructure == NULL)
    return;

  aCView->View->EraseStructure (aStructure);
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
                                         const Standard_Integer theLayer)
{
  if (!myMapOfStructure.IsBound (theCStructure.Id))
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aStructure->SetZLayer (theLayer);
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::ChangeZLayer (const Graphic3d_CStructure& theCStructure,
                                         const Graphic3d_CView& theCView,
                                         const Standard_Integer theNewLayerId)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;

  if (!myMapOfStructure.IsBound (theCStructure.Id) || !aCView)
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aCView->View->ChangeZLayer (aStructure, theNewLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================

Standard_Integer OpenGl_GraphicDriver::GetZLayer (const Graphic3d_CStructure& theCStructure) const
{
  if (!myMapOfStructure.IsBound (theCStructure.Id))
    return -1;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  return aStructure->GetZLayer();
}

//=======================================================================
//function : UnsetZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::UnsetZLayer (const Standard_Integer theLayerId)
{
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
  for( ; aStructIt.More (); aStructIt.Next ())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue ();
    if (aStruct->GetZLayer () == theLayerId)
      aStruct->SetZLayer (0);
  }
}

//=======================================================================
//function : ChangePriority
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::ChangePriority (const Graphic3d_CStructure& theCStructure,
                                           const Graphic3d_CView& theCView,
                                           const Standard_Integer theNewPriority)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;

  if (!myMapOfStructure.IsBound (theCStructure.Id) || !aCView)
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aCView->View->ChangePriority (aStructure, theNewPriority);
}
