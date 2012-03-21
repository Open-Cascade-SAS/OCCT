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

#include <IGESDimen_RadiusDimension.ixx>
#include <gp_GTrsf.hxx>


IGESDimen_RadiusDimension::IGESDimen_RadiusDimension ()    {  }


    void IGESDimen_RadiusDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& anArrow,
   const gp_XY& arcCenter,
   const Handle(IGESDimen_LeaderArrow)& anotherArrow)
{
  theNote        = aNote;
  theLeaderArrow = anArrow;
  theCenter      = arcCenter;
  theLeader2     = anotherArrow;
  if (!anotherArrow.IsNull()) InitTypeAndForm(222, 1);  // 1 admet aussi Null
  else InitTypeAndForm(222,FormNumber());
}

    void IGESDimen_RadiusDimension::InitForm (const Standard_Integer form)
{
  InitTypeAndForm(222,form);
}

    Handle(IGESDimen_GeneralNote) IGESDimen_RadiusDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader () const
{
  return theLeaderArrow;
}

    Standard_Boolean IGESDimen_RadiusDimension::HasLeader2 () const
{
  return (!theLeader2.IsNull());
}

    gp_Pnt2d IGESDimen_RadiusDimension::Center () const
{
  gp_Pnt2d g(theCenter);
  return g;
}

    gp_Pnt IGESDimen_RadiusDimension::TransformedCenter () const
{
  gp_XYZ tmpXYZ(theCenter.X(), theCenter.Y(), theLeaderArrow->ZDepth());
  if (HasTransf()) Location().Transforms(tmpXYZ);
  return gp_Pnt(tmpXYZ);
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader2 () const
{
  return theLeader2;
}
