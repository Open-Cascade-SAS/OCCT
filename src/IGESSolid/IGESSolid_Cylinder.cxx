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
#include <IGESSolid_Cylinder.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Cylinder,IGESData_IGESEntity)

IGESSolid_Cylinder::IGESSolid_Cylinder ()    {  }


    void  IGESSolid_Cylinder::Init
  (const Standard_Real aHeight, const Standard_Real aRadius,
   const gp_XYZ& aCenter, const gp_XYZ& anAxis)
{
  theHeight     = aHeight;
  theRadius     = aRadius;
  theFaceCenter = aCenter;
  theAxis       = anAxis;
  InitTypeAndForm(154,0);
}

    Standard_Real  IGESSolid_Cylinder::Height () const
{
  return theHeight;
}

    Standard_Real  IGESSolid_Cylinder::Radius () const
{
  return theRadius;
}

    gp_Pnt  IGESSolid_Cylinder::FaceCenter () const
{
  return gp_Pnt(theFaceCenter);
}

    gp_Pnt  IGESSolid_Cylinder::TransformedFaceCenter () const
{
  if (!HasTransf()) return gp_Pnt(theFaceCenter);
  else
    {
      gp_XYZ tmp = theFaceCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Cylinder::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_Cylinder::TransformedAxis () const
{
  if (!HasTransf()) return gp_Dir(theAxis);
  else
    {
      gp_XYZ tmp = theAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
