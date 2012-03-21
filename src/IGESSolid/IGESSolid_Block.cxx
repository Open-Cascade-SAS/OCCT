// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_Block.ixx>
#include <gp_GTrsf.hxx>


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
