// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_TopoDS.hxx>
#include <ShapePersistent_BRep.hxx>

#include <BRep_Builder.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>

#include <Standard_Assert.hxx>

enum
{
  ModifiedMask   = 2,
  CheckedMask    = 4,
  OrientableMask = 8,
  ClosedMask     = 16,
  InfiniteMask   = 32,
  ConvexMask     = 64
};

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_TopoDS::HShape::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myEntry;
  StdObject_Shape::read (theReadData);
}

void ShapePersistent_TopoDS::HShape::Write (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myEntry;
  StdObject_Shape::write (theWriteData);
}

void ShapePersistent_TopoDS::HShape::PChildren(SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myEntry);
  StdObject_Shape::PChildren(theChildren);
}

void ShapePersistent_TopoDS::pTBase::setFlags
  (const Handle(TopoDS_TShape)& theTShape) const
{
  theTShape->Free       (Standard_False); // Always frozen when coming from DB
  theTShape->Modified   ((myFlags & ModifiedMask)   != 0);
  theTShape->Checked    ((myFlags & CheckedMask)    != 0);
  theTShape->Orientable ((myFlags & OrientableMask) != 0);
  theTShape->Closed     ((myFlags & ClosedMask)     != 0);
  theTShape->Infinite   ((myFlags & InfiniteMask)   != 0);
  theTShape->Convex     ((myFlags & ConvexMask)     != 0);
}

static inline void AddShape
  (TopoDS_Shape& theParent, const Handle(StdObjMgt_Persistent)& theRef)
{
  Handle(ShapePersistent_TopoDS::HShape) aShape =
    Handle(ShapePersistent_TopoDS::HShape)::DownCast (theRef);

  if (aShape)
    BRep_Builder().Add (theParent, aShape->Import());
}

static inline void AddShape
  (TopoDS_Shape& theParent, const StdObject_Shape& theShape)
{
  BRep_Builder().Add (theParent, theShape.Import());
}

template <class ShapesArray>
void ShapePersistent_TopoDS::pTBase::addShapesT
  (TopoDS_Shape& theParent) const
{
  Handle(ShapesArray) aShapes = Handle(ShapesArray)::DownCast (myShapes);
  if (aShapes)
  {
    typename ShapesArray::Iterator anIter (*aShapes->Array());
    for (; anIter.More(); anIter.Next())
      AddShape (theParent, anIter.Value());
  }
}

template void ShapePersistent_TopoDS::pTBase::addShapesT
  <StdLPersistent_HArray1::Persistent> (TopoDS_Shape& theParent) const;

template void ShapePersistent_TopoDS::pTBase::addShapesT
  <StdPersistent_HArray1::Shape1> (TopoDS_Shape& theParent) const;

template <class Target>
Handle(TopoDS_TShape)
  ShapePersistent_TopoDS::pTSimple<Target>::createTShape() const
    { return new Target; }

template class ShapePersistent_TopoDS::pTSimple<TopoDS_TWire>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TShell>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TSolid>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TCompSolid>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TCompound>;

//=======================================================================
//function : Translate
//purpose  : Creates a persistent object from a shape
//=======================================================================
Handle(ShapePersistent_TopoDS::HShape)
ShapePersistent_TopoDS::Translate (const TopoDS_Shape& theShape,
                                   StdObjMgt_TransientPersistentMap& theMap,
                                   ShapePersistent_TriangleMode theTriangleMode)
{
  Handle(HShape) pHShape;

  if (theShape.IsNull())
    return pHShape;

  pHShape = new HShape;

  if (theMap.IsBound(theShape.TShape()))
  {
    // found in the registered
    Handle(StdPersistent_TopoDS::TShape) aPShape =
      Handle(StdPersistent_TopoDS::TShape)::DownCast(theMap.Find(theShape.TShape()));
    pHShape->myTShape = aPShape;
  }
  else
  {
    pTShape* aPTShape = 0;
    switch (theShape.ShapeType())
    {
    case TopAbs_VERTEX: {
      Handle(ShapePersistent_BRep::TVertex) aPVertex = new ShapePersistent_BRep::TVertex;
      pHShape->myTShape = aPVertex;
      aPVertex->myPersistent = ShapePersistent_BRep::Translate(TopoDS::Vertex(theShape), theMap);
      aPTShape = aPVertex->myPersistent.get();
    } break;
    case TopAbs_EDGE: {
      Handle(ShapePersistent_BRep::TEdge) aPEdge = new ShapePersistent_BRep::TEdge;
      pHShape->myTShape = aPEdge;
      aPEdge->myPersistent = ShapePersistent_BRep::Translate(TopoDS::Edge(theShape), theMap, theTriangleMode);
      aPTShape = aPEdge->myPersistent.get();
    } break;
    case TopAbs_FACE: {
      Handle(ShapePersistent_BRep::TFace) aPFace = new ShapePersistent_BRep::TFace;
      pHShape->myTShape = aPFace;
      aPFace->myPersistent = ShapePersistent_BRep::Translate(TopoDS::Face(theShape), theMap, theTriangleMode);
      aPTShape = aPFace->myPersistent.get();
    } break;
    case TopAbs_WIRE: {
      Handle(TWire) aPWire = new TWire;
      pHShape->myTShape = aPWire;
      aPWire->myPersistent = new TWire::pTObjectT;
      aPTShape = aPWire->myPersistent.get();
    } break;
    case TopAbs_SHELL: {
      Handle(TShell) aPShell = new TShell;
      pHShape->myTShape = aPShell;
      aPShell->myPersistent = new TShell::pTObjectT;
      aPTShape = aPShell->myPersistent.get();
    } break;
    case TopAbs_SOLID: {
      Handle(TSolid) aPSolid = new TSolid;
      pHShape->myTShape = aPSolid;
      aPSolid->myPersistent = new TSolid::pTObjectT;
      aPTShape = aPSolid->myPersistent.get();
    } break;
    case TopAbs_COMPSOLID: {
      Handle(TCompSolid) aPCompSolid = new TCompSolid;
      pHShape->myTShape = aPCompSolid;
      aPCompSolid->myPersistent = new TCompSolid::pTObjectT;
      aPTShape = aPCompSolid->myPersistent.get();
    } break;
    case TopAbs_COMPOUND: {
      Handle(TCompound) aPComp = new TCompound;
      pHShape->myTShape = aPComp;
      aPComp->myPersistent = new TCompound::pTObjectT;
      aPTShape = aPComp->myPersistent.get();
    } break;
    
    case TopAbs_SHAPE:
    default:
      Standard_ASSERT_INVOKE ("Unsupported shape type");
      break;
    }

    // Register in the persistent map
    theMap.Bind(theShape.TShape(), pHShape->myTShape);

    // Shape flags
    Standard_Integer aFlags = 0;
    if (theShape.Modified())   aFlags |= ModifiedMask;
    if (theShape.Checked())    aFlags |= CheckedMask;
    if (theShape.Orientable()) aFlags |= OrientableMask;
    if (theShape.Closed())     aFlags |= ClosedMask;
    if (theShape.Infinite())   aFlags |= InfiniteMask;
    if (theShape.Convex())     aFlags |= ConvexMask;
    aPTShape->myFlags = aFlags;

    // Copy current Shape
    TopoDS_Shape S = theShape;
    S.Orientation(TopAbs_FORWARD);
    S.Location(TopLoc_Location());
    // Count the number of <sub-shape> of the Shape's TShape
    Standard_Integer nbElem = 0;
    TopoDS_Iterator anItCount(S);
    while (anItCount.More()) {
      ++nbElem;
      anItCount.Next();
    }

    if (nbElem > 0)
    {
      Handle(StdLPersistent_HArray1OfPersistent) aShapes =
        new StdLPersistent_HArray1OfPersistent(1, nbElem);
      // translate <sub-shapes>
      TopoDS_Iterator anItTrans(S);
      for (Standard_Integer i = 1; anItTrans.More(); anItTrans.Next(), ++i) {
        aShapes->SetValue(i, Translate(anItTrans.Value(), theMap, theTriangleMode));
      }
      aPTShape->myShapes = StdLPersistent_HArray1::Translate<StdLPersistent_HArray1OfPersistent>
        ("PTopoDS_HArray1OfHShape", aShapes->Array1());
    }
  }

  pHShape->myOrient = theShape.Orientation();
  pHShape->myLocation = StdObject_Location::Translate(theShape.Location(), theMap);

  return pHShape;
}
