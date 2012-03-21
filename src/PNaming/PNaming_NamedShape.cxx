// Created on: 1997-04-15
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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



#include <PNaming_NamedShape.ixx>

//=======================================================================
//function : PNaming_NamedShape
//purpose  : 
//=======================================================================

PNaming_NamedShape::PNaming_NamedShape() 
:myShapeStatus(0),myVersion(0)
{
}

//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer PNaming_NamedShape::NbShapes() const
{ 
  if (myNewShapes.IsNull()) return 0;
  return myNewShapes->Length(); 
}


//=======================================================================
//function : OldShapes
//purpose  : 
//=======================================================================

void PNaming_NamedShape::OldShapes
(const Handle(PTopoDS_HArray1OfShape1)& theShapes) 
{ myOldShapes = theShapes; }


//=======================================================================
//function : OldShapes
//purpose  : 
//=======================================================================

Handle(PTopoDS_HArray1OfShape1) PNaming_NamedShape::OldShapes() const
{ return myOldShapes; }


//=======================================================================
//function : NewShapes
//purpose  : 
//=======================================================================

void PNaming_NamedShape::NewShapes
(const Handle(PTopoDS_HArray1OfShape1)& theShapes) 
{ myNewShapes = theShapes; }


//=======================================================================
//function : NewShapes
//purpose  : 
//=======================================================================

Handle(PTopoDS_HArray1OfShape1) PNaming_NamedShape::NewShapes() const
{ return myNewShapes; }


//=======================================================================
//function : ShapeStatus
//purpose  : 
//=======================================================================

void PNaming_NamedShape::ShapeStatus
(const Standard_Integer theShapeStatus) 
{ myShapeStatus = theShapeStatus; }


//=======================================================================
//function : ShapeStatus
//purpose  : 
//=======================================================================

Standard_Integer PNaming_NamedShape::ShapeStatus() const
{ return myShapeStatus; }

//=======================================================================
//function : Version
//purpose  : 
//=======================================================================

void PNaming_NamedShape::Version
(const Standard_Integer theVersion) 
{ myVersion = theVersion; }


//=======================================================================
//function : Version
//purpose  : 
//=======================================================================

Standard_Integer PNaming_NamedShape::Version() const
{ return myVersion; }
