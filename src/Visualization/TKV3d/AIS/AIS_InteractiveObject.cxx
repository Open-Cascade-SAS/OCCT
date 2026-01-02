// Created on: 1996-12-18
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_InteractiveObject.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <PrsMgr_PresentationManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_InteractiveObject, SelectMgr_SelectableObject)

//=================================================================================================

AIS_InteractiveObject::AIS_InteractiveObject(
  const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
    : SelectMgr_SelectableObject(aTypeOfPresentation3d),
      myCTXPtr(nullptr)
{}

//=================================================================================================

void AIS_InteractiveObject::Redisplay(const bool AllModes)
{
  if (myCTXPtr == nullptr)
    return;

  myCTXPtr->Redisplay(this, false, AllModes);
}

//=================================================================================================

bool AIS_InteractiveObject::ProcessDragging(const occ::handle<AIS_InteractiveContext>&,
                                            const occ::handle<V3d_View>&,
                                            const occ::handle<SelectMgr_EntityOwner>&,
                                            const NCollection_Vec2<int>&,
                                            const NCollection_Vec2<int>&,
                                            const AIS_DragAction)
{
  return false;
}

//=================================================================================================

occ::handle<AIS_InteractiveContext> AIS_InteractiveObject::GetContext() const
{
  return myCTXPtr;
}

//=================================================================================================

void AIS_InteractiveObject::SetContext(const occ::handle<AIS_InteractiveContext>& theCtx)
{
  if (myCTXPtr == theCtx.get())
  {
    return;
  }

  myCTXPtr = theCtx.get();
  if (!theCtx.IsNull())
  {
    myDrawer->Link(theCtx->DefaultDrawer());
  }
}

//=================================================================================================

void AIS_InteractiveObject::SetDisplayStatus(PrsMgr_DisplayStatus theStatus)
{
  myDisplayStatus = theStatus;
}

//=================================================================================================

bool AIS_InteractiveObject::HasPresentation() const
{
  return HasInteractiveContext()
         && myCTXPtr->MainPrsMgr()->HasPresentation(this, myDrawer->DisplayMode());
}

//=================================================================================================

occ::handle<Prs3d_Presentation> AIS_InteractiveObject::Presentation() const
{
  if (!HasInteractiveContext())
  {
    return occ::handle<Prs3d_Presentation>();
  }

  occ::handle<PrsMgr_Presentation> aPrs =
    myCTXPtr->MainPrsMgr()->Presentation(this, myDrawer->DisplayMode(), false);
  return aPrs;
}

//=================================================================================================

void AIS_InteractiveObject::SetAspect(const occ::handle<Prs3d_BasicAspect>& theAspect)
{

  if (!HasPresentation())
  {
    return;
  }

  occ::handle<Prs3d_Presentation> aPrs = Presentation();
  if (aPrs->Groups().IsEmpty())
  {
    return;
  }
  const occ::handle<Graphic3d_Group>& aGroup = aPrs->Groups().Last();
  if (occ::handle<Prs3d_ShadingAspect> aShadingAspect =
        occ::down_cast<Prs3d_ShadingAspect>(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect(aShadingAspect->Aspect());
  }
  else if (occ::handle<Prs3d_LineAspect> aLineAspect = occ::down_cast<Prs3d_LineAspect>(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect(aLineAspect->Aspect());
  }
  else if (occ::handle<Prs3d_PointAspect> aPointAspect =
             occ::down_cast<Prs3d_PointAspect>(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect(aPointAspect->Aspect());
  }
  else if (occ::handle<Prs3d_TextAspect> aTextAspect = occ::down_cast<Prs3d_TextAspect>(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect(aTextAspect->Aspect());
  }
}

//=================================================================================================

void AIS_InteractiveObject::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, SelectMgr_SelectableObject)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myCTXPtr)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myOwner)
}
