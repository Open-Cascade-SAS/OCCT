// Created on: 1998-09-30
// Created by: Sergey RUIN
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

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NoMoreObject.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DefaultDeltaOnRemoval.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TDataXtd_Presentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_DriverTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_AISPresentation, TDF_Attribute)

#define NO_MORE_OBJECT "TPrsStd_AISPresentation has no associated TDataXtd_PresentationData"

//=================================================================================================

TPrsStd_AISPresentation::TPrsStd_AISPresentation() {}

//=================================================================================================

occ::handle<TPrsStd_AISPresentation> TPrsStd_AISPresentation::Set(const TDF_Label&     theLabel,
                                                                  const Standard_GUID& theDriver)
{
  occ::handle<TPrsStd_AISPresentation> aPresentation;
  // create associated data (unless already there)
  occ::handle<TDataXtd_Presentation> aData = TDataXtd_Presentation::Set(theLabel, theDriver);
  if (aData.IsNull())
    throw Standard_NoMoreObject(NO_MORE_OBJECT);
  if (!theLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), aPresentation))
  {
    aPresentation = new TPrsStd_AISPresentation();
    theLabel.AddAttribute(aPresentation, true);
  }

  return aPresentation;
}

//=================================================================================================

occ::handle<TPrsStd_AISPresentation> TPrsStd_AISPresentation::Set(
  const occ::handle<TDF_Attribute>& theMaster)
{
  return TPrsStd_AISPresentation::Set(theMaster->Label(), theMaster->ID());
}

//=================================================================================================

void TPrsStd_AISPresentation::Unset(const TDF_Label& theLabel)
{
  occ::handle<TPrsStd_AISPresentation> aPresentation;
  if (theLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), aPresentation))
    theLabel.ForgetAttribute(aPresentation);

  // remove associated data
  TDataXtd_Presentation::Unset(theLabel);
}

//=================================================================================================

occ::handle<TDataXtd_Presentation> TPrsStd_AISPresentation::getData() const
{
  occ::handle<TDataXtd_Presentation> aData;
  if (!Label().FindAttribute(TDataXtd_Presentation::GetID(), aData))
    throw Standard_NoMoreObject(NO_MORE_OBJECT);
  return aData;
}

//=================================================================================================

const Standard_GUID& TPrsStd_AISPresentation::GetID()
{
  static Standard_GUID TPrsStd_AISPresentationID("3680ac6c-47ae-4366-bb94-26abb6e07341");
  return TPrsStd_AISPresentationID;
}

//=================================================================================================

void TPrsStd_AISPresentation::Display(const bool theIsUpdate)
{
  if (theIsUpdate || myAIS.IsNull())
    AISUpdate();

  AISDisplay();
}

//=================================================================================================

void TPrsStd_AISPresentation::Erase(const bool theIsRemove)
{
  if (IsDisplayed() || theIsRemove)
  {
    AISErase(theIsRemove);
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::Update()
{
  AISUpdate();
}

//=================================================================================================

bool TPrsStd_AISPresentation::IsDisplayed() const
{
  return getData()->IsDisplayed();
}

//=================================================================================================

void TPrsStd_AISPresentation::SetDisplayed(const bool theIsDisplayed)
{
  // this method can be called by AISUpdate() in the process of removal,
  // while data attribute may be already removed
  Backup();
  getData()->SetDisplayed(theIsDisplayed);
}

//=================================================================================================

void TPrsStd_AISPresentation::SetDriverGUID(const Standard_GUID& theGUID)
{
  Backup();
  getData()->SetDriverGUID(theGUID);
}

//=================================================================================================

Standard_GUID TPrsStd_AISPresentation::GetDriverGUID() const
{
  return getData()->GetDriverGUID();
}

//=================================================================================================

Graphic3d_NameOfMaterial TPrsStd_AISPresentation::Material() const
{
  return (Graphic3d_NameOfMaterial)getData()->MaterialIndex();
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnMaterial() const
{
  return getData()->HasOwnMaterial();
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetMaterial()
{
  if (!getData()->HasOwnMaterial() && !myAIS.IsNull() && !myAIS->HasMaterial())
    return;

  getData()->UnsetMaterial();

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull() && myAIS->HasMaterial())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->UnsetMaterial(myAIS, false);
    else
      myAIS->UnsetMaterial();
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::SetMaterial(const Graphic3d_NameOfMaterial theName)
{
  Backup();
  if (getData()->HasOwnMode() && getData()->MaterialIndex() == theName)
    if (!myAIS.IsNull() && myAIS->HasMaterial() && myAIS->Material() == theName)
      return;

  getData()->SetMaterialIndex(theName);

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull())
  {
    if (myAIS->HasMaterial() && myAIS->Material() == theName)
      return; // AIS has already had that material

    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->SetMaterial(myAIS, theName, false);
    else
      myAIS->SetMaterial(theName);
  }
}

//=================================================================================================

double TPrsStd_AISPresentation::Transparency() const
{
  return getData()->Transparency();
}

//=================================================================================================

void TPrsStd_AISPresentation::SetTransparency(const double theValue)
{
  Backup();
  if (getData()->HasOwnTransparency() && getData()->Transparency() == theValue)
    if (!myAIS.IsNull() && myAIS->Transparency() == theValue)
      return;

  getData()->SetTransparency(theValue);

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull())
  {
    if (myAIS->Transparency() == theValue)
      return; // AIS has already had that transparency

    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->SetTransparency(myAIS, theValue, false);
    else
      myAIS->SetTransparency(theValue);
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetTransparency()
{
  if (!getData()->HasOwnTransparency())
    return;

  getData()->UnsetTransparency();

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->UnsetTransparency(myAIS, false);
    else
      myAIS->UnsetTransparency();
  }
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnTransparency() const
{
  return getData()->HasOwnTransparency();
}

//=================================================================================================

Quantity_NameOfColor TPrsStd_AISPresentation::Color() const
{
  return getData()->Color();
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnColor() const
{
  return getData()->HasOwnColor();
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetColor()
{
  if (!getData()->HasOwnColor() && !myAIS.IsNull() && !myAIS->HasColor())
    return;
  getData()->UnsetColor();

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull() && myAIS->HasColor())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->UnsetColor(myAIS, false);
    else
      myAIS->UnsetColor();
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::SetColor(const Quantity_NameOfColor theColor)
{
  Backup();
  if (getData()->HasOwnColor() && getData()->Color() == theColor)
  {
    if (!myAIS.IsNull() && myAIS->HasColor())
    {
      Quantity_Color aColor;
      myAIS->Color(aColor);
      if (aColor.Name() == theColor)
      {
        return;
      }
    }
  }

  getData()->SetColor(theColor);

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull())
  {
    if (myAIS->HasColor())
    {
      Quantity_Color aColor;
      myAIS->Color(aColor);
      if (aColor.Name() == theColor)
      {
        return; // AIS has already had that color
      }
    }

    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->SetColor(myAIS, theColor, false);
    else
      myAIS->SetColor(theColor);
  }
}

//=================================================================================================

double TPrsStd_AISPresentation::Width() const
{
  return getData()->Width();
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnWidth() const
{
  return getData()->HasOwnWidth();
}

//=================================================================================================

void TPrsStd_AISPresentation::SetWidth(const double theWidth)
{
  Backup();
  if (getData()->HasOwnWidth() && getData()->Width() == theWidth)
    if (!myAIS.IsNull() && myAIS->HasWidth() && myAIS->Width() == theWidth)
      return;

  getData()->SetWidth(theWidth);

  if (!myAIS.IsNull())
  {
    if (myAIS->HasWidth() && myAIS->Width() == theWidth)
      return; // AIS has already had that width

    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->SetWidth(myAIS, theWidth, false);
    else
      myAIS->SetWidth(theWidth);
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetWidth()
{
  if (!getData()->HasOwnWidth())
    if (!myAIS.IsNull() && !myAIS->HasWidth())
      return;

  getData()->UnsetWidth();

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull() && myAIS->HasWidth())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->UnsetWidth(myAIS, false);
    else
      myAIS->UnsetWidth();
  }
}

//=================================================================================================

int TPrsStd_AISPresentation::Mode() const
{
  return getData()->Mode();
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnMode() const
{
  return getData()->HasOwnMode();
}

//=================================================================================================

void TPrsStd_AISPresentation::SetMode(const int theMode)
{
  Backup();
  if (getData()->HasOwnMode() && getData()->Mode() == theMode)
    if (!myAIS.IsNull() && myAIS->DisplayMode() == theMode)
      return;

  getData()->SetMode(theMode);

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull())
  {
    if (myAIS->DisplayMode() == theMode)
      return; // AIS has already had that mode

    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->SetDisplayMode(myAIS, theMode, false);
    else
      myAIS->SetDisplayMode(theMode);
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetMode()
{
  if (!getData()->HasOwnMode())
    if (!myAIS.IsNull() && !myAIS->HasDisplayMode())
      return;

  getData()->UnsetMode();

  if (myAIS.IsNull())
    AISUpdate();

  if (!myAIS.IsNull() && myAIS->HasDisplayMode())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (!aContext.IsNull())
      aContext->UnsetDisplayMode(myAIS, false);
    else
      myAIS->UnsetDisplayMode();
  }
}

//=======================================================================
// function : GetNbSelectionModes
// purpose  : Returns selection mode(s) of the attribute.
//         : It starts with 1 .. GetNbSelectionModes().
//=======================================================================
int TPrsStd_AISPresentation::GetNbSelectionModes() const
{
  return getData()->GetNbSelectionModes();
}

//=================================================================================================

int TPrsStd_AISPresentation::SelectionMode(const int index) const
{
  return getData()->SelectionMode(index);
}

//=================================================================================================

bool TPrsStd_AISPresentation::HasOwnSelectionMode() const
{
  return getData()->HasOwnSelectionMode();
}

//=================================================================================================

void TPrsStd_AISPresentation::SetSelectionMode(const int  theSelectionMode,
                                               const bool theTransaction)
{
  if (theTransaction)
    Backup();
  getData()->SetSelectionMode(theSelectionMode, theTransaction);

  if (myAIS.IsNull())
    AISUpdate();
  else
    ActivateSelectionMode();
}

//=================================================================================================

void TPrsStd_AISPresentation::AddSelectionMode(const int  theSelectionMode,
                                               const bool theTransaction)
{
  if (theTransaction)
    Backup();
  getData()->AddSelectionMode(theSelectionMode, theTransaction);

  if (myAIS.IsNull())
    AISUpdate();
  else
    ActivateSelectionMode();
}

//=================================================================================================

void TPrsStd_AISPresentation::UnsetSelectionMode()
{
  getData()->UnsetSelectionMode();
  AISUpdate();
}

//=================================================================================================

const Standard_GUID& TPrsStd_AISPresentation::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TPrsStd_AISPresentation::BackupCopy() const
{
  return new TPrsStd_AISPresentation;
}

//=================================================================================================

occ::handle<TDF_Attribute> TPrsStd_AISPresentation::NewEmpty() const
{
  return new TPrsStd_AISPresentation();
}

//=================================================================================================

void TPrsStd_AISPresentation::Restore(const occ::handle<TDF_Attribute>& /*theWith*/)
{
  myAIS.Nullify();
}

//=================================================================================================

void TPrsStd_AISPresentation::Paste(const occ::handle<TDF_Attribute>& theInto,
                                    const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TPrsStd_AISPresentation> anInto = occ::down_cast<TPrsStd_AISPresentation>(theInto);

  anInto->Backup();
  if (!anInto->myAIS.IsNull())
  {
    // Save displayed flag.
    bool displayed = anInto->IsDisplayed();
    // Erase the interactive object.
    anInto->AISErase(true);
    // Restore the displayed flag.
    if (displayed)
      anInto->SetDisplayed(displayed);
  }
  // Nullify the interactive object.
  // It will be restored on the next call to AISUpdate().
  anInto->myAIS.Nullify();
}

//=================================================================================================

void TPrsStd_AISPresentation::AfterAddition()
{
  AfterResume();
}

//=================================================================================================

void TPrsStd_AISPresentation::BeforeRemoval()
{
  BeforeForget();
}

//=================================================================================================

void TPrsStd_AISPresentation::BeforeForget()
{
  if (!myAIS.IsNull())
  { // Remove AISObject from context.
    AISErase(true);
    myAIS.Nullify();
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::AfterResume()
{
  if (IsDisplayed())
  {
    AISUpdate();
    AISDisplay();
  }
  else
    AISErase();
}

//=======================================================================
// function : BeforeUndo
// purpose  : le NamedShape associe doit etre present
//=======================================================================
bool TPrsStd_AISPresentation::BeforeUndo(const occ::handle<TDF_AttributeDelta>& AD, const bool)
{
  occ::handle<TPrsStd_AISPresentation> P;
  AD->Label().FindAttribute(TPrsStd_AISPresentation::GetID(), P);

  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    if (!P.IsNull())
      P->BeforeForget();
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval)))
  {
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    if (!P.IsNull())
      P->BeforeForget();
  }

  return true;
}

//=======================================================================
// function : AfterUndo
// purpose  : le NamedShape associe doit etre present
//=======================================================================
bool TPrsStd_AISPresentation::AfterUndo(const occ::handle<TDF_AttributeDelta>& AD, const bool)
{
  occ::handle<TPrsStd_AISPresentation> P;
  AD->Label().FindAttribute(TPrsStd_AISPresentation::GetID(), P);

  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval)))
  {
    if (!P.IsNull())
      P->AfterAddition();
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    if (!P.IsNull())
      P->AfterResume();
  }

  return true;
}

//=================================================================================================

void TPrsStd_AISPresentation::AISUpdate()
{
  Backup();
  getData()->Backup();
  occ::handle<AIS_InteractiveContext> aContext;
  if (!Label().IsNull())
  {
    aContext = getAISContext();

    occ::handle<TPrsStd_Driver> aDriver;
    if (TPrsStd_DriverTable::Get()->FindDriver(GetDriverGUID(), aDriver))
    {
      // Build a new  AIS.
      if (myAIS.IsNull())
      {
        occ::handle<AIS_InteractiveObject> aNewObj;
        if (aDriver->Update(Label(), aNewObj))
        {
          myAIS = aNewObj;
          aNewObj->SetOwner(this);
        }
      }
      else
      {
        occ::handle<AIS_InteractiveObject> anObj = myAIS;
        if (aDriver->Update(Label(), anObj))
          if (!(anObj == myAIS))
          {
            if (!aContext.IsNull())
              aContext->Remove(myAIS, false);

            // Driver has built new AIS.
            myAIS = anObj;
            anObj->SetOwner(this);
          }
      }
    }
  }
  else
    return;

  // Apply the visualization settings.
  if (!myAIS.IsNull())
  {
    if (HasOwnColor())
    {
      Quantity_NameOfColor aColor = Color();
      Quantity_Color       aPrsColor;
      myAIS->Color(aPrsColor);
      if (!(myAIS->HasColor()) || (myAIS->HasColor() && aPrsColor.Name() != aColor))
      {
        if (!aContext.IsNull())
          aContext->SetColor(myAIS, aColor, false);
        else
          myAIS->SetColor(aColor);
      }
    }

    if (HasOwnMaterial())
    {
      Graphic3d_NameOfMaterial aMaterial = Material();
      if (!(myAIS->HasMaterial()) || (myAIS->HasMaterial() && myAIS->Material() != aMaterial))
      {
        if (!aContext.IsNull())
          aContext->SetMaterial(myAIS, aMaterial, false);
        else
          myAIS->SetMaterial(aMaterial);
      }
    }

    if (HasOwnTransparency())
    {
      const double aTransparency = Transparency();
      if (myAIS->Transparency() != aTransparency)
      {
        if (!aContext.IsNull())
          aContext->SetTransparency(myAIS, aTransparency, false);
        else
          myAIS->SetTransparency(aTransparency);
      }
    }

    if (HasOwnWidth())
    {
      const double aWidth = Width();
      if (!(myAIS->HasWidth()) || (myAIS->HasWidth() && myAIS->Width() != aWidth))
      {
        if (!aContext.IsNull())
          aContext->SetWidth(myAIS, aWidth, false);
        else
          myAIS->SetWidth(aWidth);
      }
    }

    if (HasOwnMode())
    {
      const int aMode = Mode();
      if (myAIS->DisplayMode() != aMode)
        myAIS->SetDisplayMode(aMode);
    }

    ActivateSelectionMode();
  }

  if (IsDisplayed() && !aContext.IsNull())
    aContext->Redisplay(myAIS, false);
}

//=================================================================================================

void TPrsStd_AISPresentation::AISDisplay()
{
  if (!Label().IsNull())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();

    if (aContext.IsNull())
      return;

    if (!myAIS.IsNull())
    {
      if (!(myAIS->GetContext()).IsNull() && (myAIS->GetContext()) != aContext)
        myAIS->GetContext()->Remove(myAIS, false);

      if (IsDisplayed() && aContext->IsDisplayed(myAIS))
        return;

      aContext->Display(myAIS, false);

      if (aContext->IsDisplayed(myAIS))
        SetDisplayed(true);
    }
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::AISErase(const bool theIsRemove)
{
  occ::handle<AIS_InteractiveContext> aContext, anOwnContext;

  if (!myAIS.IsNull())
  {
    Backup();
    if (!Label().IsNull())
    {
      if (IsAttribute(TDataXtd_Presentation::GetID()))
        SetDisplayed(false);

      occ::handle<TPrsStd_AISViewer> viewer;
      if (!TPrsStd_AISViewer::Find(Label(), viewer))
        return;
      anOwnContext = myAIS->GetContext();
      aContext     = viewer->GetInteractiveContext();

      if (theIsRemove)
      {
        if (!aContext.IsNull())
          aContext->Remove(myAIS, false);
        if (!anOwnContext.IsNull() && anOwnContext != aContext)
          anOwnContext->Remove(myAIS, false);

        myAIS.Nullify();
      }
      else
      {
        if (!aContext.IsNull())
          aContext->Erase(myAIS, false);
        if (!anOwnContext.IsNull() && anOwnContext != aContext)
          anOwnContext->Erase(myAIS, false);
      }
    }
    else
    {
      if (theIsRemove)
      {
        if (!anOwnContext.IsNull())
        {
          anOwnContext->Remove(myAIS, false);
          myAIS.Nullify();
        }
      }
      else if (!anOwnContext.IsNull())
        anOwnContext->Erase(myAIS, false);
    }
  }
}

//=================================================================================================

occ::handle<AIS_InteractiveObject> TPrsStd_AISPresentation::GetAIS() const
{
  return myAIS;
}

//=================================================================================================

occ::handle<AIS_InteractiveContext> TPrsStd_AISPresentation::getAISContext() const
{
  occ::handle<TPrsStd_AISViewer> aViewer;
  if (TPrsStd_AISViewer::Find(Label(), aViewer))
    return aViewer->GetInteractiveContext();

  return occ::handle<AIS_InteractiveContext>();
}

//=======================================================================
// function : ActivateSelectionMode
// purpose  : Activates selection mode of the interactive object.
//           It is called internally on change of selection mode and AISUpdate().
//=======================================================================
void TPrsStd_AISPresentation::ActivateSelectionMode()
{
  if (!myAIS.IsNull() && HasOwnSelectionMode())
  {
    occ::handle<AIS_InteractiveContext> aContext = getAISContext();
    if (!aContext.IsNull())
    {
      NCollection_List<int> anActivatedModes;
      aContext->ActivatedModes(myAIS, anActivatedModes);
      int nbSelModes = GetNbSelectionModes();
      if (nbSelModes == 1)
      {
        bool isActivated    = false;
        int  aSelectionMode = SelectionMode();
        if (aSelectionMode == -1)
        {
          aContext->Deactivate(myAIS);
        }
        else
        {
          for (NCollection_List<int>::Iterator aModeIter(anActivatedModes); aModeIter.More();
               aModeIter.Next())
          {
            if (aModeIter.Value() == aSelectionMode)
            {
              isActivated = true;
              break;
            }
          }
          if (!isActivated)
            aContext->SetSelectionModeActive(myAIS,
                                             aSelectionMode,
                                             true,
                                             AIS_SelectionModesConcurrency_Multiple);
        }
      }
      else
      {
        for (int iSelMode = 1; iSelMode <= nbSelModes; iSelMode++)
        {
          const int aSelectionMode = SelectionMode(iSelMode);
          aContext->SetSelectionModeActive(myAIS,
                                           aSelectionMode,
                                           true /*activate*/,
                                           iSelMode == 1 ? AIS_SelectionModesConcurrency_Single
                                                         : AIS_SelectionModesConcurrency_Multiple);
        }
      }
    }
  }
}

//=================================================================================================

void TPrsStd_AISPresentation::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAIS.get())
}
