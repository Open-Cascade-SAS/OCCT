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

#include <IGESAppli_LineWidening.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LineWidening, IGESData_IGESEntity)

IGESAppli_LineWidening::IGESAppli_LineWidening() {}

void IGESAppli_LineWidening::Init(const int    nbPropVal,
                                  const double aWidth,
                                  const int    aCornering,
                                  const int    aExtnFlag,
                                  const int    aJustifFlag,
                                  const double aExtnVal)
{
  theNbPropertyValues  = nbPropVal;
  theWidth             = aWidth;
  theCorneringCode     = aCornering;
  theExtensionFlag     = aExtnFlag;
  theJustificationFlag = aJustifFlag;
  theExtensionValue    = aExtnVal;
  InitTypeAndForm(406, 5);
}

int IGESAppli_LineWidening::NbPropertyValues() const
{
  return theNbPropertyValues;
}

double IGESAppli_LineWidening::WidthOfMetalization() const
{
  return theWidth;
}

int IGESAppli_LineWidening::CorneringCode() const
{
  return theCorneringCode;
}

int IGESAppli_LineWidening::ExtensionFlag() const
{
  return theExtensionFlag;
}

int IGESAppli_LineWidening::JustificationFlag() const
{
  return theJustificationFlag;
}

double IGESAppli_LineWidening::ExtensionValue() const
{
  return theExtensionValue;
}
