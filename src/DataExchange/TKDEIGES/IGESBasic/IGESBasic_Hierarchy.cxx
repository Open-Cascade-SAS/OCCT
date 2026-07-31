// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_Hierarchy.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_Hierarchy, IGESData_IGESEntity)

IGESBasic_Hierarchy::IGESBasic_Hierarchy() = default;

void IGESBasic_Hierarchy::Init(const int nbPropVal,
                               const int aLineFont,
                               const int aView,
                               const int anEntityLevel,
                               const int aBlankStatus,
                               const int aLineWt,
                               const int aColorNum)
{
  theLineFont         = aLineFont;
  theView             = aView;
  theEntityLevel      = anEntityLevel;
  theBlankStatus      = aBlankStatus;
  theLineWeight       = aLineWt;
  theColorNum         = aColorNum;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406, 10);
}

int IGESBasic_Hierarchy::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESBasic_Hierarchy::NewLineFont() const
{
  return theLineFont;
}

int IGESBasic_Hierarchy::NewView() const
{
  return theView;
}

int IGESBasic_Hierarchy::NewEntityLevel() const
{
  return theEntityLevel;
}

int IGESBasic_Hierarchy::NewBlankStatus() const
{
  return theBlankStatus;
}

int IGESBasic_Hierarchy::NewLineWeight() const
{
  return theLineWeight;
}

int IGESBasic_Hierarchy::NewColorNum() const
{
  return theColorNum;
}
