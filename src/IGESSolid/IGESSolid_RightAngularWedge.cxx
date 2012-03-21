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

#include <IGESSolid_RightAngularWedge.ixx>
#include <gp_GTrsf.hxx>


IGESSolid_RightAngularWedge::IGESSolid_RightAngularWedge ()    {  }


    void  IGESSolid_RightAngularWedge::Init
  (const gp_XYZ& aSize,   const Standard_Real LowX,
   const gp_XYZ& aCorner, const gp_XYZ& anXAxis, const gp_XYZ& anZAxis)
{
  theSize         = aSize;
  theXSmallLength = LowX;
  theCorner       = aCorner;         // default (0,0,0)
  theXAxis        = anXAxis;         // default (1,0,0)
  theZAxis        = anZAxis;         // default (0,0,1)
  InitTypeAndForm(152,0);
}

    gp_XYZ  IGESSolid_RightAngularWedge::Size () const
{
  return theSize;
}

    Standard_Real  IGESSolid_RightAngularWedge::XBigLength () const
{
  return theSize.X();
}

    Standard_Real  IGESSolid_RightAngularWedge::XSmallLength () const
{
  return theXSmallLength;
}

    Standard_Real  IGESSolid_RightAngularWedge::YLength () const
{
  return theSize.Y();
}

    Standard_Real  IGESSolid_RightAngularWedge::ZLength () const
{
  return theSize.Z();
}

    gp_Pnt  IGESSolid_RightAngularWedge::Corner () const
{
  return gp_Pnt(theCorner);
}

    gp_Pnt  IGESSolid_RightAngularWedge::TransformedCorner () const
{
  if (!HasTransf()) return gp_Pnt(theCorner);
  else
    {
      gp_XYZ tmp = theCorner;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_RightAngularWedge::XAxis () const
{
  return gp_Dir(theXAxis);
}

    gp_Dir  IGESSolid_RightAngularWedge::TransformedXAxis () const
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

    gp_Dir  IGESSolid_RightAngularWedge::YAxis () const
{
  return gp_Dir(theXAxis ^ theZAxis);     // ^ overloaded
}

    gp_Dir  IGESSolid_RightAngularWedge::TransformedYAxis () const
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

    gp_Dir  IGESSolid_RightAngularWedge::ZAxis () const
{
  return gp_Dir(theZAxis);
}

    gp_Dir  IGESSolid_RightAngularWedge::TransformedZAxis () const
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
