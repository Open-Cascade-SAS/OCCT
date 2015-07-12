// Created on: 1993-01-14
// Created by: Remi LEQUETTE
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

#ifndef _TopExp_Explorer_HeaderFile
#define _TopExp_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopExp_Stack.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class TopoDS_Shape;


//! An Explorer is a Tool to visit  a Topological Data
//! Structure form the TopoDS package.
//!
//! An Explorer is built with :
//!
//! * The Shape to explore.
//!
//! * The type of Shapes to find : e.g VERTEX, EDGE.
//! This type cannot be SHAPE.
//!
//! * The type of Shapes to avoid. e.g  SHELL, EDGE.
//! By default   this type is  SHAPE which  means no
//! restriction on the exploration.
//!
//! The Explorer  visits  all the  structure   to find
//! shapes of the   requested  type  which   are   not
//! contained in the type to avoid.
//!
//! Example to find all the Faces in the Shape S :
//!
//! TopExp_Explorer Ex;
//! for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
//! ProcessFace(Ex.Current());
//! }
//!
//! // an other way
//! TopExp_Explorer Ex(S,TopAbs_FACE);
//! while (Ex.More()) {
//! ProcessFace(Ex.Current());
//! Ex.Next();
//! }
//!
//! To find all the vertices which are not in an edge :
//!
//! for (Ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ...)
//!
//! To  find all the faces  in   a SHELL, then all the
//! faces not in a SHELL :
//!
//! TopExp_Explorer Ex1, Ex2;
//!
//! for (Ex1.Init(S,TopAbs_SHELL),...) {
//! // visit all shells
//! for (Ex2.Init(Ex1.Current(),TopAbs_FACE),...) {
//! // visit all the faces of the current shell
//! }
//! }
//!
//! for (Ex1.Init(S,TopAbs_FACE,TopAbs_SHELL),...) {
//! // visit all faces not in a shell
//! }
//!
//! If   the type  to avoid  is   the same  or is less
//! complex than the type to find it has no effect.
//!
//! For example searching edges  not in a vertex  does
//! not make a difference.
class TopExp_Explorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty explorer, becomes usefull after Init.
  Standard_EXPORT TopExp_Explorer();
  
  //! Creates an Explorer on the Shape <S>.
  //!
  //! <ToFind> is the type of shapes to search.
  //! TopAbs_VERTEX, TopAbs_EDGE, ...
  //!
  //! <ToAvoid>   is the type   of shape to  skip in the
  //! exploration.   If   <ToAvoid>  is  equal  or  less
  //! complex than <ToFind> or if  <ToAVoid> is SHAPE it
  //! has no effect on the exploration.
  Standard_EXPORT TopExp_Explorer(const TopoDS_Shape& S, const TopAbs_ShapeEnum ToFind, const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);
  
  //! Resets this explorer on the shape S. It is initialized to
  //! search the shape S, for shapes of type ToFind, that
  //! are not part of a shape ToAvoid.
  //! If the shape ToAvoid is equal to TopAbs_SHAPE, or
  //! if it is the same as, or less complex than, the shape
  //! ToFind it has no effect on the search.
  Standard_EXPORT void Init (const TopoDS_Shape& S, const TopAbs_ShapeEnum ToFind, const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);
  
  //! Returns  True if  there are   more  shapes in  the
  //! exploration.
    Standard_Boolean More() const;
  
  //! Moves to the next Shape in the exploration.
  //! Exceptions
  //! Standard_NoMoreObject if there are no more shapes to explore.
  Standard_EXPORT void Next();
  
  //! Returns the current shape in the exploration.
  //! Exceptions
  //! Standard_NoSuchObject if this explorer has no more shapes to explore.
  Standard_EXPORT const TopoDS_Shape& Current() const;
  
  //! Reinitialize  the    exploration with the original
  //! arguments.
  Standard_EXPORT void ReInit();
  
  //! Returns the current depth of the exploration. 0 is
  //! the shape to explore itself.
    Standard_Integer Depth() const;
  
  //! Clears the content of the explorer. It will return
  //! False on More().
    void Clear();
  
  Standard_EXPORT void Destroy();
~TopExp_Explorer()
{
  Destroy();
}




protected:





private:



  TopExp_Stack myStack;
  Standard_Integer myTop;
  Standard_Integer mySizeOfStack;
  TopoDS_Shape myShape;
  Standard_Boolean hasMore;
  TopAbs_ShapeEnum toFind;
  TopAbs_ShapeEnum toAvoid;


};


#include <TopExp_Explorer.lxx>





#endif // _TopExp_Explorer_HeaderFile
