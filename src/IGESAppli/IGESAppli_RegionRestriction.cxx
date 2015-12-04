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

#include <IGESAppli_RegionRestriction.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_RegionRestriction,IGESData_IGESEntity)

IGESAppli_RegionRestriction::IGESAppli_RegionRestriction ()    {  }


    void  IGESAppli_RegionRestriction::Init
  (const Standard_Integer nbPropVal,  const Standard_Integer aViasRest,
   const Standard_Integer aCompoRest, const Standard_Integer aCktRest)
{
  theNbPropertyValues  = nbPropVal;
  theElectViasRestrict = aViasRest;
  theElectCompRestrict = aCompoRest;
  theElectCktRestrict  = aCktRest;
  InitTypeAndForm(406,2);
}


    Standard_Integer  IGESAppli_RegionRestriction::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalViasRestriction () const
{
  return theElectViasRestrict;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalComponentRestriction
  () const
{
  return theElectCompRestrict;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalCktRestriction () const
{
  return theElectCktRestrict;
}
