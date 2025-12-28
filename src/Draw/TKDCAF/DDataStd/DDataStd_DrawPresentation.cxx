// Created on: 1998-09-23
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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

#include <DDataStd_DrawDriver.hxx>
#include <DDataStd_DrawPresentation.hxx>
#include <Draw.hxx>
#include <Draw_Viewer.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DefaultDeltaOnRemoval.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDataStd_DrawPresentation, TDF_Attribute)

#ifndef _WIN32
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif
//=================================================================================================

bool DDataStd_DrawPresentation::IsDisplayed(const TDF_Label& L)
{
  occ::handle<DDataStd_DrawPresentation> P;
  if (L.FindAttribute(DDataStd_DrawPresentation::GetID(), P))
  {
    return P->IsDisplayed();
  }
  return false;
}

//=================================================================================================

bool DDataStd_DrawPresentation::HasPresentation(const TDF_Label& L)
{
  return (L.IsAttribute(DDataStd_DrawPresentation::GetID()));
}

//=================================================================================================

void DDataStd_DrawPresentation::Display(const TDF_Label& L)
{
  occ::handle<DDataStd_DrawPresentation> P;
  // set
  if (!L.FindAttribute(DDataStd_DrawPresentation::GetID(), P))
  {
    P = new DDataStd_DrawPresentation();
    L.AddAttribute(P);
  }
  // display
  if (P->IsDisplayed())
    return;
  if (P->GetDrawable().IsNull())
    P->DrawBuild();
  DrawDisplay(P->Label(), P);
  P->SetDisplayed(true);
}

//=================================================================================================

void DDataStd_DrawPresentation::Erase(const TDF_Label& L)
{
  occ::handle<DDataStd_DrawPresentation> P;
  if (L.FindAttribute(DDataStd_DrawPresentation::GetID(), P))
  {
    if (P->IsDisplayed())
    {
      DrawErase(P->Label(), P);
      P->SetDisplayed(false);
    }
  }
}

//=================================================================================================

void DDataStd_DrawPresentation::Update(const TDF_Label& L)
{
  occ::handle<DDataStd_DrawPresentation> P;
  if (L.FindAttribute(DDataStd_DrawPresentation::GetID(), P))
  {
    DrawErase(P->Label(), P);
    P->Backup();
    P->DrawBuild();
    DrawDisplay(P->Label(), P);
    P->SetDisplayed(true);
  }
}

//=======================================================================
// function : GetID
// purpose  : idem GUID DDataStd Display
//=======================================================================

const Standard_GUID& DDataStd_DrawPresentation::GetID()
{
  static Standard_GUID DDataStd_DrawPresentationID("1c0296d4-6dbc-22d4-b9c8-0070b0ee301b");
  return DDataStd_DrawPresentationID;
}

//=================================================================================================

DDataStd_DrawPresentation::DDataStd_DrawPresentation()
    : isDisplayed(false)
{
}

//=================================================================================================

void DDataStd_DrawPresentation::SetDisplayed(const bool status)
{
  isDisplayed = status;
}

//=================================================================================================

bool DDataStd_DrawPresentation::IsDisplayed() const
{
  return isDisplayed;
}

//=================================================================================================

void DDataStd_DrawPresentation::SetDrawable(const occ::handle<Draw_Drawable3D>& D)
{
  myDrawable = D;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DDataStd_DrawPresentation::GetDrawable() const
{
  return myDrawable;
}

//=================================================================================================

const Standard_GUID& DDataStd_DrawPresentation::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> DDataStd_DrawPresentation::NewEmpty() const
{
  return new DDataStd_DrawPresentation();
}

//=================================================================================================

void DDataStd_DrawPresentation::Restore(const occ::handle<TDF_Attribute>& With)
{
  // std::cout<< "DDataStd_DrawPresentation::Restore" << std::endl;
  isDisplayed = occ::down_cast<DDataStd_DrawPresentation>(With)->IsDisplayed();
  myDrawable  = occ::down_cast<DDataStd_DrawPresentation>(With)->GetDrawable();
}

//=================================================================================================

void DDataStd_DrawPresentation::Paste(const occ::handle<TDF_Attribute>& Into,
                                      const occ::handle<TDF_RelocationTable>&) const
{
  occ::down_cast<DDataStd_DrawPresentation>(Into)->SetDisplayed(isDisplayed);
  occ::down_cast<DDataStd_DrawPresentation>(Into)->SetDrawable(myDrawable);
}

//=================================================================================================

void DDataStd_DrawPresentation::AfterAddition()
{
  // std::cout<< "DDataStd_DrawPresentation::AfterAddition" << std::endl;
  // if (isDisplayed)  DrawDisplay ();
}

//=================================================================================================

void DDataStd_DrawPresentation::BeforeRemoval()
{
  // std::cout<< "DDataStd_DrawPresentation::BeforeRemoval" << std::endl;
  // if (isDisplayed)  DrawErase ();
}

//=================================================================================================

void DDataStd_DrawPresentation::BeforeForget()
{
  // std::cout<< "DDataStd_DrawPresentation::BeforeForget" << std::endl;
  if (isDisplayed)
    DrawErase(Label(), this);
}

//=================================================================================================

void DDataStd_DrawPresentation::AfterResume()
{
  // std::cout<< "DDataStd_DrawPresentation::AfterResume"<< std::endl;
  if (isDisplayed)
    DrawDisplay(Label(), this);
}

//=======================================================================
// function : BeforeUndo
// purpose  : the associated NamedShape should be present
//=======================================================================

bool DDataStd_DrawPresentation::BeforeUndo(const occ::handle<TDF_AttributeDelta>& AD,
                                           const bool /*forceIt*/)
{
  occ::handle<DDataStd_DrawPresentation> Pme = this;
  occ::handle<DDataStd_DrawPresentation> Pdt =
    occ::down_cast<DDataStd_DrawPresentation>(AD->Attribute());
  occ::handle<DDataStd_DrawPresentation> Pfw;
  if (AD->Label().FindAttribute(GetID(), Pfw))
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : attribute in framework" << std::endl;
  }
  else
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : attribute not in framework" <<
    // std::endl;
  }
  //
  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : delta on addition" << std::endl;
    if (Pfw->IsDisplayed())
      DrawErase(AD->Label(), Pfw);
    // TDF appelle BeforeRemoval effacer
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval)))
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : delta on removal" << std::endl;
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : delta on modification" << std::endl;
    if (Pfw->IsDisplayed())
      DrawErase(AD->Label(), Pfw);
  }
  else
  {
    // std::cout<< "DDataStd_DrawPresentation::BeforeUndo : unknown delta" << std::endl;
  }
  // if (Label().IsNull()) std::cout<< "DDataStd_DrawPresentation::BeforeUndo : null label" <<
  // std::endl;
  return true;
}

//=======================================================================
// function : AfterUndo
// purpose  : associated NamedShape should be present
//=======================================================================

bool DDataStd_DrawPresentation::AfterUndo(const occ::handle<TDF_AttributeDelta>& AD,
                                          const bool /*forceIt*/)
{
  occ::handle<DDataStd_DrawPresentation> Pme = this;
  occ::handle<DDataStd_DrawPresentation> Pdt =
    occ::down_cast<DDataStd_DrawPresentation>(AD->Attribute());
  occ::handle<DDataStd_DrawPresentation> Pfw;
  if (AD->Label().FindAttribute(GetID(), Pfw))
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : attribute in framework" << std::endl;
  }
  else
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : attribute not in framework" << std::endl;
  }

  //
  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : delta on addition" << std::endl;
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval)))
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : delta on removal" << std::endl;
    if (Pdt->IsDisplayed())
      DrawDisplay(AD->Label(), Pdt);
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : delta on modification" << std::endl;
    if (Pdt->IsDisplayed())
      DrawDisplay(AD->Label(), Pdt);
  }
  else
  {
    // std::cout<< "DDataStd_DrawPresentation::AfterUndo : unknown delta" << std::endl;
  }
  // if (Label().IsNull()) std::cout<< "DDataStd_DrawPresentation::AfterUndo : null label" <<
  // std::endl;
  return true;
}

//=================================================================================================

void DDataStd_DrawPresentation::DrawBuild()
{
  // std::cout<< "DDataStd_DrawPresentation::DrawBuild" << std::endl;
  occ::handle<DDataStd_DrawDriver> DD = DDataStd_DrawDriver::Get();
  if (DD.IsNull())
  {
    DD = new DDataStd_DrawDriver();
    DDataStd_DrawDriver::Set(DD);
  }
  occ::handle<Draw_Drawable3D> D3D = DD->Drawable(Label());
  if (D3D.IsNull())
    std::cout << "DDataStd_DrawPresentation::DrawBuild : null drawable" << std::endl;
  myDrawable = D3D;
}

//=================================================================================================

void DDataStd_DrawPresentation::DrawDisplay(const TDF_Label&                              L,
                                            const occ::handle<DDataStd_DrawPresentation>& P)
{
  if (!L.IsNull())
  {
    if (!P->GetDrawable().IsNull())
    {
      TCollection_AsciiString S;
      TDF_Tool::Entry(L, S);
      const char* name = S.ToCString();
      Draw::Set(name, P->GetDrawable());
      return;
    }
    else
    {
      // std::cout<< "DDataStd_DrawPresentation::DrawDisplay : null Drawable" << std::endl;
      return;
    }
  }
  std::cout << "DDataStd_DrawPresentation::DrawDisplay : null Label" << std::endl;
}

//=================================================================================================

void DDataStd_DrawPresentation::DrawErase(const TDF_Label&                              L,
                                          const occ::handle<DDataStd_DrawPresentation>& P)
{
  if (!L.IsNull())
  {
    if (!P->GetDrawable().IsNull())
    {
      dout.RemoveDrawable(P->GetDrawable());
      return;
    }
    else
    {
      // std::cout<< "DDataStd_DrawPresentation::DrawErase : null Drawable" << std::endl;
      return;
    }
  }
  std::cout << "DDataStd_DrawPresentation::DrawErase : null Label" << std::endl;
}
