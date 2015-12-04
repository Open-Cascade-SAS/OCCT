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

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_DrilledHole,IGESData_IGESEntity)

IGESAppli_DrilledHole::IGESAppli_DrilledHole ()    {  }

    void  IGESAppli_DrilledHole::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aSize,    const Standard_Real    anotherSize,
   const Standard_Integer aPlating, const Standard_Integer aLayer,
   const Standard_Integer anotherLayer)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiaSize     = aSize;
  theFinishDiaSize    = anotherSize;
  thePlatingFlag      = aPlating;
  theNbLowerLayer     = aLayer;
  theNbHigherLayer    = anotherLayer;
  InitTypeAndForm(406,6);
}


    Standard_Integer  IGESAppli_DrilledHole::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real  IGESAppli_DrilledHole::DrillDiaSize () const
{
  return theDrillDiaSize;
}

    Standard_Real  IGESAppli_DrilledHole::FinishDiaSize () const
{
  return theFinishDiaSize;
}

    Standard_Boolean  IGESAppli_DrilledHole::IsPlating () const
{
  return (thePlatingFlag != 0);
}

    Standard_Integer  IGESAppli_DrilledHole::NbLowerLayer () const
{
  return theNbLowerLayer;
}

    Standard_Integer  IGESAppli_DrilledHole::NbHigherLayer () const
{
  return theNbHigherLayer;
}
