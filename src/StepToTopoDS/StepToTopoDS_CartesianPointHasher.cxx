// Created on: 1993-08-30
// Created by: Martine LANGLOIS
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



#include <StepToTopoDS_CartesianPointHasher.ixx>
#include <StepGeom_CartesianPoint.hxx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer StepToTopoDS_CartesianPointHasher::HashCode
  (const Handle(StepGeom_CartesianPoint)& K, const Standard_Integer Upper)
{
  return ::HashCode(K,Upper);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_CartesianPointHasher::IsEqual
  (const Handle(StepGeom_CartesianPoint)& K1,
   const Handle(StepGeom_CartesianPoint)& K2)
{
  return (K1 == K2);
}
