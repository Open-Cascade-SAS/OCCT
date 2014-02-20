// Created on: 1997-04-15
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PNaming_NamedShape.ixx>

//=======================================================================
//function : PNaming_NamedShape
//purpose  : 
//=======================================================================

PNaming_NamedShape::PNaming_NamedShape() :
    myShapeStatus(0),
	myVersion(0)
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
