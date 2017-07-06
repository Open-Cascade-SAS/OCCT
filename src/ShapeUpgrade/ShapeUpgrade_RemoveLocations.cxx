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


#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_RemoveLocations,Standard_Transient)

//#include <ShapeUpgrade_DataMapOfShapeListOfTransient.hxx>
//=======================================================================
//function : ShapeUpgrade_RemoveLocations
//purpose  : 
//=======================================================================
ShapeUpgrade_RemoveLocations::ShapeUpgrade_RemoveLocations()
{
  myLevelRemoving = TopAbs_SHAPE;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeUpgrade_RemoveLocations::Remove(const TopoDS_Shape& theShape) 
{
  TopoDS_Shape aShape = theShape;
  myShape = aShape;
  TopAbs_ShapeEnum shtype = theShape.ShapeType();
  Standard_Boolean isRemoveLoc = ((shtype != TopAbs_COMPOUND && myLevelRemoving == TopAbs_SHAPE) || 
                   ((Standard_Integer)myLevelRemoving <= ((Standard_Integer)shtype)));
  TopoDS_Shape S;
  Standard_Boolean isDone = MakeNewShape(theShape,S,myShape,isRemoveLoc);
  
  return isDone;

}

//=======================================================================
//function : RebuildShape
//purpose  : 
//=======================================================================
static Standard_Boolean RebuildShape(const TopoDS_Face& theFace, TopoDS_Face& theNewFace)
{
  BRep_Builder aB;
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace,aLoc);
  Standard_Boolean isRebuild = Standard_False;
  if(!aLoc.IsIdentity()) {
    Handle(Geom_Surface) anewSurf =Handle(Geom_Surface)::DownCast( aSurf->Transformed(aLoc.Transformation()));
    aB.MakeFace(theNewFace,anewSurf,BRep_Tool::Tolerance(theFace));
    isRebuild = Standard_True;
  }
  return isRebuild;
}
//=======================================================================
//function : RebuildShape
//purpose  : 
//=======================================================================
static Standard_Boolean RebuildShape(const TopoDS_Edge& theEdge, TopoDS_Edge& theNewEdge,
                                     const TopoDS_Face& theFace, TopoDS_Face& theNewFace,
                                     Standard_Boolean isBound)
{
  Standard_Boolean isRebuild = Standard_False;
  BRep_Builder aB;
  if(!isBound) {
    Handle(Geom_Curve) C3d;
    TopLoc_Location aLoc;
    Standard_Real First3d,Last3d;
    C3d = BRep_Tool::Curve( theEdge,aLoc,First3d,Last3d);
    aB.MakeEdge(theNewEdge);
    if(!C3d.IsNull()) {
       if(!aLoc.IsIdentity()) {
         Handle(Geom_Curve) anewC3d = Handle(Geom_Curve)::DownCast(C3d->Transformed(aLoc.Transformation()));
         C3d = anewC3d;
       }
       
       aB.UpdateEdge(theNewEdge,C3d,BRep_Tool::Tolerance(theEdge));
       aB.Range(theNewEdge,First3d,Last3d);
     }
    theNewEdge.Orientation(theEdge.Orientation());
    if(BRep_Tool::Degenerated(theEdge))
      aB.Degenerated(theNewEdge,Standard_True);
    isRebuild = Standard_True;
  }
  if(!theFace.IsNull()) {
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace);
    if(!aSurf->IsKind(STANDARD_TYPE(Geom_Plane))) {
      Handle(Geom2d_Curve) c2d,c2d1;
      Standard_Real First2d,Last2d;
      
      c2d= BRep_Tool::CurveOnSurface( theEdge,theFace,First2d,Last2d);
      if(BRep_Tool::IsClosed(theEdge,theFace)) {
        if(!BRep_Tool::IsClosed(theNewEdge,theNewFace)) {
          TopoDS_Edge tmpE = TopoDS::Edge(theEdge.Reversed());
          c2d1= BRep_Tool::CurveOnSurface(tmpE,theFace,First2d,Last2d);
          TopAbs_Orientation OrEdge = theNewEdge.Orientation();
          
          if(theFace.Orientation() == TopAbs_REVERSED)
            OrEdge = ( OrEdge == TopAbs_FORWARD ? TopAbs_REVERSED : TopAbs_FORWARD);
          
          if(OrEdge == TopAbs_FORWARD)
            aB.UpdateEdge(theNewEdge,c2d, c2d1,theNewFace,0);
          else aB.UpdateEdge(theNewEdge,c2d1, c2d,theNewFace,0);
          
        }
      }
      else 
        aB.UpdateEdge(theNewEdge,c2d,theNewFace,0);
      
      if(!c2d.IsNull() || !c2d1.IsNull())
        aB.Range(theNewEdge,theNewFace,First2d,Last2d);
    }
  }
  return isRebuild;
}
//=======================================================================
//function :  RebuildShape
//purpose  : 
//=======================================================================
static Standard_Boolean RebuildShape(const TopoDS_Vertex& theVertex, TopoDS_Vertex& theNewVertex)
{
  BRep_Builder aB;
  aB.MakeVertex(theNewVertex);
  theNewVertex.Orientation( theVertex.Orientation());
  gp_Pnt p1 = BRep_Tool::Pnt( theVertex);
  aB.UpdateVertex(theNewVertex,p1,BRep_Tool::Tolerance( theVertex));
  return Standard_True;
}
//=======================================================================
//function : MakeNewShape
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_RemoveLocations::MakeNewShape(const TopoDS_Shape& theShape,
                                     const TopoDS_Shape& theAncShape,
                                     TopoDS_Shape& theNewShape,                     
                                     const Standard_Boolean theRemoveLoc)
{
  Standard_Boolean isDone = Standard_False;
  TopoDS_Shape aNewShape;
  TopAbs_ShapeEnum shtype = theShape.ShapeType();
  BRep_Builder aB;
  TopoDS_Shape aShape = theShape;
  if(!theRemoveLoc && !theShape.Location().IsIdentity()) {
    TopLoc_Location nulLoc;
    aShape.Location(nulLoc);
  }
  Standard_Boolean isBound = myMapNewShapes.IsBound(aShape);
  if(isBound) {
     aNewShape= myMapNewShapes.Find(aShape);
     aNewShape.Orientation(theShape.Orientation());
     if(!theRemoveLoc && !theShape.Location().IsIdentity()) {
       TopLoc_Location aL = theShape.Location();
       aNewShape.Location(aL);
     }
     if(shtype != TopAbs_EDGE) {
       theNewShape = aNewShape;
       return Standard_True;
     }
  }
  
  
  Standard_Boolean isRemoveLoc = theRemoveLoc;
  if(!theRemoveLoc) {
    isRemoveLoc = ((shtype != TopAbs_COMPOUND && myLevelRemoving == TopAbs_SHAPE) || 
                   ((Standard_Integer)myLevelRemoving <= ((Standard_Integer)shtype)));
  }
    
  Standard_Boolean aRebuild = Standard_False;
  TopoDS_Shape anAncShape = theAncShape;
  if(shtype == TopAbs_FACE) 
    anAncShape = aShape;
  if(isRemoveLoc && (!aShape.Location().IsIdentity() || shtype == TopAbs_EDGE || shtype == TopAbs_FACE )) {
    
    //Rebuild geometry for shape with location.
    if(shtype == TopAbs_FACE) {
      TopoDS_Face anewFace;
      TopoDS_Face oldFace = TopoDS::Face(aShape);
      aRebuild = RebuildShape(oldFace,anewFace);
      if(aRebuild) {
        aNewShape = anewFace;
        myMapNewShapes.Bind(oldFace,aNewShape);
      }
      
    }
    else if(shtype == TopAbs_EDGE) {
      TopoDS_Edge oldEdge = TopoDS::Edge(aShape);
      TopoDS_Edge anewEdge;
      TopoDS_Face F,newFace;
      
      if(!anAncShape.IsNull()) {
        F = TopoDS::Face(anAncShape);
        newFace = F;
        if(myMapNewShapes.IsBound(F))
          newFace = TopoDS::Face(myMapNewShapes.Find(F));
      }
      if(isBound)
        anewEdge = TopoDS::Edge(aNewShape);
      aRebuild = RebuildShape(oldEdge,anewEdge,F,newFace,isBound);
      aNewShape = anewEdge;
        
    }
    else if(shtype == TopAbs_VERTEX) {
      TopoDS_Vertex aVnew;
      TopoDS_Vertex aV = TopoDS::Vertex(aShape);
      aRebuild = RebuildShape(aV,aVnew);
      if(aRebuild)
        aNewShape = aVnew;
    }
  }
  isDone = aRebuild;
  
  //Removing location from sub-shapes in dependance of LevelRemoving and re-building shape.
  
  if(!isBound) {
    if(!aRebuild)
    {
      aNewShape = theShape.EmptyCopied();
      // it is safe to simply copy Closed flag since this operation does not change topology
      aNewShape.Closed (theShape.Closed());
    }
    TopLoc_Location oldLoc,nullloc;
    oldLoc = theShape.Location();
    if(!oldLoc.IsIdentity())
      aNewShape.Location(nullloc);
    TopAbs_Orientation orient = theShape.Orientation();
    aNewShape.Orientation(TopAbs_FORWARD);
    TopoDS_Iterator aIt(aShape,Standard_False,isRemoveLoc);
    for( ; aIt.More(); aIt.Next()) {
      TopoDS_Shape subshape = aIt.Value();
      TopoDS_Shape anewsubshape;
      Standard_Boolean isDoneSubShape = MakeNewShape(subshape,anAncShape,anewsubshape,isRemoveLoc);
      isDone = (isDone || isDoneSubShape);
      aB.Add(aNewShape,anewsubshape);
    }
    if(isDone) 
      aNewShape.Orientation(orient);
    else
      aNewShape = aShape;
    myMapNewShapes.Bind(aShape,aNewShape);
    if(!theRemoveLoc && !oldLoc.IsIdentity())
      aNewShape.Location(oldLoc);
    
  }
  theNewShape = aNewShape;

  return (isDone || isBound);
}

