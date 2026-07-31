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

#include <IGESAppli_DrilledHole.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_DrilledHole, IGESData_IGESEntity)

IGESAppli_DrilledHole::IGESAppli_DrilledHole() = default;

void IGESAppli_DrilledHole::Init(const int    nbPropVal,
                                 const double aSize,
                                 const double anotherSize,
                                 const int    aPlating,
                                 const int    aLayer,
                                 const int    anotherLayer)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiaSize     = aSize;
  theFinishDiaSize    = anotherSize;
  thePlatingFlag      = aPlating;
  theNbLowerLayer     = aLayer;
  theNbHigherLayer    = anotherLayer;
  InitTypeAndForm(406, 6);
}

int IGESAppli_DrilledHole::NbPropertyValues() const
{
  return theNbPropertyValues;
}

double IGESAppli_DrilledHole::DrillDiaSize() const
{
  return theDrillDiaSize;
}

double IGESAppli_DrilledHole::FinishDiaSize() const
{
  return theFinishDiaSize;
}

bool IGESAppli_DrilledHole::IsPlating() const
{
  return (thePlatingFlag != 0);
}

int IGESAppli_DrilledHole::NbLowerLayer() const
{
  return theNbLowerLayer;
}

int IGESAppli_DrilledHole::NbHigherLayer() const
{
  return theNbHigherLayer;
}
