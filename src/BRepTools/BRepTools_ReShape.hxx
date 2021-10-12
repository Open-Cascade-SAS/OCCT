// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepTools_ReShape_HeaderFile
#define _BRepTools_ReShape_HeaderFile

#include <BRepTools_History.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <TopAbs_ShapeEnum.hxx>

class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class BRepTools_ReShape;
DEFINE_STANDARD_HANDLE(BRepTools_ReShape, Standard_Transient)

//! Rebuilds a Shape by making pre-defined substitutions on some
//! of its components
//!
//! In a first phase, it records requests to replace or remove
//! some individual shapes
//! For each shape, the last given request is recorded
//! Requests may be applied "Oriented" (i.e. only to an item with
//! the SAME orientation) or not (the orientation of replacing
//! shape is respectful of that of the original one)
//!
//! Then, these requests may be applied to any shape which may
//! contain one or more of these individual shapes
//!
//! Supports the 'BRepTools_History' history by method 'History'.
class BRepTools_ReShape : public Standard_Transient
{
public:
  //! Returns an empty Reshape
  Standard_EXPORT BRepTools_ReShape();
  
  //! Clears all substitutions requests
  Standard_EXPORT virtual void Clear();
  
  //! Sets a request to Remove a Shape whatever the orientation
  Standard_EXPORT virtual void Remove (const TopoDS_Shape& shape);
  
  //! Sets a request to Replace a Shape by a new one.
  virtual void Replace (const TopoDS_Shape& shape, const TopoDS_Shape& newshape)
  {
    replace (shape, newshape, TReplacementKind_Modify);
  }
  
  //! Merges the parts to the single product.
  //! The first part is replaced by the product.
  //! The other parts are removed.
  //! The history of the merged shapes is presented by equal ways.
  template<typename TCollection> void Merge(
    const TCollection& theParts, const TopoDS_Shape& theProduct)
  {
    typename TCollection::Iterator aPIt(theParts);

    if (aPIt.More())
    {
      replace(aPIt.Value(), theProduct, TReplacementKind_Merge_Main);

      aPIt.Next();
    }

    const TReplacementKind aKind = TReplacementKind_Merge_Ordinary;
    for (; aPIt.More(); aPIt.Next())
    {
      replace(aPIt.Value(), theProduct, aKind);
    }
  }

  //! Tells if a shape is recorded for Replace/Remove
  Standard_EXPORT virtual Standard_Boolean IsRecorded (const TopoDS_Shape& shape) const;
  
  //! Returns the new value for an individual shape
  //! If not recorded, returns the original shape itself
  //! If to be Removed, returns a Null Shape
  //! Else, returns the replacing item
  Standard_EXPORT virtual TopoDS_Shape Value (const TopoDS_Shape& shape) const;
  
  //! Returns a complete substitution status for a shape
  //! 0  : not recorded,   <newsh> = original <shape>
  //! < 0: to be removed,  <newsh> is NULL
  //! > 0: to be replaced, <newsh> is a new item
  //! If <last> is False, returns status and new shape recorded in
  //! the map directly for the shape, if True and status > 0 then
  //! recursively searches for the last status and new shape.
  Standard_EXPORT virtual Standard_Integer Status (const TopoDS_Shape& shape, TopoDS_Shape& newsh, const Standard_Boolean last = Standard_False);

  //! Applies the substitutions requests to a shape.
  //!
  //! theUntil gives the level of type until which requests are taken into account.
  //! For subshapes of the type <until> no rebuild and further exploring are done.
  //!
  //! NOTE: each subshape can be replaced by shape of the same type
  //! or by shape containing only shapes of that type
  //! (for example, TopoDS_Edge can be replaced by TopoDS_Edge,
  //! TopoDS_Wire or TopoDS_Compound containing TopoDS_Edges).
  //! If incompatible shape type is encountered, it is ignored and flag FAIL1 is set in Status.
  Standard_EXPORT virtual TopoDS_Shape Apply (const TopoDS_Shape& theShape,
                                              const TopAbs_ShapeEnum theUntil = TopAbs_SHAPE);

  //! Returns (modifiable) the flag which defines whether Location of shape take into account
  //! during replacing shapes.
  virtual Standard_Boolean& ModeConsiderLocation()
  {
    return myConsiderLocation;
  }

  //! Returns modified copy of vertex if original one is not recorded or returns modified original vertex otherwise.
  //@param theV - original vertex.
  //@param theTol - new tolerance of vertex, optional.
  Standard_EXPORT TopoDS_Vertex CopyVertex(const TopoDS_Vertex& theV,
                                           const Standard_Real theTol = -1.0);

  //! Returns modified copy of vertex if original one is not recorded or returns modified original vertex otherwise.
  //@param theV - original vertex.
  //@param theNewPos - new position for vertex copy.
  //@param theTol - new tolerance of vertex.
  Standard_EXPORT TopoDS_Vertex CopyVertex(const TopoDS_Vertex& theV,
                                           const gp_Pnt& theNewPos,
                                           const Standard_Real aTol);

  //! Checks if shape has been recorded by reshaper as a value
  //@param theShape is the given shape
  Standard_EXPORT Standard_Boolean IsNewShape(const TopoDS_Shape& theShape) const;

  //! Returns the history of the substituted shapes.
  Standard_EXPORT Handle(BRepTools_History) History() const;

  DEFINE_STANDARD_RTTIEXT(BRepTools_ReShape,Standard_Transient)

protected:
  //! The kinds of the replacements.
  enum TReplacementKind
  {
    TReplacementKind_Remove = 1,
    TReplacementKind_Modify = 2,
    TReplacementKind_Merge_Main = 4,
    TReplacementKind_Merge_Ordinary = 8
  };

  //! Replaces the first shape by the second one
  //! after the following reorientation.
  //!
  //! If the first shape has the reversed orientation
  //! then the both shapes are reversed.
  //! If the first shape has the internal or external orientation then: <br>
  //! - the second shape is oriented forward (reversed) if it's orientation
  //!   is equal (not equal) to the orientation of the first shape; <br>
  //! - the first shape is oriented forward.
  Standard_EXPORT virtual void replace (
    const TopoDS_Shape& shape,
    const TopoDS_Shape& newshape,
    const TReplacementKind theKind);

private:
  //! Returns 'true' if the kind of a replacement is an ordinary merging.
  static Standard_Boolean isOrdinaryMerged(const TReplacementKind theKind)
  {
    return (theKind == TReplacementKind_Merge_Ordinary);
  }

  //! A replacement of an initial shape.
  struct TReplacement
  {
  public:
    //! The default constructor.
    TReplacement() : myKind(TReplacementKind_Remove)
    {
    }

    //! The initializing constructor.
    TReplacement(
        const TopoDS_Shape& theResult, const TReplacementKind theKind) :
      myResult(theResult), myKind(theKind)
    {
    }

    //! Returns the result of the replacement.
    TopoDS_Shape Result() const
    {
      return (myKind != TReplacementKind_Merge_Ordinary) ?
        myResult : TopoDS_Shape();
    }

    //! Returns the result of the relation.
    const TopoDS_Shape& RelationResult() const
    {
      return myResult;
    }

    //! Returns the kind of the relation
    //! between an initial shape and the result of the replacement.
    BRepTools_History::TRelationType RelationKind() const
    {
      return (myKind == TReplacementKind_Remove) ?
        BRepTools_History::TRelationType_Removed :
        BRepTools_History::TRelationType_Modified;
    }

  private:
    TopoDS_Shape myResult; //!< The result of the replacement.
    TReplacementKind myKind; //!< The kind of the replacement.
  };

  typedef NCollection_DataMap<TopoDS_Shape, TReplacement,
    TopTools_ShapeMapHasher> TShapeToReplacement;

private:
  //! Maps each shape to its replacement.
  //! If a shape is not bound to the map then the shape is replaced by itself.
  TShapeToReplacement myShapeToReplacement;

protected:
  TopTools_MapOfShape myNewShapes;
  Standard_Integer myStatus;


private:


  Standard_Boolean myConsiderLocation;


};







#endif // _BRepTools_ReShape_HeaderFile
