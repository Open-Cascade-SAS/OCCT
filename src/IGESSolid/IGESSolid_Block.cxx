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
#include <IGESSolid_Block.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Block,IGESData_IGESEntity)

IGESSolid_Block::IGESSolid_Block ()    {  }


    void  IGESSolid_Block::Init
  (const gp_XYZ& aSize, const gp_XYZ& aCorner,
   const gp_XYZ& aXAxis, const gp_XYZ& aZAxis)
{
  theSize   = aSize;
  theCorner = aCorner;         // default (0,0,0)
  theXAxis  = aXAxis;          // default (1,0,0)
  theZAxis  = aZAxis;          // default (0,0,1)
  InitTypeAndForm(150,0);
}

    gp_XYZ  IGESSolid_Block::Size () const
{
  return theSize;
}

    Standard_Real  IGESSolid_Block::XLength () const
{
  return theSize.X();
}

    Standard_Real  IGESSolid_Block::YLength () const
{
  return theSize.Y();
}

    Standard_Real  IGESSolid_Block::ZLength () const
{
  return theSize.Z();
}

    gp_Pnt  IGESSolid_Block::Corner () const
{
  return gp_Pnt(theCorner);
}

    gp_Pnt  IGESSolid_Block::TransformedCorner () const
{
  if (!HasTransf()) return gp_Pnt(theCorner);
  else
    {
      gp_XYZ tmp = theCorner;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Block::XAxis () const
{
  return gp_Dir(theXAxis);
}

    gp_Dir  IGESSolid_Block::TransformedXAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis);
  else
    {
      gp_XYZ xyz = theXAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}

    gp_Dir  IGESSolid_Block::YAxis () const
{
  return gp_Dir(theXAxis ^ theZAxis);     // ^ overloaded
}

    gp_Dir  IGESSolid_Block::TransformedYAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis ^ theZAxis);
  else
    {
      gp_XYZ xyz = theXAxis ^ theZAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}

    gp_Dir  IGESSolid_Block::ZAxis () const
{
  return gp_Dir(theZAxis);
}

    gp_Dir  IGESSolid_Block::TransformedZAxis () const
{
  if (!HasTransf()) return gp_Dir(theZAxis);
  else
    {
      gp_XYZ xyz(theZAxis);
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}
