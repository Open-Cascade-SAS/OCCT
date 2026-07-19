// Created on: 1999-09-13
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeAnalysis_CheckSmallFace_HeaderFile
#define _ShapeAnalysis_CheckSmallFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <ShapeExtend_Status.hxx>
class TopoDS_Face;
class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Compound;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Analysis of the face size
class ShapeAnalysis_CheckSmallFace
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty tool
  //! Checks a Shape i.e. each of its faces, records checks as
  //! diagnostics in the <infos>
  //!
  //! If <infos> has not been set before, no check is done
  //!
  //! For faces which are in a Shell, topological data are recorded
  //! to allow recovering connectivities after fixing or removing
  //! the small faces or parts of faces
  //! Enchains various checks on a face
  //! inshell : to compute more information, relevant to topology
  Standard_EXPORT ShapeAnalysis_CheckSmallFace();

  //! Checks if a Face is as a Spot
  //! Returns 0 if not, 1 if yes, 2 if yes and all vertices are the
  //! same
  //! By default, considers the tolerance zone of its vertices
  //! A given value <tol> may be given to check a spot of this size
  //! If a Face is a Spot, its location is returned in <spot>, and
  //! <spotol> returns an equivalent tolerance, which is computed as
  //! half of max dimension of min-max box of the face
  Standard_EXPORT int IsSpotFace(const TopoDS_Face& F,
                                 gp_Pnt&            spot,
                                 double&            spotol,
                                 const double       tol = -1.0) const;

  //! Acts as IsSpotFace, but records in <infos> a diagnostic
  //! "SpotFace" with the Pnt as value (data "Location")
  Standard_EXPORT bool CheckSpotFace(const TopoDS_Face& F, const double tol = -1.0);

  //! Checks if a Face lies on a Surface which is a strip
  //! So the Face is a strip. But a Face may be a strip elsewhere ..
  //!
  //! A given value <tol> may be given to check max width
  //! By default, considers the tolerance zone of its edges
  //! Returns 0 if not a strip support, 1 strip in U, 2 strip in V
  Standard_EXPORT bool IsStripSupport(const TopoDS_Face& F, const double tol = -1.0);

  //! Checks if two edges define a strip, i.e. distance maxi below
  //! tolerance, given or some of those of E1 and E2
  Standard_EXPORT bool CheckStripEdges(const TopoDS_Edge& E1,
                                       const TopoDS_Edge& E2,
                                       const double       tol,
                                       double&            dmax) const;

  //! Searches for two and only two edges up tolerance
  //! Returns True if OK, false if not 2 edges
  //! If True, returns the two edges and their maximum distance
  Standard_EXPORT bool FindStripEdges(const TopoDS_Face& F,
                                      TopoDS_Edge&       E1,
                                      TopoDS_Edge&       E2,
                                      const double       tol,
                                      double&            dmax);

  //! Checks if a Face is a single strip, i.e. brings two great
  //! edges which are confused on their whole length, possible other
  //! edges are small or null length
  //!
  //! Returns 0 if not a strip support, 1 strip in U, 2 strip in V
  //! Records diagnostic in info if it is a single strip
  Standard_EXPORT bool CheckSingleStrip(const TopoDS_Face& F,
                                        TopoDS_Edge&       E1,
                                        TopoDS_Edge&       E2,
                                        const double       tol = -1.0);

  //! Checks if a Face is as a Strip
  //! Returns 0 if not or non determined, 1 if in U, 2 if in V
  //! By default, considers the tolerance zone of its edges
  //! A given value <tol> may be given to check a strip of max this width
  //!
  //! If a Face is determined as a Strip, it is delinited by two
  //! lists of edges. These lists are recorded in diagnostic
  //! Diagnostic "StripFace" brings data "Direction" (U or V),
  //! "List1" , "List2" (if they could be computed)
  Standard_EXPORT bool CheckStripFace(const TopoDS_Face& F,
                                      TopoDS_Edge&       E1,
                                      TopoDS_Edge&       E2,
                                      const double       tol = -1.0);

  //! Checks if a Face brings vertices which split it, either
  //! confused with non adjacent vertices, or confused with their
  //! projection on non adjacent edges
  //! Returns the count of found splitting vertices
  //! Each vertex then brings a diagnostic "SplittingVertex",
  //! with data : "Face" for the face, "Edge" for the split edge
  Standard_EXPORT int CheckSplittingVertices(
    const TopoDS_Face& F,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                                          MapEdges,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<double>, TopTools_ShapeMapHasher>& MapParam,
    TopoDS_Compound& theAllVert);

  //! Checks if a Face has a pin, which can be edited
  //! No singularity : no pin, returns 0
  //! If there is a pin, checked topics, with returned value :
  //! - 0 : nothing to do more
  //! - 1 : "smooth", i.e. not a really sharp pin
  //! -> diagnostic "SmoothPin"
  //! - 2 : stretched pin, i.e. is possible to relimit the face by
  //! another vertex, so that this vertex still gives a pin
  //! -> diagnostic "StretchedPin" with location of vertex (Pnt)
  Standard_EXPORT bool CheckPin(const TopoDS_Face& F, int& whatrow, int& sence);

  //! Checks if a Face is twisted (apart from checking Pin, i.e. it
  //! does not give information on pin, only "it is twisted")
  Standard_EXPORT bool CheckTwisted(const TopoDS_Face& F, double& paramu, double& paramv);

  Standard_EXPORT bool CheckPinFace(
    const TopoDS_Face&                                                        F,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& mapEdges,
    const double                                                              toler = -1.0);

  Standard_EXPORT bool CheckPinEdges(const TopoDS_Edge& theFirstEdge,
                                     const TopoDS_Edge& theSecondEdge,
                                     const double       coef1,
                                     const double       coef2,
                                     const double       toler) const;

  //! Returns the status of last call to Perform()
  //! ShapeExtend_OK   : face was OK, nothing done
  //! ShapeExtend_DONE1: some wires are fixed
  //! ShapeExtend_DONE2: orientation of wires fixed
  //! ShapeExtend_DONE3: missing seam added
  //! ShapeExtend_DONE4: small area wire removed
  //! ShapeExtend_DONE5: natural bounds added
  //! ShapeExtend_FAIL1: some fails during fixing wires
  //! ShapeExtend_FAIL2: cannot fix orientation of wires
  //! ShapeExtend_FAIL3: cannot add missing seam
  //! ShapeExtend_FAIL4: cannot remove small area wire
  bool Status(const ShapeExtend_Status status) const;

  //! Sets a fixed Tolerance to check small face
  //! By default, local tolerance zone is considered
  //! Sets a fixed MaxTolerance to check small face
  //! Sets a fixed Tolerance to check small face
  //! By default, local tolerance zone is considered
  //! Unset fixed tolerance, comes back to local tolerance zones
  //! Unset fixed tolerance, comes back to local tolerance zones
  void SetTolerance(const double tol);

  //! Returns the tolerance to check small faces, negative value if
  //! local tolerances zones are to be considered
  double Tolerance() const;

  bool StatusSpot(const ShapeExtend_Status status) const;

  bool StatusStrip(const ShapeExtend_Status status) const;

  bool StatusPin(const ShapeExtend_Status status) const;

  bool StatusTwisted(const ShapeExtend_Status status) const;

  bool StatusSplitVert(const ShapeExtend_Status status) const;

  bool StatusPinFace(const ShapeExtend_Status status) const;

  bool StatusPinEdges(const ShapeExtend_Status status) const;

private:
  TopoDS_Shape myComp;
  int          myStatus;
  int          myStatusSpot;
  int          myStatusStrip;
  int          myStatusPin;
  int          myStatusTwisted;
  int          myStatusSplitVert;
  int          myStatusPinFace;
  int          myStatusPinEdges;
  double       myPrecision;
};

#include <ShapeAnalysis_CheckSmallFace.lxx>

#endif // _ShapeAnalysis_CheckSmallFace_HeaderFile
