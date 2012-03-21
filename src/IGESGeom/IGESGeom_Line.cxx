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

#include <IGESGeom_Line.ixx>
#include <gp_GTrsf.hxx>


IGESGeom_Line::IGESGeom_Line ()    {  }


    void IGESGeom_Line::Init
  (const gp_XYZ& aStart, const gp_XYZ& anEnd)
{
  theStart = aStart;
  theEnd   = anEnd;
  InitTypeAndForm(110,0);
}

    Standard_Integer  IGESGeom_Line::Infinite () const
      {  return FormNumber();  }

    void  IGESGeom_Line::SetInfinite (const Standard_Integer status)
      {  if (status >= 0 && status <= 2) InitTypeAndForm(110,status);  }


    gp_Pnt IGESGeom_Line::StartPoint () const
{
  gp_Pnt start(theStart);
  return start;
}

    gp_Pnt IGESGeom_Line::TransformedStartPoint () const
{
  gp_XYZ Start = theStart;
  if (HasTransf()) Location().Transforms(Start);
  gp_Pnt transStart(Start);
  return transStart;
}

    gp_Pnt IGESGeom_Line::EndPoint () const
{
  gp_Pnt end(theEnd);
  return end;
}

    gp_Pnt IGESGeom_Line::TransformedEndPoint () const
{
  gp_XYZ End = theEnd;
  if (HasTransf()) Location().Transforms(End);
  gp_Pnt transEnd(End);
  return transEnd;
}
