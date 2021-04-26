// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/TreeModel_ItemStream.hxx>

#include <inspector/TreeModel_ItemProperties.hxx>

#include <Standard_Dump.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ItemStream::TreeModel_ItemStream (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
 : TreeModel_ItemBase (theParent, theRow, theColumn)
{
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TreeModel_ItemStream::Init()
{
  TreeModel_ItemBase::Init();

  int aStreamChildrenCount = 0;
  if (Column() == 0)
  {
    if (!myProperties)
    {
      myProperties = new TreeModel_ItemProperties();
      myProperties->SetItem (currentItem());
    }
    myProperties->Init();
    aStreamChildrenCount = myProperties->Children().Extent();
  }
  m_iStreamChildren = aStreamChildrenCount;
  initStream (myStream);
}

// =======================================================================
// function : Rest
// purpose :
// =======================================================================
void TreeModel_ItemStream::Reset()
{
  myStream.str ("");
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemStream::initValue (const int theItemRole) const
{
  QVariant aParentValue = TreeModel_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0)
    return QVariant();

  if (theItemRole == Qt::ForegroundRole)
    return QColor (Qt::darkBlue);

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return Properties() ? Properties()->Key().ToCString() : "";
  }
  return QVariant();
}

// =======================================================================
// function : StoreItemProperties
// purpose :
// =======================================================================
void TreeModel_ItemStream::StoreItemProperties (const int, const int, const QVariant& theValue)
{
  Parent()->StoreItemProperties (-1, -1, theValue);
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void TreeModel_ItemStream::initStream (Standard_OStream& theOStream) const
{
  if (!Properties())
    return;

  theOStream << Properties()->StreamValue();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void TreeModel_ItemStream::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<TreeModel_ItemStream*>(this)->Init();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemStream::createChild (int theRow, int theColumn)
{
  return TreeModel_ItemStream::CreateItem (currentItem(), theRow, theColumn);
}
