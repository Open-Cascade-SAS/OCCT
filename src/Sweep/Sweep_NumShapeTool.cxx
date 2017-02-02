// Created on: 1993-06-02
// Created by: Laurent BOURESCHE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Sweep_NumShape.hxx>
#include <Sweep_NumShapeTool.hxx>

//=======================================================================
//function : Sweep_NumShapeTool
//purpose  : 
//=======================================================================
Sweep_NumShapeTool::Sweep_NumShapeTool(const Sweep_NumShape& aShape) :
       myNumShape(aShape)
{
}


//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer  Sweep_NumShapeTool::NbShapes()const 
{
  if (myNumShape.Type()==TopAbs_EDGE){
    if (myNumShape.Closed()) { return myNumShape.Index(); }
    else { return myNumShape.Index() + 1 ;}
  }
  else{
    return 1;
  }
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  Sweep_NumShapeTool::Index
  (const Sweep_NumShape& aShape)const 
{
  if (aShape.Type()==TopAbs_EDGE){
    return 1;
  }
  else{
    if (aShape.Closed()) { return 2; }
    else { return (aShape.Index() + 1); }
  }
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

Sweep_NumShape  Sweep_NumShapeTool::Shape
  (const Standard_Integer anIndex)const 
{
  if (anIndex == 1){
    return myNumShape;
  }
  else{
    return Sweep_NumShape((anIndex-1),TopAbs_VERTEX,
			  myNumShape.Closed(),Standard_False,
			  Standard_False);
  }
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum  Sweep_NumShapeTool::Type(const Sweep_NumShape& aShape)const 
{
  return aShape.Type();
}



//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  Sweep_NumShapeTool::Orientation
  (const Sweep_NumShape& aShape)const 
{
  return aShape.Orientation();
}


//=======================================================================
//function : HasFirstVertex
//purpose  : 
//=======================================================================

Standard_Boolean Sweep_NumShapeTool::HasFirstVertex()const
{
  if (myNumShape.Type()==TopAbs_EDGE)
    return !myNumShape.BegInfinite();
  return Standard_True;
}


//=======================================================================
//function : HasLastVertex
//purpose  : 
//=======================================================================

Standard_Boolean Sweep_NumShapeTool::HasLastVertex()const
{
  if (myNumShape.Type()==TopAbs_EDGE)
    return !myNumShape.EndInfinite();
  return Standard_True;
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

Sweep_NumShape Sweep_NumShapeTool::FirstVertex()const
{
  if (myNumShape.Type()==TopAbs_EDGE){
    if(HasFirstVertex()){
      return Sweep_NumShape(1,TopAbs_VERTEX,
			    myNumShape.Closed(),Standard_False,
			    Standard_False);
    }
    else throw Standard_ConstructionError("inifinite Shape");
  }
  return myNumShape;
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

Sweep_NumShape Sweep_NumShapeTool::LastVertex()const
{
  if (myNumShape.Type()==TopAbs_EDGE){
    if(HasLastVertex()){
      return Sweep_NumShape(NbShapes()-1,TopAbs_VERTEX,
			    myNumShape.Closed(),Standard_False,
			    Standard_False);
    }
    else throw Standard_ConstructionError("inifinite Shape");
  }
  return myNumShape;
}

