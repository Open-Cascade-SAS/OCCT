// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/***********************************************************************


************************************************************************/

// for the class
#include <Visual3d_LayerItem.ixx>

//
//-Constructors
//
Visual3d_LayerItem::Visual3d_LayerItem() :
MyIsNeedToRecompute(Standard_False)
{
}

void Visual3d_LayerItem::ComputeLayerPrs()
{
  SetNeedToRecompute( Standard_False );
}

void Visual3d_LayerItem::RedrawLayerPrs()
{
  if ( IsNeedToRecompute() )
    ComputeLayerPrs();
}

Standard_Boolean Visual3d_LayerItem::IsNeedToRecompute() const
{
  return MyIsNeedToRecompute;
}

void Visual3d_LayerItem::SetNeedToRecompute( const Standard_Boolean flag )
{
  MyIsNeedToRecompute = flag;
}
