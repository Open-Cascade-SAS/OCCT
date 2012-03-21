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

#include <IGESDimen_DiameterDimension.ixx>
#include <gp_XYZ.hxx>
#include <gp_GTrsf.hxx>


IGESDimen_DiameterDimension::IGESDimen_DiameterDimension ()    {  }


    void  IGESDimen_DiameterDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader,
   const gp_XY& aCenter)
{
  theNote         = aNote;
  theFirstLeader  = aLeader;
  theSecondLeader = anotherLeader;
  theCenter       = aCenter;
  InitTypeAndForm(206,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_DiameterDimension::Note () const 
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_DiameterDimension::FirstLeader
  () const 
{
  return theFirstLeader;
}

    Standard_Boolean  IGESDimen_DiameterDimension::HasSecondLeader () const 
{
  return (! theSecondLeader.IsNull());
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_DiameterDimension::SecondLeader
  () const 
{
  return theSecondLeader;
}

    gp_Pnt2d  IGESDimen_DiameterDimension::Center () const 
{
  gp_Pnt2d center(theCenter);
  return center;
}

    gp_Pnt2d  IGESDimen_DiameterDimension::TransformedCenter () const 
{
  gp_XYZ center(theCenter.X(), theCenter.Y(), 0);
  if (HasTransf()) Location().Transforms(center);
  return gp_Pnt2d(center.X(), center.Y());
}
