// File:	Sweep_NumShapeTool.cxx
// Created:	Wed Jun  2 16:18:37 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <Sweep_NumShapeTool.ixx>
#include <Standard_ConstructionError.hxx>

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
    else Standard_ConstructionError::Raise("inifinite Shape");
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
    else Standard_ConstructionError::Raise("inifinite Shape");
  }
  return myNumShape;
}

