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

#include <IGESGeom_Flash.ixx>
#include <Standard_OutOfRange.hxx>
#include <gp_GTrsf.hxx>


IGESGeom_Flash::IGESGeom_Flash ()     {  }


    void IGESGeom_Flash::Init
  (const gp_XY&                       aPoint,
   const Standard_Real                aDim1,
   const Standard_Real                aDim2,
   const Standard_Real                aRotation,
   const Handle(IGESData_IGESEntity)& aReference)
{
  thePoint     = aPoint;
  theDim1      = aDim1;
  theDim2      = aDim2;
  theRotation  = aRotation;
  theReference = aReference;
  InitTypeAndForm(125,FormNumber());
// FormNumber : 0-4, Shape of the Flash
}

    void  IGESGeom_Flash::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 4) Standard_OutOfRange::Raise
    ("IGESGeom_Flash : SetFormNumber");
  InitTypeAndForm(125,form);
}


    gp_Pnt2d IGESGeom_Flash::ReferencePoint () const
{
  return ( gp_Pnt2d(thePoint) );
}

    gp_Pnt IGESGeom_Flash::TransformedReferencePoint () const
{
  gp_XYZ Point(thePoint.X(), thePoint.Y(), 0.0);
  if (HasTransf()) Location().Transforms(Point);
  return gp_Pnt(Point);
}

    Standard_Real IGESGeom_Flash::Dimension1 () const
{
  return theDim1;
}

    Standard_Real IGESGeom_Flash::Dimension2 () const
{
  return theDim2;
}

    Standard_Real IGESGeom_Flash::Rotation () const
{
  return theRotation;
}

    Standard_Boolean IGESGeom_Flash::HasReferenceEntity () const
{
  return (! theReference.IsNull() );
}

    Handle(IGESData_IGESEntity) IGESGeom_Flash::ReferenceEntity () const
{
  return theReference;
}
