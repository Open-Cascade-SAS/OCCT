// Created on: 1994-03-10
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepClass3d_SolidExplorer_HeaderFile
#define _BRepClass3d_SolidExplorer_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <BRepClass3d_BndBoxTree.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_State.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedMap.hxx>

class gp_Pnt;
class TopoDS_Face;
class gp_Vec;
class TopoDS_Shell;
class gp_Lin;
class IntCurvesFace_Intersector;

//! Provide an exploration of a BRep Shape for the classification.
//! Provide access to the special UB tree to obtain fast search.
class BRepClass3d_SolidExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepClass3d_SolidExplorer();

  Standard_EXPORT BRepClass3d_SolidExplorer(const TopoDS_Shape& S);

  Standard_EXPORT virtual ~BRepClass3d_SolidExplorer();

  Standard_EXPORT void InitShape(const TopoDS_Shape& S);

  //! Should return True if P outside of bounding vol. of the shape
  Standard_EXPORT virtual bool Reject(const gp_Pnt& P) const;

  //! compute a point P in the face F. Param is a Real in
  //! ]0,1[ and is used to initialise the algorithm. For
  //! different values , different points are returned.
  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F, gp_Pnt& P, double& Param);

  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F,
                                                  gp_Pnt&            P,
                                                  double&            u,
                                                  double&            v,
                                                  double&            Param);

  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F,
                                                  gp_Pnt&            P,
                                                  double&            u,
                                                  double&            v,
                                                  double&            Param,
                                                  gp_Vec&            theVecD1U,
                                                  gp_Vec&            theVecD1V);

  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F,
                                                  gp_Pnt&            P,
                                                  double&            u,
                                                  double&            v);

  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F, gp_Pnt& P);

  Standard_EXPORT static bool FindAPointInTheFace(const TopoDS_Face& F, double& u, double& v);

  Standard_EXPORT bool PointInTheFace(const TopoDS_Face& F,
                                      gp_Pnt&            P,
                                      double&            u,
                                      double&            v,
                                      double&            Param,
                                      int&               Index) const;

  Standard_EXPORT bool PointInTheFace(const TopoDS_Face&                      F,
                                      gp_Pnt&                                 P,
                                      double&                                 u,
                                      double&                                 v,
                                      double&                                 Param,
                                      int&                                    Index,
                                      const occ::handle<BRepAdaptor_Surface>& surf,
                                      const double                            u1,
                                      const double                            v1,
                                      const double                            u2,
                                      const double                            v2) const;

  //! <Index> gives point index to search from and returns
  //! point index of succeseful search
  Standard_EXPORT bool PointInTheFace(const TopoDS_Face&                      F,
                                      gp_Pnt&                                 P,
                                      double&                                 u,
                                      double&                                 v,
                                      double&                                 Param,
                                      int&                                    Index,
                                      const occ::handle<BRepAdaptor_Surface>& surf,
                                      const double                            u1,
                                      const double                            v1,
                                      const double                            u2,
                                      const double                            v2,
                                      gp_Vec&                                 theVecD1U,
                                      gp_Vec&                                 theVecD1V) const;

  //! Starts an exploration of the shells.
  Standard_EXPORT void InitShell();

  //! Returns True if there is a current shell.
  Standard_EXPORT bool MoreShell() const;

  //! Sets the explorer to the next shell.
  Standard_EXPORT void NextShell();

  //! Returns the current shell.
  Standard_EXPORT TopoDS_Shell CurrentShell() const;

  //! Returns True if the Shell is rejected.
  Standard_EXPORT virtual bool RejectShell(const gp_Lin& L) const;

  //! Starts an exploration of the faces of the current shell.
  Standard_EXPORT void InitFace();

  //! Returns True if current face in current shell.
  Standard_EXPORT bool MoreFace() const;

  //! Sets the explorer to the next Face of the current shell.
  Standard_EXPORT void NextFace();

  //! Returns the current face.
  Standard_EXPORT TopoDS_Face CurrentFace() const;

  //! returns True if the face is rejected.
  Standard_EXPORT virtual bool RejectFace(const gp_Lin& L) const;

  //! Returns in <L>, <Par> a segment having at least
  //! one intersection with the shape boundary to
  //! compute intersections.
  Standard_EXPORT int Segment(const gp_Pnt& P, gp_Lin& L, double& Par);

  //! Returns in <L>, <Par> a segment having at least
  //! one intersection with the shape boundary to
  //! compute intersections.
  //!
  //! The First Call to this method returns a line which
  //! point to a point of the first face of the shape.
  //! The Second Call provide a line to the second face
  //! and so on.
  Standard_EXPORT int OtherSegment(const gp_Pnt& P, gp_Lin& L, double& Par);

  //! Returns the index of face for which
  //! last segment is calculated.
  Standard_EXPORT int GetFaceSegmentIndex() const;

  Standard_EXPORT virtual void DumpSegment(const gp_Pnt&      P,
                                           const gp_Lin&      L,
                                           const double       Par,
                                           const TopAbs_State S) const;

  Standard_EXPORT const TopoDS_Shape& GetShape() const;

  Standard_EXPORT IntCurvesFace_Intersector& Intersector(const TopoDS_Face& F) const;

  //! Return UB-tree instance which is used for edge / vertex checks.
  const NCollection_UBTree<int, Bnd_Box>& GetTree() { return myTree; }

  //! Return edge/vertices map for current shape.
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& GetMapEV()
  {
    return myMapEV;
  }

  Standard_EXPORT void Destroy();

private:
  Standard_EXPORT BRepClass3d_SolidExplorer(const BRepClass3d_SolidExplorer& Oth) = delete;

protected:
  Standard_EXPORT TopAbs_State ClassifyUVPoint(const IntCurvesFace_Intersector& theIntersector,
                                               const occ::handle<BRepAdaptor_Surface>& theSurf,
                                               const gp_Pnt2d& theP2d) const;

private:
  TopoDS_Shape                                                      myShape;
  bool                                                              myReject;
  int                                                               myFirstFace;
  double                                                            myParamOnEdge;
  TopExp_Explorer                                                   myShellExplorer;
  TopExp_Explorer                                                   myFaceExplorer;
  NCollection_DataMap<TopoDS_Shape, void*, TopTools_ShapeMapHasher> myMapOfInter;
  NCollection_UBTree<int, Bnd_Box>                                  myTree;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>     myMapEV;
};

#endif // _BRepClass3d_SolidExplorer_HeaderFile
