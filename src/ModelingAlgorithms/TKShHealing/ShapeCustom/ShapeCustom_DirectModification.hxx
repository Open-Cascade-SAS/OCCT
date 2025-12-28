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

#ifndef _ShapeCustom_DirectModification_HeaderFile
#define _ShapeCustom_DirectModification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ShapeCustom_Modification.hxx>
#include <GeomAbs_Shape.hxx>
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Vertex;
class gp_Pnt;
class Geom2d_Curve;

//! implements a modification for the BRepTools
//! Modifier algorithm. Will redress indirect
//! surfaces.
class ShapeCustom_DirectModification : public ShapeCustom_Modification
{

public:
  Standard_EXPORT ShapeCustom_DirectModification();

  //! Returns true if the face <F> has been
  //! modified. In this case, <S> is the new geometric
  //! support of the face, <L> the new location, <Tol>
  //! the new tolerance. Otherwise, returns
  //! false, and <S>, <L>, <Tol> are not
  //! significant.
  Standard_EXPORT bool NewSurface(const TopoDS_Face&    F,
                                              occ::handle<Geom_Surface>& S,
                                              TopLoc_Location&      L,
                                              double&        Tol,
                                              bool&     RevWires,
                                              bool&     RevFace) override;

  //! Returns true if the edge <E> has been
  //! modified. In this case, <C> is the new geometric
  //! support of the edge, <L> the new location, <Tol>
  //! the new tolerance. Otherwise, returns
  //! false, and <C>, <L>, <Tol> are not
  //! significant.
  Standard_EXPORT bool NewCurve(const TopoDS_Edge&  E,
                                            occ::handle<Geom_Curve>& C,
                                            TopLoc_Location&    L,
                                            double&      Tol) override;

  //! Returns true if the vertex <V> has been
  //! modified. In this case, <P> is the new geometric
  //! support of the vertex, <Tol> the new tolerance.
  //! Otherwise, returns false, and <P>, <Tol>
  //! are not significant.
  Standard_EXPORT bool NewPoint(const TopoDS_Vertex& V,
                                            gp_Pnt&              P,
                                            double&       Tol) override;

  //! Returns true if the edge <E> has a new
  //! curve on surface on the face <F>.In this case, <C>
  //! is the new geometric support of the edge, <L> the
  //! new location, <Tol> the new tolerance.
  //!
  //! Otherwise, returns false, and <C>, <L>,
  //! <Tol> are not significant.
  //!
  //! <NewE> is the new edge created from <E>. <NewF>
  //! is the new face created from <F>. They may be useful.
  Standard_EXPORT bool NewCurve2d(const TopoDS_Edge&    E,
                                              const TopoDS_Face&    F,
                                              const TopoDS_Edge&    NewE,
                                              const TopoDS_Face&    NewF,
                                              occ::handle<Geom2d_Curve>& C,
                                              double&        Tol) override;

  //! Returns true if the Vertex <V> has a new
  //! parameter on the edge <E>. In this case, <P> is
  //! the parameter, <Tol> the new tolerance.
  //! Otherwise, returns false, and <P>, <Tol>
  //! are not significant.
  Standard_EXPORT bool NewParameter(const TopoDS_Vertex& V,
                                                const TopoDS_Edge&   E,
                                                double&       P,
                                                double&       Tol) override;

  //! Returns the continuity of <NewE> between <NewF1>
  //! and <NewF2>.
  //!
  //! <NewE> is the new edge created from <E>. <NewF1>
  //! (resp. <NewF2>) is the new face created from <F1>
  //! (resp. <F2>).
  Standard_EXPORT GeomAbs_Shape Continuity(const TopoDS_Edge& E,
                                           const TopoDS_Face& F1,
                                           const TopoDS_Face& F2,
                                           const TopoDS_Edge& NewE,
                                           const TopoDS_Face& NewF1,
                                           const TopoDS_Face& NewF2) override;

  DEFINE_STANDARD_RTTIEXT(ShapeCustom_DirectModification, ShapeCustom_Modification)

};

#endif // _ShapeCustom_DirectModification_HeaderFile
