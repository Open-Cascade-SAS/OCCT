// Created on: 1995-07-17
// Created by: Jing-Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_ThruSections_HeaderFile
#define _BRepOffsetAPI_ThruSections_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_ThruSectionErrorStatus.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <GeomAbs_Shape.hxx>
#include <Approx_ParametrizationType.hxx>
#include <Standard_Integer.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <NCollection_Handle.hxx>

class TopoDS_Wire;
class TopoDS_Vertex;
class TopoDS_Shape;
class Geom_BSplineSurface;
class BRepFill_Generator;

//! Describes functions to build a loft. This is a shell or a
//! solid passing through a set of sections in a given
//! sequence. Usually sections are wires, but the first and
//! the last sections may be vertices (punctual sections).
class BRepOffsetAPI_ThruSections  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes an algorithm for building a shell or a solid
  //! passing through a set of sections, where:
  //! -          isSolid is set to true if the construction algorithm is
  //! required to build a solid or to false if it is required to build
  //! a shell (the default value),
  //! -          ruled is set to true if the faces generated between
  //! the edges of two consecutive wires are ruled surfaces or to
  //! false (the default value) if they are smoothed out by approximation,
  //! -          pres3d defines the precision criterion used by the
  //! approximation algorithm; the default value is 1.0e-6.
  //! Use AddWire and AddVertex to define the
  //! successive sections of the shell or solid to be built.
  Standard_EXPORT BRepOffsetAPI_ThruSections(const Standard_Boolean isSolid = Standard_False, const Standard_Boolean ruled = Standard_False, const Standard_Real pres3d = 1.0e-06);
  
  //! Initializes this algorithm for building a shell or a solid
  //! passing through a set of sections, where:
  //! - isSolid is set to true if this construction algorithm is
  //! required to build a solid or to false if it is required to
  //! build a shell. false is the default value;
  //! - ruled is set to true if the faces generated between the
  //! edges of two consecutive wires are ruled surfaces or
  //! to false (the default value) if they are smoothed out by approximation,
  //! - pres3d defines the precision criterion used by the
  //! approximation algorithm; the default value is 1.0e-6.
  //! Use AddWire and AddVertex to define the successive
  //! sections of the shell or solid to be built.
  Standard_EXPORT void Init (const Standard_Boolean isSolid = Standard_False, const Standard_Boolean ruled = Standard_False, const Standard_Real pres3d = 1.0e-06);
  
  //! Adds the wire wire to the set of
  //! sections through which the shell or solid is built.
  //! Use the Build function to construct the shape.
  Standard_EXPORT void AddWire (const TopoDS_Wire& wire);
  
  //! Adds the vertex Vertex (punctual section) to the set of sections
  //! through which the shell or solid is built. A vertex may be added to the
  //! set of sections only as first or last section. At least one wire
  //! must be added to the set of sections by the method AddWire.
  //! Use the Build function to construct the shape.
  Standard_EXPORT void AddVertex (const TopoDS_Vertex& aVertex);
  
  //! Sets/unsets the option to
  //! compute origin and orientation on wires to avoid twisted results
  //! and update wires to have same number of edges.
  Standard_EXPORT void CheckCompatibility (const Standard_Boolean check = Standard_True);
  
  //! Define the approximation algorithm
  Standard_EXPORT void SetSmoothing (const Standard_Boolean UseSmoothing);
  
  //! Define the type of parametrization   used in the approximation
  Standard_EXPORT void SetParType (const Approx_ParametrizationType ParType);
  
  //! Define the Continuity used in the approximation
  Standard_EXPORT void SetContinuity (const GeomAbs_Shape C);
  
  //! define the Weights  associed to the criterium used in
  //! the  optimization.
  //!
  //! if Wi <= 0
  Standard_EXPORT void SetCriteriumWeight (const Standard_Real W1, const Standard_Real W2, const Standard_Real W3);
  
  //! Define the maximal U degree of result surface
  Standard_EXPORT void SetMaxDegree (const Standard_Integer MaxDeg);
  
  //! returns the type of parametrization used in the approximation
  Standard_EXPORT Approx_ParametrizationType ParType() const;
  
  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;
  
  //! returns the maximal U degree of result surface
  Standard_EXPORT Standard_Integer MaxDegree() const;
  
  //! Define the approximation algorithm
  Standard_EXPORT Standard_Boolean UseSmoothing() const;
  
  //! returns the Weights associed  to the criterium used in
  //! the  optimization.
  Standard_EXPORT void CriteriumWeight (Standard_Real& W1, Standard_Real& W2, Standard_Real& W3) const;
  
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! Returns the TopoDS Shape of the bottom of the loft if solid
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;
  
  //! Returns the TopoDS Shape of the top of the loft if solid
  Standard_EXPORT const TopoDS_Shape& LastShape() const;
  
  //! if Ruled
  //! Returns the Face generated by each edge
  //! except the last wire
  //! if smoothed
  //! Returns the Face generated by each edge of the first wire
  Standard_EXPORT TopoDS_Shape GeneratedFace (const TopoDS_Shape& Edge) const;

  //! Sets the mutable input state.
  //! If true then the input profile can be modified inside
  //! the thrusection operation. Default value is true.
  Standard_EXPORT void SetMutableInput(const Standard_Boolean theIsMutableInput);
  
  //! Returns a list of new shapes generated from the shape
  //! S by the shell-generating algorithm.
  //! This function is redefined from BRepBuilderAPI_MakeShape::Generated.
  //! S can be an edge or a vertex of a given Profile (see methods AddWire and AddVertex).
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;

  //! Returns the list of original wires
  const TopTools_ListOfShape& Wires() const
  {
    return myInputWires;
  }
  //! Returns the current mutable input state
  Standard_EXPORT Standard_Boolean IsMutableInput() const;
  
  //! Returns the status of thrusection operation
  BRepFill_ThruSectionErrorStatus GetStatus() const
  {
    return myStatus;
  }

protected:





private:

  
  Standard_EXPORT void CreateRuled();
  
  Standard_EXPORT void CreateSmoothed();
  
  Standard_EXPORT Handle(Geom_BSplineSurface) TotalSurf (const TopTools_Array1OfShape& shapes,
                                                         const Standard_Integer NbSect,
                                                         const Standard_Integer NbEdges,
                                                         const Standard_Boolean w1Point,
                                                         const Standard_Boolean w2Point,
                                                         const Standard_Boolean vClosed) const;


  TopTools_ListOfShape myInputWires; //!< List of input wires
  TopTools_SequenceOfShape myWires;  //!< Working wires
  TopTools_DataMapOfShapeListOfInteger myEdgeNewIndices;
  TopTools_DataMapOfShapeInteger myVertexIndex;
  Standard_Integer myNbEdgesInSection;
  Standard_Boolean myIsSolid;
  Standard_Boolean myIsRuled;
  Standard_Boolean myWCheck;
  Standard_Real myPres3d;
  TopoDS_Face myFirst;
  TopoDS_Face myLast;
  Standard_Boolean myDegen1;
  Standard_Boolean myDegen2;
  TopTools_DataMapOfShapeShape myEdgeFace;
  GeomAbs_Shape myContinuity;
  Approx_ParametrizationType myParamType;
  Standard_Integer myDegMax;
  Standard_Real myCritWeights[3];
  Standard_Boolean myUseSmoothing;
  Standard_Boolean myMutableInput;
  NCollection_Handle<BRepFill_Generator> myBFGenerator;
  BRepFill_ThruSectionErrorStatus myStatus;

};







#endif // _BRepOffsetAPI_ThruSections_HeaderFile
