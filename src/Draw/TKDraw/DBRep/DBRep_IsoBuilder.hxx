// Created on: 1994-03-25
// Created by: Jean Marc LACHAUME
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

#ifndef _DBRep_IsoBuilder_HeaderFile
#define _DBRep_IsoBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <NCollection_IndexedDataMap.hxx>
class TopoDS_Face;
class DBRep_Face;

//! Creation of isoparametric curves.
class DBRep_IsoBuilder : public Geom2dHatch_Hatcher
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates the builder.
  Standard_EXPORT DBRep_IsoBuilder(const TopoDS_Face&     TopologicalFace,
                                   const Standard_Real    Infinite,
                                   const Standard_Integer NbIsos);

  //! Returns the total number of domains.
  Standard_Integer NbDomains() const { return myNbDom; }

  //! Loading of the isoparametric curves in the
  //! Data Structure of a drawable face.
  Standard_EXPORT void LoadIsos(const Handle(DBRep_Face)& Face) const;

protected:
  typedef NCollection_IndexedDataMap<TopoDS_Shape, Handle(Geom2d_Curve)> DataMapOfEdgePCurve;

  //! Adds to the hatcher the 2D segments connecting the p-curves
  //! of the neighboring edges to close the 2D gaps which are
  //! closed in 3D by the tolerance of vertices shared between edges.
  //! It will allow trimming correctly the iso-lines passing through
  //! such gaps.
  //! The method also trims the intersecting 2D curves of the face,
  //! forbidding the iso-lines beyond the face boundaries.
  Standard_EXPORT void FillGaps(const TopoDS_Face& theFace, DataMapOfEdgePCurve& theEdgePCurveMap);

private:
  Standard_Real           myInfinite;
  Standard_Real           myUMin;
  Standard_Real           myUMax;
  Standard_Real           myVMin;
  Standard_Real           myVMax;
  TColStd_Array1OfReal    myUPrm;
  TColStd_Array1OfInteger myUInd;
  TColStd_Array1OfReal    myVPrm;
  TColStd_Array1OfInteger myVInd;
  Standard_Integer        myNbDom;
};

#endif // _DBRep_IsoBuilder_HeaderFile
