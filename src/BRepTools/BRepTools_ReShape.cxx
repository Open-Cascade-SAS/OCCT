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

//    abv 28.04.99 S4137: ading method Apply for work on all types of shapes
//    sln 29.11.01 Bug24: correction iteration through map in method 'Status'
//    sln 29.11.01 Bug22: correction of methods Replace and Value for case when mode myConsiderLocation is on

#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_ReShape.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_ReShape,Standard_Transient)

namespace
{

//! Adds the shape to the map.
//! If the shape is a wire, shell or solid then
//! adds the sub-shapes of the shape instead.
//! Returns 'true' if the sub-shapes were added.
template<typename TMap>
void Add(TMap& theMap, const TopoDS_Shape& theShape)
{
  const TopAbs_ShapeEnum aType = theShape.ShapeType();
  if (aType != TopAbs_WIRE && aType != TopAbs_SHELL &&
    aType != TopAbs_COMPSOLID)
  {
    theMap.Add(theShape);
    return;
  }

  for (TopoDS_Iterator aIt(theShape); aIt.More(); aIt.Next())
  {
    theMap.Add(aIt.Value());
  }
}

}

//include <ShapeExtend.hxx>
//#include <BRepTools_Edge.hxx>
static void CopyRanges (const TopoDS_Shape& toedge, const TopoDS_Shape& fromedge,
			const Standard_Real alpha, const Standard_Real beta) 
{
  Handle(BRep_TEdge) aTEdgeFrom = Handle(BRep_TEdge)::DownCast(fromedge.TShape());
  Handle(BRep_TEdge) aTEdgeTo   = Handle(BRep_TEdge)::DownCast(toedge.TShape());
  BRep_ListOfCurveRepresentation& tolist = aTEdgeTo->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation fromitcr (aTEdgeFrom->ChangeCurves());
  for (; fromitcr.More(); fromitcr.Next()) {
    Handle(BRep_GCurve) fromGC = Handle(BRep_GCurve)::DownCast(fromitcr.Value());
    if ( fromGC.IsNull() ) continue;
    Standard_Boolean isC3d = fromGC->IsCurve3D();
    if(isC3d) {
      if(fromGC->Curve3D().IsNull()) continue; }
    else {
       if(fromGC->PCurve().IsNull()) continue; }
      
    if ( ! isC3d && ! fromGC->IsCurveOnSurface()) continue; // only 3d curves and pcurves are treated

    Handle(Geom_Surface) surface;
    TopLoc_Location L;
    if ( ! isC3d ) {
      surface = fromGC->Surface();
      L = fromGC->Location();
    } 

    Handle(BRep_GCurve) toGC;
    for (BRep_ListIteratorOfListOfCurveRepresentation toitcr (tolist); toitcr.More(); toitcr.Next()) {
      toGC = Handle(BRep_GCurve)::DownCast(toitcr.Value());
      if ( toGC.IsNull() ) continue;
      if ( isC3d ) {
	if ( ! toGC->IsCurve3D() ) continue;
      }
      else if ( ! toGC->IsCurveOnSurface() || 
	       surface != toGC->Surface() || L != toGC->Location() ) continue;
      Standard_Real first = fromGC->First();
      Standard_Real last = fromGC->Last();
      Standard_Real len = last - first;
      toGC->SetRange ( first+alpha*len, first+beta*len );
      break;
    }
  }
}


//=======================================================================
//function : BRepTools_ReShape
//purpose  : 
//=======================================================================

BRepTools_ReShape::BRepTools_ReShape()
{
  myConsiderLocation = Standard_False;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepTools_ReShape::Clear() 
{
  myShapeToReplacement.Clear();
  myNewShapes.Clear();
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepTools_ReShape::Remove (const TopoDS_Shape& shape)
{
  TopoDS_Shape nulshape;
  replace(shape, nulshape, TReplacementKind_Remove);
}

//=======================================================================
//function : replace
//purpose  : 
//=======================================================================

void BRepTools_ReShape::replace (const TopoDS_Shape& ashape,
                                 const TopoDS_Shape& anewshape,
                                 const TReplacementKind theKind)
{
  TopoDS_Shape shape = ashape;
  TopoDS_Shape newshape = anewshape;
  if ( shape.IsNull() || shape == newshape ) return;

  if (shape.Orientation() == TopAbs_REVERSED)
  {
    shape.Reverse();
    newshape.Reverse();
  }
  // protect against INTERNAL or EXTERNAL shape
  else if (shape.Orientation() == TopAbs_INTERNAL
    || shape.Orientation() == TopAbs_EXTERNAL)
  {
    newshape.Orientation((newshape.Orientation() == shape.Orientation()) ?
      TopAbs_FORWARD : TopAbs_REVERSED);
    shape.Orientation(TopAbs_FORWARD);
  }

  if (myConsiderLocation) {
    //sln 29.11.01 Bug22: Change location of 'newshape' in accordance with location of 'shape'
    newshape.Location(newshape.Location().Multiplied(shape.Location().Inverted()));
    TopLoc_Location nullLoc; 
    shape.Location ( nullLoc );
  }

#ifdef OCCT_DEBUG
  if ( IsRecorded ( shape ) && ((myConsiderLocation && ! Value ( shape ).IsPartner ( newshape )) ||
                                 (!myConsiderLocation && ! Value ( shape ).IsSame ( newshape )))) 
    cout << "Warning: BRepTools_ReShape::Replace: shape already recorded" << endl;
#endif

  myShapeToReplacement.Bind(shape, TReplacement(newshape, theKind));
  myNewShapes.Add (newshape);
}

//=======================================================================
//function : IsRecorded
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_ReShape::IsRecorded (const TopoDS_Shape& ashape) const
{
  TopoDS_Shape shape = ashape;
  if (myConsiderLocation) {
    TopLoc_Location nullLoc;
    shape.Location ( nullLoc );
  }
  if (shape.IsNull()) return Standard_False;
  return myShapeToReplacement.IsBound (shape);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

TopoDS_Shape BRepTools_ReShape::Value (const TopoDS_Shape& ashape) const
{
  TopoDS_Shape res;
  if (ashape.IsNull()) return res;
  TopoDS_Shape shape = ashape;
  if (myConsiderLocation) {
    TopLoc_Location nullLoc;
    shape.Location ( nullLoc );
  }
  
  Standard_Boolean fromMap = Standard_False;
  if (!myShapeToReplacement.IsBound(shape))
  {
    res = shape;
  }
  else
  {
    res = myShapeToReplacement(shape).Result();
    if (shape.Orientation() == TopAbs_REVERSED)
    {
      res.Reverse();
    }
    fromMap = Standard_True;
  }
  // for INTERNAL/EXTERNAL, since they are not fully supported, keep orientation
  if ( shape.Orientation() == TopAbs_INTERNAL ||
       shape.Orientation() == TopAbs_EXTERNAL ) 
    res.Orientation ( shape.Orientation() );

  if (myConsiderLocation) {
    //sln 29.11.01 Bug22: Recalculate location of resulting shape in accordance with
    //whether result is from map or not
    if(fromMap) res.Location(ashape.Location()*res.Location());
    else res.Location(ashape.Location());
  }

  return res;
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Integer BRepTools_ReShape::Status(const TopoDS_Shape& ashape,
					    TopoDS_Shape& newsh,
					    const Standard_Boolean last) 
{
  Standard_Integer res = 0;
  if (ashape.IsNull())  {  newsh.Nullify();  return res;  }

  TopoDS_Shape shape = ashape;
  TopLoc_Location aLocSh = shape.Location();
  if (myConsiderLocation) {
    TopLoc_Location nullLoc;
    shape.Location ( nullLoc );
  }

  if (!myShapeToReplacement.IsBound(shape))
  {
    newsh = shape;
    res = 0;
  }
  else
  {
    newsh = myShapeToReplacement(shape).Result();
    res = 1;
  }
  if (res > 0) {
    if (newsh.IsNull()) res = -1;
    else if (newsh.IsEqual (shape)) res = 0;
    else if ( last && ((myConsiderLocation && ! newsh.IsPartner (shape)) ||
                       (!myConsiderLocation && ! newsh.IsSame (shape)))) {
      //TopoDS_Shape newnewsh;
      //Standard_Integer newres = Status (newsh, newnewsh, last);
      //newsh = newnewsh;
      //if (newres) res = newres;
      // sln 29.11.01 Bug24: Correction iteration through maps. Way of iteration used early does not
      // correspond to way of storing information in the maps.
      newsh = Apply(shape, TopAbs_SHAPE);
      if (newsh.IsNull()) res = -1; 
      if (newsh.IsEqual (shape)) res = 0;
    }
  }
  if(myConsiderLocation && !newsh.IsNull()) 
  {
    TopLoc_Location aResLoc = (res >0 && !newsh.Location().IsIdentity() ? 
      aLocSh * newsh.Location() : aLocSh);
    newsh.Location(aResLoc);
  }
  return res;
}

//=======================================================================
//function : EncodeStatus
//purpose  : static
//=======================================================================
static Standard_Integer EncodeStatus (const Standard_Integer status)
{
  switch ( status ) {
  case 0   : return 0x0000; //ShapeExtend_OK
  case 1: return 0x0001;    //ShapeExtend_DONE1
  case 2: return 0x0002;    //....
  case 3: return 0x0004;
  case 4: return 0x0008;
  case 5: return 0x0010;
  case 6: return 0x0020;
  case 7: return 0x0040;
  case 8: return 0x0080;    //....
  case 9 : return 0x00ff;   //ShapeExtend_DONE
  case 10: return 0x0100;   //ShapeExtend_FAIL1
  case 11: return 0x0200;   //...
  case 12: return 0x0400;
  case 13: return 0x0800;
  case 14: return 0x1000;
  case 15: return 0x2000;
  case 16: return 0x4000;
  case 17: return 0x8000;   //....
  case 18 : return 0xff00;  //ShapeExtend_FAIL
  }
  return 0;
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

TopoDS_Shape BRepTools_ReShape::Apply (const TopoDS_Shape& shape,
                                       const TopAbs_ShapeEnum until) 
{
  myStatus = EncodeStatus(0); //ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  if ( shape.IsNull() ) return shape;

  // apply direct replacement
  TopoDS_Shape newsh = Value ( shape );
  
  // if shape removed, return NULL
  if ( newsh.IsNull() ) {
    myStatus = EncodeStatus (2); //ShapeExtend_DONE2
    return newsh;
  }
  
  // if shape replaced, apply modifications to the result recursively 
  if ( (myConsiderLocation && ! newsh.IsPartner (shape)) || 
      (!myConsiderLocation &&! newsh.IsSame ( shape )) ) {
    TopoDS_Shape res = Apply ( newsh, until );
    myStatus |= EncodeStatus(1); //ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    return res;
  }

  TopAbs_ShapeEnum st = shape.ShapeType(); //, subt;
  if ( st >= until ) return newsh;    // critere d arret
  if(st == TopAbs_VERTEX || st == TopAbs_SHAPE)
    return shape;
  // define allowed types of components
  //fix for SAMTECH bug OCC322 about abcense internal vertices after sewing. 
  /*
  switch ( st ) {
  case TopAbs_COMPOUND:  subt = TopAbs_SHAPE;  break;
  case TopAbs_COMPSOLID: subt = TopAbs_SOLID;  break;
  case TopAbs_SOLID:     subt = TopAbs_SHELL;  break;
  case TopAbs_SHELL:     subt = TopAbs_FACE;   break;
  case TopAbs_FACE:      subt = TopAbs_WIRE;   break;
  case TopAbs_WIRE:      subt = TopAbs_EDGE;   break;
  case TopAbs_EDGE:      subt = TopAbs_VERTEX; break;
  case TopAbs_VERTEX:
  case TopAbs_SHAPE:
  default:               return shape;
  }
  */
  BRep_Builder B;
  
  TopoDS_Shape result = shape.EmptyCopied();
  TopAbs_Orientation orien = shape.Orientation();
  result.Orientation(TopAbs_FORWARD); // protect against INTERNAL or EXTERNAL shapes
  Standard_Boolean modif = Standard_False;
  Standard_Integer locStatus = myStatus;
  
  // apply recorded modifications to subshapes
  Standard_Boolean isEmpty = Standard_True;
  for ( TopoDS_Iterator it(shape,Standard_False); it.More(); it.Next() ) {
    TopoDS_Shape sh = it.Value();
    newsh = Apply ( sh, until );
    if ( newsh != sh ) {
      if ( myStatus & EncodeStatus(4)) //ShapeExtend::DecodeStatus ( myStatus, ShapeExtend_DONE4 ) )
        locStatus |= EncodeStatus(4); //|= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
      modif = 1;
    }
    if ( newsh.IsNull() ) {
      locStatus |= EncodeStatus(4); //ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
      continue;
    }
    if ( isEmpty )
      isEmpty = Standard_False;
    locStatus |= EncodeStatus(3);//ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    if ( st == TopAbs_COMPOUND || newsh.ShapeType() == sh.ShapeType()) { //fix for SAMTECH bug OCC322 about abcense internal vertices after sewing.
      B.Add ( result, newsh );
      continue;
    }
    Standard_Integer nitems = 0;
    for ( TopoDS_Iterator subit(newsh); subit.More(); subit.Next(), nitems++ ) {
      TopoDS_Shape subsh = subit.Value();
      if ( subsh.ShapeType() == sh.ShapeType() ) B.Add ( result, subsh );//fix for SAMTECH bug OCC322 about abcense internal vertices after sewing.
      else locStatus |= EncodeStatus(10);//ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
    }
    if ( ! nitems ) locStatus |= EncodeStatus(10);//ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
  }
  if ( ! modif ) return shape;

  // For empty topological containers (any kind of shape except vertex, edge
  // and face) we have to produce an empty result
  if ( isEmpty && st != TopAbs_VERTEX && st != TopAbs_EDGE && st != TopAbs_FACE )
  {
    result = TopoDS_Shape();
  }
  else
  {
    // restore Range on edge broken by EmptyCopied()
    if ( st == TopAbs_EDGE ) {
      CopyRanges (result, shape, 0, 1);
    }
    else if (st == TopAbs_FACE)  {
      TopoDS_Face face = TopoDS::Face ( shape );
      if( BRep_Tool::NaturalRestriction( face ) ) {
        BRep_Builder aB;
        aB.NaturalRestriction( TopoDS::Face (  result ), Standard_True );
      }
    }
    else if (st == TopAbs_WIRE || st == TopAbs_SHELL)
      result.Closed (BRep_Tool::IsClosed (result));

    result.Orientation(orien);
  }

  replace(shape, result,
    result.IsNull() ? TReplacementKind_Remove : TReplacementKind_Modify);
  myStatus = locStatus;

  return result;
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

/*Standard_Boolean BRepTools_ReShape::Status (const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status );
}*/

//=======================================================================
//function : CopyVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex BRepTools_ReShape::CopyVertex(const TopoDS_Vertex& theV,
                                            const Standard_Real theTol)
{
  return CopyVertex(theV, BRep_Tool::Pnt(theV), theTol);
}

//=======================================================================
//function : CopyVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex BRepTools_ReShape::CopyVertex(const TopoDS_Vertex& theV,
                                            const gp_Pnt& theNewPos,
                                            const Standard_Real theTol)
{
  TopoDS_Vertex aVertexCopy;
  Standard_Boolean isRecorded = IsRecorded(theV);
  aVertexCopy = isRecorded ? TopoDS::Vertex(Apply(theV)) : TopoDS::Vertex(theV.EmptyCopied());

  BRep_Builder B;
  Standard_Real aNewTol = theTol > 0.0 ? theTol : BRep_Tool::Tolerance(theV);
  B.UpdateVertex(aVertexCopy, theNewPos, aNewTol);

  if (!isRecorded)
    Replace(theV, aVertexCopy);

  return aVertexCopy;
}

Standard_Boolean BRepTools_ReShape::IsNewShape(const TopoDS_Shape& theShape) const
{
  return myNewShapes.Contains(theShape);
}

//=======================================================================
//function : History
//purpose  :
//=======================================================================

Handle(BRepTools_History) BRepTools_ReShape::History() const
{
  Handle(BRepTools_History) aHistory = new BRepTools_History;

  // Fill the history.
  for (TShapeToReplacement::Iterator aRIt(myShapeToReplacement);
    aRIt.More(); aRIt.Next())
  {
    const TopoDS_Shape& aShape = aRIt.Key();
    if (!BRepTools_History::IsSupportedType(aShape) ||
      myNewShapes.Contains(aShape))
    {
      continue;
    }

    NCollection_IndexedMap<TopoDS_Shape> aIntermediates;
    NCollection_Map<TopoDS_Shape> aModified;
    aIntermediates.Add(aShape);
    for (Standard_Integer aI = 1; aI <= aIntermediates.Size(); ++aI)
    {
      const TopoDS_Shape& aIntermediate = aIntermediates(aI);
      const TReplacement* aReplacement =
        myShapeToReplacement.Seek(aIntermediate);
      if (aReplacement == NULL)
      {
        Add(aModified, aIntermediate);
      }
      else if (aReplacement->RelationKind() !=
        BRepTools_History::TRelationType_Removed)
      {
        const TopoDS_Shape aResult = aReplacement->RelationResult();
        if (!aResult.IsNull())
        {
          Add(aIntermediates, aResult);
        }
      }
    }

    if (aModified.IsEmpty())
    {
      aHistory->Remove(aShape);
    }
    else
    {
      for (NCollection_Map<TopoDS_Shape>::Iterator aIt(aModified);
        aIt.More(); aIt.Next())
      {
        aHistory->AddModified(aShape, aIt.Value());
      }
    }
  }

  return aHistory;
}
