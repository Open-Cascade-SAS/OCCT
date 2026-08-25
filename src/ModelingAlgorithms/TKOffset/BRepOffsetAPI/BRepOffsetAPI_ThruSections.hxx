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
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <GeomAbs_Shape.hxx>
#include <Approx_ParametrizationType.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>
#include <gp_Vec.hxx>

class TopoDS_Wire;
class TopoDS_Vertex;
class TopoDS_Shape;
class Geom_BSplineSurface;
class BRepFill_Generator;
class gp_Dir;

//! Describes functions to build a loft. This is a shell or a
//! solid passing through a set of sections in a given
//! sequence. Usually sections are wires, but the first and
//! the last sections may be vertices (punctual sections).
class BRepOffsetAPI_ThruSections : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Maps section edges to the support faces whose derivatives are to be matched.
  using EdgeFaceMap = NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>;

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
  Standard_EXPORT BRepOffsetAPI_ThruSections(const bool   isSolid = false,
                                             const bool   ruled   = false,
                                             const double pres3d  = 1.0e-06);

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
  Standard_EXPORT void Init(const bool   isSolid = false,
                            const bool   ruled   = false,
                            const double pres3d  = 1.0e-06);

  //! Adds the wire wire to the set of
  //! sections through which the shell or solid is built.
  //! Use the Build function to construct the shape.
  Standard_EXPORT void AddWire(const TopoDS_Wire& wire);

  //! Adds the vertex Vertex (punctual section) to the set of sections
  //! through which the shell or solid is built. A vertex may be added to the
  //! set of sections only as first or last section. At least one wire
  //! must be added to the set of sections by the method AddWire.
  //! Use the Build function to construct the shape.
  Standard_EXPORT void AddVertex(const TopoDS_Vertex& aVertex);

  //! Sets/unsets the option to
  //! compute origin and orientation on wires to avoid twisted results
  //! and update wires to have same number of edges.
  Standard_EXPORT void CheckCompatibility(const bool check = true);

  //! Endpoint constraints are supported by the non-ruled B-spline builder with
  //! smoothing disabled. Build() reports IncompatibleOptions if they are used
  //! with a ruled loft or the variational smoothing solver. The constrained
  //! endpoint must be a wire, and the first and last sections must be distinct.
  //!
  //! Constrains the derivative of the loft at the first section to be parallel
  //! to the specified direction. The constraint is geometric (G1): its final
  //! magnitude is selected by the approximation algorithm.
  Standard_EXPORT void SetFirstSectionTangent(const gp_Dir& theDirection);

  //! Constrains the geometric continuity at the first section in the specified
  //! direction. theContinuity must be GeomAbs_G1 or GeomAbs_G2. G2 uses the
  //! zero-curvature continuation defined by translating the section along the
  //! specified direction.
  Standard_EXPORT void SetFirstSectionTangent(const gp_Dir&       theDirection,
                                              const GeomAbs_Shape theContinuity);

  //! Constrains the derivative of the loft at the last section to be parallel
  //! to the specified direction.
  Standard_EXPORT void SetLastSectionTangent(const gp_Dir& theDirection);

  //! Constrains the geometric continuity at the last section in the specified
  //! direction. See SetFirstSectionTangent() for the G2 convention.
  Standard_EXPORT void SetLastSectionTangent(const gp_Dir&       theDirection,
                                             const GeomAbs_Shape theContinuity);

  //! Constrains the derivative at the first section to the normal of the plane
  //! containing that section. Build fails with InvalidBoundaryConstraint if no
  //! such plane can be found. A negative tolerance uses edge tolerances.
  Standard_EXPORT void SetFirstSectionNormal(const double theTolerance = -1.0);

  //! Constrains the first section to G1 or G2 continuity with the implicit
  //! surface obtained by translating the section along its plane normal.
  Standard_EXPORT void SetFirstSectionNormal(const GeomAbs_Shape theContinuity,
                                             const double        theTolerance = -1.0);

  //! Constrains the derivative at the last section to the normal of the plane
  //! containing that section.
  Standard_EXPORT void SetLastSectionNormal(const double theTolerance = -1.0);

  //! Constrains the last section to G1 or G2 continuity with the implicit
  //! surface obtained by translating the section along its plane normal.
  Standard_EXPORT void SetLastSectionNormal(const GeomAbs_Shape theContinuity,
                                            const double        theTolerance = -1.0);

  //! Constrains the derivative at the first section to lie in the tangent
  //! plane of the support face along that section. Each section edge must lie
  //! on the support face; if no pcurve is available, it is projected onto the
  //! face. The angular tolerance is in radians and must be greater than zero
  //! and less than pi/2.
  Standard_EXPORT void SetFirstSectionSupport(const TopoDS_Face& theSupport,
                                              const double       theAngularTolerance = 0.01);

  //! Constrains the first section to G1 or G2 continuity with the support face.
  //! G2 follows both the tangent plane and normal curvature of the support
  //! surface along the interiors of the connecting edges. The angular and
  //! curvature tolerances are acceptance thresholds checked by adaptive
  //! sampling; theCurvatureTolerance is expressed in inverse model units and
  //! must be greater than zero for G2. It is ignored for G1.
  Standard_EXPORT void SetFirstSectionSupport(const TopoDS_Face&  theSupport,
                                              const GeomAbs_Shape theContinuity,
                                              const double        theAngularTolerance   = 0.01,
                                              const double        theCurvatureTolerance = 0.1);

  //! Constrains the first section to G1 continuity with an individual support
  //! face for each section edge. Each key must be an edge of the first input
  //! section and each value must be a face containing that edge, or a face whose
  //! geometry coincides with it within the edge and face tolerances. Every edge
  //! of the section must be mapped. Edge splits produced by CheckCompatibility()
  //! are handled automatically.
  Standard_EXPORT void SetFirstSectionSupports(const EdgeFaceMap& theSupports,
                                               const double       theAngularTolerance = 0.01);

  //! Constrains the first section to G1 or G2 continuity with the support face
  //! mapped to each section edge. See SetFirstSectionSupport() for the tolerance
  //! definitions. At a vertex shared by non-tangent support faces there is no
  //! unique G1 or G2 value. The faces are checked one-sided along their edge
  //! interiors, and a bisecting derivative is used to close the loft profile.
  Standard_EXPORT void SetFirstSectionSupports(const EdgeFaceMap&  theSupports,
                                               const GeomAbs_Shape theContinuity,
                                               const double        theAngularTolerance   = 0.01,
                                               const double        theCurvatureTolerance = 0.1);

  //! Constrains the derivative at the last section to lie in the tangent
  //! plane of the support face along that section.
  Standard_EXPORT void SetLastSectionSupport(const TopoDS_Face& theSupport,
                                             const double       theAngularTolerance = 0.01);

  //! Constrains the last section to G1 or G2 continuity with the support face.
  Standard_EXPORT void SetLastSectionSupport(const TopoDS_Face&  theSupport,
                                             const GeomAbs_Shape theContinuity,
                                             const double        theAngularTolerance   = 0.01,
                                             const double        theCurvatureTolerance = 0.1);

  //! Constrains the last section to G1 continuity with an individual support
  //! face for each section edge. See SetFirstSectionSupports().
  Standard_EXPORT void SetLastSectionSupports(const EdgeFaceMap& theSupports,
                                              const double       theAngularTolerance = 0.01);

  //! Constrains the last section to G1 or G2 continuity with the support face
  //! mapped to each section edge.
  Standard_EXPORT void SetLastSectionSupports(const EdgeFaceMap&  theSupports,
                                              const GeomAbs_Shape theContinuity,
                                              const double        theAngularTolerance   = 0.01,
                                              const double        theCurvatureTolerance = 0.1);

  //! Removes the derivative constraint at the first section.
  Standard_EXPORT void ClearFirstSectionConstraint();

  //! Removes the derivative constraint at the last section.
  Standard_EXPORT void ClearLastSectionConstraint();

  //! Returns true if a derivative constraint is defined at the first section.
  bool HasFirstSectionConstraint() const { return myHasFirstTangent; }

  //! Returns true if a derivative constraint is defined at the last section.
  bool HasLastSectionConstraint() const { return myHasLastTangent; }

  //! Returns the requested geometric continuity at the first section.
  GeomAbs_Shape FirstSectionContinuity() const { return myFirstBoundaryContinuity; }

  //! Returns the requested geometric continuity at the last section.
  GeomAbs_Shape LastSectionContinuity() const { return myLastBoundaryContinuity; }

  //! Define the approximation algorithm
  Standard_EXPORT void SetSmoothing(const bool UseSmoothing);

  //! Define the type of parametrization used in the approximation
  Standard_EXPORT void SetParType(const Approx_ParametrizationType ParType);

  //! Define the Continuity used in the approximation
  Standard_EXPORT void SetContinuity(const GeomAbs_Shape C);

  //! define the Weights associed to the criterium used in
  //! the optimization.
  //!
  //! if Wi <= 0
  Standard_EXPORT void SetCriteriumWeight(const double W1, const double W2, const double W3);

  //! Define the maximal U degree of result surface
  Standard_EXPORT void SetMaxDegree(const int MaxDeg);

  //! returns the type of parametrization used in the approximation
  Standard_EXPORT Approx_ParametrizationType ParType() const;

  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;

  //! returns the maximal U degree of result surface
  Standard_EXPORT int MaxDegree() const;

  //! Define the approximation algorithm
  Standard_EXPORT bool UseSmoothing() const;

  //! returns the Weights associed to the criterium used in
  //! the optimization.
  Standard_EXPORT void CriteriumWeight(double& W1, double& W2, double& W3) const;

  Standard_EXPORT void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

  //! Returns the TopoDS Shape of the bottom of the loft if solid
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;

  //! Returns the TopoDS Shape of the top of the loft if solid
  Standard_EXPORT const TopoDS_Shape& LastShape() const;

  //! if Ruled
  //! Returns the Face generated by each edge
  //! except the last wire
  //! if smoothed
  //! Returns the Face generated by each edge of the first wire
  Standard_EXPORT TopoDS_Shape GeneratedFace(const TopoDS_Shape& Edge) const;

  //! Sets the mutable input state.
  //! If true then the input profile can be modified inside
  //! the thrusection operation. Default value is true.
  Standard_EXPORT void SetMutableInput(const bool theIsMutableInput);

  //! Returns a list of new shapes generated from the shape
  //! S by the shell-generating algorithm.
  //! This function is redefined from BRepBuilderAPI_MakeShape::Generated.
  //! S can be an edge or a vertex of a given Profile (see methods AddWire and AddVertex).
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(const TopoDS_Shape& S) override;

  //! Returns the list of original wires
  const NCollection_List<TopoDS_Shape>& Wires() const { return myInputWires; }

  //! Returns the current mutable input state
  Standard_EXPORT bool IsMutableInput() const;

  //! Returns the status of thrusection operation
  BRepFill_ThruSectionErrorStatus GetStatus() const { return myStatus; }

private:
  Standard_EXPORT void CreateRuled();

  Standard_EXPORT void CreateSmoothed();

  Standard_EXPORT occ::handle<Geom_BSplineSurface> TotalSurf(
    const NCollection_Array1<TopoDS_Shape>& shapes,
    const int                               NbSect,
    const int                               NbEdges,
    const bool                              w1Point,
    const bool                              w2Point,
    const bool                              vClosed);

  NCollection_List<TopoDS_Shape>     myInputWires; //!< List of input wires
  NCollection_Sequence<TopoDS_Shape> myWires;      //!< Working wires
  NCollection_DataMap<TopoDS_Shape, NCollection_List<int>, TopTools_ShapeMapHasher>
                                                                           myEdgeNewIndices;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>          myVertexIndex;
  int                                                                      myNbEdgesInSection;
  bool                                                                     myIsSolid;
  bool                                                                     myIsRuled;
  bool                                                                     myWCheck;
  double                                                                   myPres3d;
  TopoDS_Face                                                              myFirst;
  TopoDS_Face                                                              myLast;
  bool                                                                     myDegen1;
  bool                                                                     myDegen2;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myEdgeFace;
  GeomAbs_Shape                                                            myContinuity;
  Approx_ParametrizationType                                               myParamType;
  int                                                                      myDegMax;
  double                                                                   myCritWeights[3];
  bool                                                                     myUseSmoothing;
  bool                                                                     myMutableInput;
  gp_Vec                                                                   myFirstTangent;
  gp_Vec                                                                   myLastTangent;
  double                                                                   myFirstNormalTolerance;
  double                                                                   myLastNormalTolerance;
  TopoDS_Face                                                              myFirstSupport;
  TopoDS_Face                                                              myLastSupport;
  EdgeFaceMap                                                              myFirstEdgeSupports;
  EdgeFaceMap                                                              myLastEdgeSupports;
  EdgeFaceMap                            myFirstWorkingEdgeSupports;
  EdgeFaceMap                            myLastWorkingEdgeSupports;
  double                                 myFirstSupportAngularTolerance;
  double                                 myLastSupportAngularTolerance;
  double                                 myFirstSupportCurvatureTolerance;
  double                                 myLastSupportCurvatureTolerance;
  bool                                   myHasFirstTangent;
  bool                                   myHasLastTangent;
  bool                                   myFirstTangentIsNormal;
  bool                                   myLastTangentIsNormal;
  bool                                   myFirstTangentIsSupport;
  bool                                   myLastTangentIsSupport;
  GeomAbs_Shape                          myFirstBoundaryContinuity;
  GeomAbs_Shape                          myLastBoundaryContinuity;
  NCollection_Handle<BRepFill_Generator> myBFGenerator;
  BRepFill_ThruSectionErrorStatus        myStatus;
};

#endif // _BRepOffsetAPI_ThruSections_HeaderFile
