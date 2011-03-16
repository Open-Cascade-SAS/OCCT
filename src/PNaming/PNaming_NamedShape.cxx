// File:	PNaming_NamedShape.cxx
// Created:	Tue Apr 15 16:57:30 1997
// Author:	VAUTHIER Jean-Claude
//		<jcv@bigbox.paris1.matra-dtv.fr>


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
