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

#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Ellipsoid,IGESData_IGESEntity)

IGESSolid_Ellipsoid::IGESSolid_Ellipsoid ()    {  }


    void  IGESSolid_Ellipsoid::Init
  (const gp_XYZ& aSize,   const gp_XYZ& aCenter,
   const gp_XYZ& anXAxis, const gp_XYZ& anZAxis)
{
  theSize   = aSize;
  theCenter = aCenter;         // default (0,0,0)
  theXAxis  = anXAxis;         // default (1,0,0)
  theZAxis  = anZAxis;         // default (0,0,1)
  InitTypeAndForm(168,0);
}

    gp_XYZ  IGESSolid_Ellipsoid::Size () const
{
  return theSize;
}

    Standard_Real  IGESSolid_Ellipsoid::XLength () const
{
  return theSize.X();
}

    Standard_Real  IGESSolid_Ellipsoid::YLength () const
{
  return theSize.Y();
}

    Standard_Real  IGESSolid_Ellipsoid::ZLength () const
{
  return theSize.Z();
}

    gp_Pnt  IGESSolid_Ellipsoid::Center () const
{
  return gp_Pnt(theCenter);
}

    gp_Pnt  IGESSolid_Ellipsoid::TransformedCenter () const
{
  if (!HasTransf()) return gp_Pnt(theCenter);
  else
    {
      gp_XYZ tmp = theCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Ellipsoid::XAxis () const
{
  return gp_Dir(theXAxis);
}

    gp_Dir  IGESSolid_Ellipsoid::TransformedXAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis);
  else
    {
      gp_XYZ tmp = theXAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}

    gp_Dir  IGESSolid_Ellipsoid::YAxis () const
{
  return gp_Dir(theXAxis ^ theZAxis);     // ^ overloaded
}

    gp_Dir  IGESSolid_Ellipsoid::TransformedYAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis ^ theZAxis);
  else
    {
      gp_XYZ tmp = theXAxis ^ theZAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}

    gp_Dir  IGESSolid_Ellipsoid::ZAxis () const
{
  return gp_Dir(theZAxis);
}

    gp_Dir  IGESSolid_Ellipsoid::TransformedZAxis () const
{
  if (!HasTransf()) return gp_Dir(theZAxis);
  else
    {
      gp_XYZ tmp = theZAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
