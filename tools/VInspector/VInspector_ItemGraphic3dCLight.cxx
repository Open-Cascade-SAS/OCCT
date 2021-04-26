// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <inspector/VInspector_ItemGraphic3dCLight.hxx>

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemContextProperties.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCLight::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dCLight::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole == Qt::ForegroundRole)
  {
    if (GetLight().IsNull())
      return QVariant();

    if (!GetLight()->IsEnabled())
      return QColor(Qt::darkGray);

    return QVariant();
  }

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetLight().IsNull())
    return Column() == 0 ? "Empty light" : "";

  if (Column() != 0)
    return QVariant();

  switch (GetLight()->Type())
  {
    case Graphic3d_TOLS_AMBIENT:     return "Ambient light";
    case Graphic3d_TOLS_DIRECTIONAL: return "Directional light";
    case Graphic3d_TOLS_POSITIONAL:  return "Positional light";
    case Graphic3d_TOLS_SPOT:        return "Spot light";
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCLight::Init()
{
  VInspector_ItemContextPropertiesPtr aParentItem = itemDynamicCast<VInspector_ItemContextProperties>(Parent());
  Handle(Graphic3d_CLight) aLight;
  if (aParentItem)
  {
    VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
    if (aParentContextItem)
    {
      Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
      Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
      if (!aViewer.IsNull())
      {
        int aLightId = Row() - 2 /*in parent*/;
        int aCurrentId = 0;
        for (V3d_ListOfLightIterator aLightsIt (aViewer->ActiveLightIterator()); aLightsIt.More(); aLightsIt.Next(), aCurrentId++)
        {
          if (aCurrentId != aLightId)
            continue;
          aLight = aLightsIt.Value();
        }
      }
    }
  }
  myLight = aLight;
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCLight::Reset()
{
  VInspector_ItemBase::Reset();

  myLight = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCLight::StoreItemProperties (const int theRow, const int theColumn, const QVariant& theValue)
{
  if (myLight.IsNull())
    return;

  const Handle(TreeModel_ItemProperties)& aProperties = Properties();
  if (theRow == -1 && theColumn == -1) // processing sub-item value
  {
    Standard_SStream aStream;
    aStream << theValue.toString().toStdString().c_str();

    int aStartPos = 1;
    Quantity_ColorRGBA aColor;
    if (aColor.InitFromJson (aStream, aStartPos))
    {
      myLight->SetColor (aColor.GetRGB());
      return;
    }

    // "Direction"
    gp_Dir aDir;
    if (aDir.InitFromJson (aStream, aStartPos))
    {
      myLight->SetDirection (aDir);
      return;
    }

    // "Position"
    gp_Pnt aPnt;
    if (aPnt.InitFromJson (aStream, aStartPos))
    {
      myLight->SetPosition (aPnt);
      return;
    }
  }

  QString aPropertyName = aProperties->Data(theRow, 0).toString();
  QVariant aPropertyValue = aProperties->Data(theRow, 1);
  if (aPropertyName == "Position")
  {
  }
  else if (aPropertyName == "Intensity")
  {
    myLight->SetIntensity ((Standard_ShortReal)aPropertyValue.toReal());
  }
  else if (aPropertyName == "ConstAttenuation")
  {
    myLight->SetAttenuation ((Standard_ShortReal)aPropertyValue.toReal(),
                             (Standard_ShortReal)aProperties->Data(theRow + 1, 1).toReal());
  }
  else if (aPropertyName == "LinearAttenuation")
  {
    myLight->SetAttenuation ((Standard_ShortReal)aProperties->Data(theRow - 1, 1).toReal(),
                             (Standard_ShortReal)aPropertyValue.toReal());
  }
  else if (aPropertyName == "Angle")
  {
    myLight->SetAngle ((Standard_ShortReal)aPropertyValue.toReal());
  }
  else if (aPropertyName == "Concentration")
  {
    myLight->SetConcentration ((Standard_ShortReal)aPropertyValue.toReal());
  }
  else if (aPropertyName == "Range")
  {
    myLight->SetRange ((Standard_ShortReal)aPropertyValue.toReal());
  }
  else if (aPropertyName == "Smoothness")
  {
    if (myLight->Type() == Graphic3d_TOLS_DIRECTIONAL)
    {
      myLight->SetSmoothAngle ((Standard_ShortReal)aPropertyValue.toReal());
    }
    else
    {
      myLight->SetSmoothRadius ((Standard_ShortReal)aPropertyValue.toReal());
    }
  }
  else if (aPropertyName == "IsHeadlight")
  {
    myLight->SetHeadlight (aPropertyValue.toInt() == 1);
  }
  else if (aPropertyName == "IsEnabled")
  {
    myLight->SetEnabled (aPropertyValue.toInt() == 1);
  }
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCLight::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dCLight*>(this)->Init();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCLight::initStream (Standard_OStream& theOStream) const
{
  Handle(Graphic3d_CLight) aLight = GetLight();
  if (aLight.IsNull())
    return;

  aLight->DumpJson (theOStream);
}
