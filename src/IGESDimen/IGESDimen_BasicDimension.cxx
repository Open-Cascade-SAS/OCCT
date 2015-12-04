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

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESDimen_BasicDimension.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_BasicDimension,IGESData_IGESEntity)

IGESDimen_BasicDimension::IGESDimen_BasicDimension ()    {  }

    void  IGESDimen_BasicDimension::Init
  (const Standard_Integer nbPropVal, 
   const gp_XY& thell, const gp_XY& thelr,
   const gp_XY& theur, const gp_XY& theul)
{
  theNbPropertyValues = nbPropVal;
  theLowerLeft  = thell;
  theLowerRight = thelr;
  theUpperRight = theur;
  theUpperLeft  = theul;
  InitTypeAndForm(406,31);
}


    Standard_Integer  IGESDimen_BasicDimension::NbPropertyValues () const 
{
  return theNbPropertyValues;
}

    gp_Pnt2d  IGESDimen_BasicDimension::LowerLeft () const 
{
  gp_Pnt2d g(theLowerLeft);
  return g;
}


    gp_Pnt2d  IGESDimen_BasicDimension::LowerRight () const 
{
  gp_Pnt2d g(theLowerRight);
  return g;
}

    gp_Pnt2d  IGESDimen_BasicDimension::UpperRight () const 
{
  gp_Pnt2d g(theUpperRight);
  return g;
}

    gp_Pnt2d  IGESDimen_BasicDimension::UpperLeft () const 
{
  gp_Pnt2d g(theUpperLeft);
  return g;
}
