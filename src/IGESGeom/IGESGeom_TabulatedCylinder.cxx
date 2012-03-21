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

#include <IGESGeom_TabulatedCylinder.ixx>
#include <gp_GTrsf.hxx>


IGESGeom_TabulatedCylinder::IGESGeom_TabulatedCylinder ()    {  }


    void IGESGeom_TabulatedCylinder::Init
  (const Handle(IGESData_IGESEntity)& aDirectrix,
   const gp_XYZ&                      anEnd)
{
  theDirectrix = aDirectrix;
  theEnd       = anEnd;
  InitTypeAndForm(122,0);
}

    Handle(IGESData_IGESEntity) IGESGeom_TabulatedCylinder::Directrix () const 
{
  return theDirectrix;
}

    gp_Pnt IGESGeom_TabulatedCylinder::EndPoint () const
{
  return ( gp_Pnt(theEnd) );
}

    gp_Pnt IGESGeom_TabulatedCylinder::TransformedEndPoint () const
{
  gp_XYZ EndPoint = theEnd;
  if (HasTransf()) Location().Transforms(EndPoint);
  return ( gp_Pnt(EndPoint) );
}
