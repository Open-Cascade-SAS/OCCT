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

#include <IGESGeom_Point.ixx>
#include <gp_GTrsf.hxx>


IGESGeom_Point::IGESGeom_Point ()     {  }


    void IGESGeom_Point::Init
  (const gp_XYZ& aPoint, 
   const Handle(IGESBasic_SubfigureDef)& aSymbol)
{
  thePoint  = aPoint;
  theSymbol = aSymbol;
  InitTypeAndForm(116,0);
}

    gp_Pnt IGESGeom_Point::Value () const
{
  return gp_Pnt(thePoint);
}

    gp_Pnt IGESGeom_Point::TransformedValue () const
{
  gp_XYZ Val = thePoint;
  if (HasTransf()) Location().Transforms(Val);
  gp_Pnt transVal(Val);
  return transVal;
}

    Standard_Boolean IGESGeom_Point::HasDisplaySymbol () const
{
  return (!theSymbol.IsNull());
}

    Handle(IGESBasic_SubfigureDef) IGESGeom_Point::DisplaySymbol () const
{
  return theSymbol;
}
