// Created on: 1993-07-06
// Created by: Yves FRICAUD
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

#ifndef _BRepMAT2d_BisectingLocus_HeaderFile
#define _BRepMAT2d_BisectingLocus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <MAT2d_Tool2d.hxx>
#include <Standard_Boolean.hxx>
#include <MAT2d_DataMapOfBiIntInteger.hxx>
#include <Standard_Integer.hxx>
#include <MAT_Side.hxx>
#include <GeomAbs_JoinType.hxx>
#include <MAT_DataMapOfIntegerBasicElt.hxx>
class MAT_Graph;
class BRepMAT2d_Explorer;
class MAT_BasicElt;
class Geom2d_Geometry;
class gp_Pnt2d;
class MAT_Node;
class Bisector_Bisec;
class MAT_Arc;


//! BisectingLocus generates and contains the Bisecting_Locus
//! of a set of lines from Geom2d, defined by <ExploSet>.
//!
//! If the set of lines contains closed lines:
//! ------------------------------------------
//! These lines cut the plane  in areas.
//! One map can  be  computed for each area.
//!
//! Bisecting locus computes a map in an area.
//! The area is defined by a side (MAT_Left,MAT_Right)
//! on one of the closed lines.
//!
//! If the set of lines contains only open lines:
//! --------------------------------------------
//! the map recovers all the plane.
//!
//! Warning: Assume the orientation of the   closed  lines  are
//! compatible.
//!
//! Assume the explo contains only lines located in the
//! area where the bisecting locus will be computed.
//!
//! Assume a line don't cross itself or an other line.
//!
//! Remark:
//! the  curves  coming   from   the  explorer can   be
//! decomposed in different parts. It  the  case for the
//! curves other than circles or lines.
//!
//! The map of bisecting  locus is described by a graph.
//! - The  BasicsElements  correspond  to elements on
//! the figure described by the Explorer from BRepMAT2d.
//! - The Arcs correspond to the bisectors.
//! - The Nodes are the extremities of the arcs.
class BRepMAT2d_BisectingLocus 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepMAT2d_BisectingLocus();
  
  //! Computation of the Bisector_Locus in a set of Lines
  //! defined in <anExplo>.
  //! The bisecting locus are computed on the side <aSide>
  //! from the line <LineIndex> in <anExplo>.
  Standard_EXPORT void Compute (BRepMAT2d_Explorer& anExplo, const Standard_Integer LineIndex = 1, const MAT_Side aSide = MAT_Left, const GeomAbs_JoinType aJoinType = GeomAbs_Arc, const Standard_Boolean IsOpenResult = Standard_False);
  
  //! Returns True if Compute has succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns <theGraph> of <me>.
  Standard_EXPORT Handle(MAT_Graph) Graph() const;
  
  //! Returns the number of contours.
  Standard_EXPORT Standard_Integer NumberOfContours() const;
  
  //! Returns the number of BasicElts on the line
  //! <IndLine>.
  Standard_EXPORT Standard_Integer NumberOfElts (const Standard_Integer IndLine) const;
  
  //! Returns the number of sections of a curve.
  //! this curve is the Indexth curve in the IndLineth contour
  //! given by anExplo.
  Standard_EXPORT Standard_Integer NumberOfSections (const Standard_Integer IndLine, const Standard_Integer Index) const;
  
  //! Returns the BasicElts located at the position
  //! <Index> on the contour designed by <IndLine>.
  //! Remark: the BasicElts on a contour are sorted.
  Standard_EXPORT Handle(MAT_BasicElt) BasicElt (const Standard_Integer IndLine, const Standard_Integer Index) const;
  
  //! Returns the geometry linked to the <BasicElt>.
  Standard_EXPORT Handle(Geom2d_Geometry) GeomElt (const Handle(MAT_BasicElt)& aBasicElt) const;
  
  //! Returns the geometry of  type <gp> linked to
  //! the <Node>.
  Standard_EXPORT gp_Pnt2d GeomElt (const Handle(MAT_Node)& aNode) const;
  
  //! Returns the  geometry of type <Bissec>
  //! linked   to the arc <ARC>.
  //! <Reverse> is False when the FirstNode of <anArc>
  //! correspond to the first point of geometry.
  Standard_EXPORT Bisector_Bisec GeomBis (const Handle(MAT_Arc)& anArc, Standard_Boolean& Reverse) const;




protected:





private:

  
  Standard_EXPORT void Fusion();
  
  Standard_EXPORT void RenumerationAndFusion (const Standard_Integer IndexLine, const Standard_Integer LengthLine, Standard_Integer& IndexLast, MAT_DataMapOfIntegerBasicElt& NewMap);


  Handle(MAT_Graph) theGraph;
  MAT2d_Tool2d theTool;
  Standard_Boolean isDone;
  MAT2d_DataMapOfBiIntInteger nbSect;
  Standard_Integer nbContours;


};







#endif // _BRepMAT2d_BisectingLocus_HeaderFile
