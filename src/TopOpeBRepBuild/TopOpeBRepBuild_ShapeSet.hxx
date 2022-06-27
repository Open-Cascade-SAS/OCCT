// Created on: 1993-06-16
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_ShapeSet_HeaderFile
#define _TopOpeBRepBuild_ShapeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <Standard_OStream.hxx>



//! Auxiliary class providing an exploration of a set
//! of shapes to build faces or solids.
//! To build faces  : shapes are wires, elements are edges.
//! To build solids : shapes are shells, elements are faces.
//! The ShapeSet stores a list of shapes, a list of elements
//! to start reconstructions, and a map to search neighbours.
//! The map stores the connection  between elements through
//! subshapes of type <SubShapeType> given in constructor.
//! <SubShapeType> is :
//! - TopAbs_VERTEX to connect edges
//! - TopAbs_EDGE to connect faces
//!
//! Signature needed by the BlockBuilder :
//! InitStartElements(me : in out)
//! MoreStartElements(me) returns Boolean;
//! NextStartElement(me : in out);
//! StartElement(me) returns Shape; ---C++: return const &
//! InitNeighbours(me : in out; S : Shape);
//! MoreNeighbours(me) returns Boolean;
//! NextNeighbour(me : in out);
//! Neighbour(me) returns Shape; ---C++: return const &
class TopOpeBRepBuild_ShapeSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a ShapeSet  in order to build shapes connected
  //! by <SubShapeType>  shapes.
  //! <checkshape>:check (or not) the shapes, startelements, elements added.
  Standard_EXPORT TopOpeBRepBuild_ShapeSet(const TopAbs_ShapeEnum SubShapeType, const Standard_Boolean checkshape = Standard_True);
  
  Standard_EXPORT virtual ~TopOpeBRepBuild_ShapeSet();
  
  //! Adds <S> to the list of shapes. (wires or shells).
  Standard_EXPORT virtual void AddShape (const TopoDS_Shape& S);
  
  //! (S is a face or edge)
  //! Add S to the list of starting shapes used for reconstructions.
  //! apply AddElement(S).
  Standard_EXPORT virtual void AddStartElement (const TopoDS_Shape& S);
  
  //! for each subshape SE of S of type mySubShapeType
  //! - Add subshapes of S to the map of subshapes (mySubShapeMap)
  //! - Add S to the list of shape incident to subshapes of S.
  Standard_EXPORT virtual void AddElement (const TopoDS_Shape& S);
  
  //! return a reference on myStartShapes
  Standard_EXPORT const TopTools_ListOfShape& StartElements() const;
  
  Standard_EXPORT void InitShapes();
  
  Standard_EXPORT Standard_Boolean MoreShapes() const;
  
  Standard_EXPORT void NextShape();
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT void InitStartElements();
  
  Standard_EXPORT Standard_Boolean MoreStartElements() const;
  
  Standard_EXPORT void NextStartElement();
  
  Standard_EXPORT const TopoDS_Shape& StartElement() const;
  
  Standard_EXPORT virtual void InitNeighbours (const TopoDS_Shape& S);
  
  Standard_EXPORT Standard_Boolean MoreNeighbours();
  
  Standard_EXPORT void NextNeighbour();
  
  Standard_EXPORT const TopoDS_Shape& Neighbour() const;
  
  Standard_EXPORT TopTools_ListOfShape& ChangeStartShapes();
  
  //! Build the list of neighbour shapes of myCurrentShape
  //! (neighbour shapes and myCurrentShapes are of type t)
  //! Initialize myIncidentShapesIter on neighbour shapes.
  Standard_EXPORT virtual void FindNeighbours();
  
  Standard_EXPORT virtual const TopTools_ListOfShape& MakeNeighboursList (const TopoDS_Shape& E, const TopoDS_Shape& V);
  
  Standard_EXPORT Standard_Integer MaxNumberSubShape (const TopoDS_Shape& Shape);
  
  Standard_EXPORT void CheckShape (const Standard_Boolean checkshape);
  
  Standard_EXPORT Standard_Boolean CheckShape() const;
  
  Standard_EXPORT Standard_Boolean CheckShape (const TopoDS_Shape& S, const Standard_Boolean checkgeom = Standard_False);
  
  Standard_EXPORT void DumpName (Standard_OStream& OS, const TCollection_AsciiString& str) const;
  
  Standard_EXPORT void DumpCheck (Standard_OStream& OS, const TCollection_AsciiString& str, const TopoDS_Shape& S, const Standard_Boolean chk) const;
  
  Standard_EXPORT virtual void DumpSS();
  
  Standard_EXPORT virtual void DumpBB();
  
  Standard_EXPORT void DEBName (const TCollection_AsciiString& N);
  
  Standard_EXPORT const TCollection_AsciiString& DEBName() const;
  
  Standard_EXPORT void DEBNumber (const Standard_Integer I);
  
  Standard_EXPORT Standard_Integer DEBNumber() const;
  
  Standard_EXPORT virtual TCollection_AsciiString SName (const TopoDS_Shape& S, const TCollection_AsciiString& sb = "", const TCollection_AsciiString& sa = "") const;
  
  Standard_EXPORT virtual TCollection_AsciiString SNameori (const TopoDS_Shape& S, const TCollection_AsciiString& sb = "", const TCollection_AsciiString& sa = "") const;
  
  Standard_EXPORT virtual TCollection_AsciiString SName (const TopTools_ListOfShape& S, const TCollection_AsciiString& sb = "", const TCollection_AsciiString& sa = "") const;
  
  Standard_EXPORT virtual TCollection_AsciiString SNameori (const TopTools_ListOfShape& S, const TCollection_AsciiString& sb = "", const TCollection_AsciiString& sa = "") const;




protected:

  
  Standard_EXPORT void ProcessAddShape (const TopoDS_Shape& S);
  
  Standard_EXPORT void ProcessAddStartElement (const TopoDS_Shape& S);
  
  Standard_EXPORT void ProcessAddElement (const TopoDS_Shape& S);


  TopAbs_ShapeEnum myShapeType;
  TopAbs_ShapeEnum mySubShapeType;
  TopOpeBRepTool_ShapeExplorer mySubShapeExplorer;
  TopTools_ListOfShape myStartShapes;
  TopTools_ListIteratorOfListOfShape myStartShapesIter;
  TopTools_IndexedDataMapOfShapeListOfShape mySubShapeMap;
  TopTools_ListIteratorOfListOfShape myIncidentShapesIter;
  TopTools_ListOfShape myShapes;
  TopTools_ListIteratorOfListOfShape myShapesIter;
  TopoDS_Shape myCurrentShape;
  TopTools_ListOfShape myCurrentShapeNeighbours;
  Standard_Integer myDEBNumber;
  TCollection_AsciiString myDEBName;
  TopTools_IndexedMapOfOrientedShape myOMSS;
  TopTools_IndexedMapOfOrientedShape myOMES;
  TopTools_IndexedMapOfOrientedShape myOMSH;
  Standard_Boolean myCheckShape;


private:





};







#endif // _TopOpeBRepBuild_ShapeSet_HeaderFile
