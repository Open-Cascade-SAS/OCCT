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

#include <ShapeUpgrade_FaceDivideArea.ixx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Precision.hxx> 
#include <ShapeUpgrade_SplitSurfaceArea.hxx> 
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>  
#include <TopoDS.hxx>
#include <ShapeExtend.hxx> 
#include <ShapeBuild_ReShape.hxx>
#include <BRep_Builder.hxx>

//=======================================================================
//function : ShapeUpgrade_FaceDivideArea
//purpose  : 
//=======================================================================

ShapeUpgrade_FaceDivideArea::ShapeUpgrade_FaceDivideArea()
{
  myMaxArea = Precision::Infinite();
  SetPrecision(1.e-5);
  SetSplitSurfaceTool (new ShapeUpgrade_SplitSurfaceArea);
}

//=======================================================================
//function : ShapeUpgrade_FaceDivideArea
//purpose  : 
//=======================================================================

ShapeUpgrade_FaceDivideArea::ShapeUpgrade_FaceDivideArea(const TopoDS_Face& F)
{
  myMaxArea = Precision::Infinite();
  SetPrecision(1.e-5);
  SetSplitSurfaceTool (new ShapeUpgrade_SplitSurfaceArea);
  Init(F);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeUpgrade_FaceDivideArea::Perform() 
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  GProp_GProps aGprop;
  
  BRepGProp::SurfaceProperties(myFace,aGprop,Precision());
  Standard_Real anArea = aGprop.Mass();
  if((anArea - myMaxArea) < Precision::Confusion())
    return Standard_False;
 
  Standard_Integer anbParts = RealToInt(ceil(anArea/myMaxArea));
  Handle(ShapeUpgrade_SplitSurfaceArea) aSurfTool= Handle(ShapeUpgrade_SplitSurfaceArea)::
    DownCast(GetSplitSurfaceTool ());
  if(aSurfTool.IsNull())
    return Standard_False;
  aSurfTool->NbParts() = anbParts;
  if(!ShapeUpgrade_FaceDivide::Perform())
    return Standard_False;
  
  TopoDS_Shape aResult = Result();
  if(aResult.ShapeType() == TopAbs_FACE)
    return Standard_False;
  Standard_Integer aStatus = myStatus;
  TopExp_Explorer aExpF(aResult,TopAbs_FACE);
  TopoDS_Shape aCopyRes = aResult.EmptyCopied();
  
  Standard_Boolean isModified = Standard_False;
  for( ; aExpF.More() ; aExpF.Next()) {
    TopoDS_Shape aSh = Context()->Apply(aExpF.Current());
    TopoDS_Face aFace = TopoDS::Face(aSh);
    Init(aFace);
    BRep_Builder aB;
    if(Perform()) {
      isModified = Standard_True;
      TopoDS_Shape aRes = Result();
      TopExp_Explorer aExpR(aRes,TopAbs_FACE);
      for( ; aExpR.More(); aExpR.Next())
        aB.Add(aCopyRes,aExpR.Current());
    }
    else
      aB.Add(aCopyRes,aFace);
  }
  if(isModified)
    Context()->Replace(aResult,aCopyRes);
  myStatus |= aStatus;  
  myResult = Context()->Apply ( aResult );
  return Status ( ShapeExtend_DONE ); 
}

