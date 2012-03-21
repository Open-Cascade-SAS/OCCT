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

#include <IGESGeom_SurfaceOfRevolution.ixx>


IGESGeom_SurfaceOfRevolution::IGESGeom_SurfaceOfRevolution ()    {  }


    void IGESGeom_SurfaceOfRevolution::Init
  (const Handle(IGESGeom_Line)& anAxis,
   const Handle(IGESData_IGESEntity)& aGeneratrix,
   const Standard_Real aStartAngle, const Standard_Real anEndAngle)
{
  theLine       = anAxis;
  theGeneratrix = aGeneratrix;
  theStartAngle = aStartAngle;
  theEndAngle   = anEndAngle;
  InitTypeAndForm(120,0);
}

    Handle(IGESGeom_Line) IGESGeom_SurfaceOfRevolution::AxisOfRevolution () const
{
  return theLine;
}

    Handle(IGESData_IGESEntity) IGESGeom_SurfaceOfRevolution::Generatrix () const
{
  return theGeneratrix;
}

    Standard_Real IGESGeom_SurfaceOfRevolution::StartAngle () const
{
  return theStartAngle;
}

    Standard_Real IGESGeom_SurfaceOfRevolution::EndAngle () const
{
  return theEndAngle;
}
