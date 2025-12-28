// Created on: 2003-11-27
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <MeshVS_Drawer.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_Drawer, Standard_Transient)

//=================================================================================================

void MeshVS_Drawer::Assign(const occ::handle<MeshVS_Drawer>& aDrawer)
{
  if (!aDrawer.IsNull())
  {
    myIntegers    = aDrawer->myIntegers;
    myDoubles     = aDrawer->myDoubles;
    myBooleans    = aDrawer->myBooleans;
    myColors      = aDrawer->myColors;
    myMaterials   = aDrawer->myMaterials;
    myAsciiString = aDrawer->myAsciiString;
  }
}

//=================================================================================================

void MeshVS_Drawer::SetInteger(const int Key, const int Value)
{
  if (myIntegers.IsBound(Key))
    myIntegers.ChangeFind(Key) = Value;
  else
    myIntegers.Bind(Key, Value);
}

//=================================================================================================

void MeshVS_Drawer::SetDouble(const int Key, const double Value)
{
  if (myDoubles.IsBound(Key))
    myDoubles.ChangeFind(Key) = Value;
  else
    myDoubles.Bind(Key, Value);
}

//=================================================================================================

void MeshVS_Drawer::SetBoolean(const int Key, const bool Value)
{
  if (myBooleans.IsBound(Key))
    myBooleans.ChangeFind(Key) = Value;
  else
    myBooleans.Bind(Key, Value);
}

//=================================================================================================

void MeshVS_Drawer::SetColor(const int Key, const Quantity_Color& Value)
{
  if (myColors.IsBound(Key))
    myColors.ChangeFind(Key) = Value;
  else
    myColors.Bind(Key, Value);
}

//=================================================================================================

void MeshVS_Drawer::SetMaterial(const int Key, const Graphic3d_MaterialAspect& Value)
{
  if (myMaterials.IsBound(Key))
    myMaterials.ChangeFind(Key) = Value;
  else
    myMaterials.Bind(Key, Value);
}

//=================================================================================================

void MeshVS_Drawer::SetAsciiString(const int Key, const TCollection_AsciiString& Value)
{
  if (myAsciiString.IsBound(Key))
    myAsciiString.ChangeFind(Key) = Value;
  else
    myAsciiString.Bind(Key, Value);
}

//=================================================================================================

bool MeshVS_Drawer::GetInteger(const int Key,
                                           int&      Value) const
{
  bool aRes = myIntegers.IsBound(Key);
  if (aRes)
    Value = myIntegers.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::GetDouble(const int Key, double& Value) const
{
  bool aRes = myDoubles.IsBound(Key);
  if (aRes)
    Value = myDoubles.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::GetBoolean(const int Key,
                                           bool&      Value) const
{
  bool aRes = myBooleans.IsBound(Key);
  if (aRes)
    Value = myBooleans.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::GetColor(const int Key, Quantity_Color& Value) const
{
  bool aRes = myColors.IsBound(Key);
  if (aRes)
    Value = myColors.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::GetMaterial(const int    Key,
                                            Graphic3d_MaterialAspect& Value) const
{
  bool aRes = myMaterials.IsBound(Key);
  if (aRes)
    Value = myMaterials.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::GetAsciiString(const int   Key,
                                               TCollection_AsciiString& Value) const
{
  bool aRes = myAsciiString.IsBound(Key);
  if (aRes)
    Value = myAsciiString.Find(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveInteger(const int Key)
{
  bool aRes = myIntegers.IsBound(Key);
  if (aRes)
    myIntegers.UnBind(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveDouble(const int Key)
{
  bool aRes = myDoubles.IsBound(Key);
  if (aRes)
    myDoubles.UnBind(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveBoolean(const int Key)
{
  bool aRes = myBooleans.IsBound(Key);
  if (aRes)
    myBooleans.UnBind(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveColor(const int Key)
{
  bool aRes = myColors.IsBound(Key);
  if (aRes)
    myColors.UnBind(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveMaterial(const int Key)
{
  bool aRes = myMaterials.IsBound(Key);
  if (aRes)
    myMaterials.UnBind(Key);
  return aRes;
}

//=================================================================================================

bool MeshVS_Drawer::RemoveAsciiString(const int Key)
{
  bool aRes = myAsciiString.IsBound(Key);
  if (aRes)
    myAsciiString.UnBind(Key);
  return aRes;
}
