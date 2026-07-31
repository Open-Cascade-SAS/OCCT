// Created on: 1994-09-28
// Created by: Marie Jose MARTZ
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

#ifndef _IGESToBRep_BRepEntity_HeaderFile
#define _IGESToBRep_BRepEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESToBRep_CurveAndSurface.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>

class TopoDS_Shape;
class IGESData_IGESEntity;
class TopoDS_Vertex;
class IGESSolid_VertexList;
class IGESSolid_EdgeList;
class IGESSolid_Loop;
class TopoDS_Face;
class gp_Trsf2d;
class IGESSolid_Face;
class IGESSolid_Shell;
class IGESSolid_ManifoldSolid;

//! Provides methods to transfer BRep entities
//! ( VertexList 502, EdgeList 504, Loop 508,
//! Face 510, Shell 514, ManifoldSolid 186)
//! from IGES to CASCADE.
class IGESToBRep_BRepEntity : public IGESToBRep_CurveAndSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a tool BRepEntity ready to run, with
  //! epsilons set to 1.E-04, TheModeTopo to True, the
  //! optimization of the continuity to False.
  Standard_EXPORT IGESToBRep_BRepEntity();

  //! Creates a tool BRepEntity ready to run and sets its
  //! fields as CS's.
  Standard_EXPORT IGESToBRep_BRepEntity(const IGESToBRep_CurveAndSurface& CS);

  //! Creates a tool BRepEntity ready to run.
  Standard_EXPORT IGESToBRep_BRepEntity(const double eps,
                                        const double epsGeom,
                                        const double epsCoeff,
                                        const bool   mode,
                                        const bool   modeapprox,
                                        const bool   optimized);

  //! Transfer the BRepEntity" : Face, Shell or ManifoldSolid.
  Standard_EXPORT TopoDS_Shape
    TransferBRepEntity(const occ::handle<IGESData_IGESEntity>& start,
                       const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfer the entity number "index" of the VertexList "start"
  Standard_EXPORT TopoDS_Vertex TransferVertex(const occ::handle<IGESSolid_VertexList>& start,
                                               const int                                index);

  //! Transfer the entity number "index" of the EdgeList "start".
  Standard_EXPORT TopoDS_Shape TransferEdge(const occ::handle<IGESSolid_EdgeList>& start,
                                            const int                              index);

  //! Transfer the Loop Entity
  Standard_EXPORT TopoDS_Shape TransferLoop(const occ::handle<IGESSolid_Loop>& start,
                                            const TopoDS_Face&                 Face,
                                            const gp_Trsf2d&                   trans,
                                            const double                       uFact);

  //! Transfer the Face Entity
  Standard_EXPORT TopoDS_Shape TransferFace(const occ::handle<IGESSolid_Face>& start);

  //! Transfer the Shell Entity
  Standard_EXPORT TopoDS_Shape
    TransferShell(const occ::handle<IGESSolid_Shell>& start,
                  const Message_ProgressRange&        theProgress = Message_ProgressRange());

  //! Transfer the ManifoldSolid Entity
  Standard_EXPORT TopoDS_Shape
    TransferManifoldSolid(const occ::handle<IGESSolid_ManifoldSolid>& start,
                          const Message_ProgressRange& theProgress = Message_ProgressRange());
};

#endif // _IGESToBRep_BRepEntity_HeaderFile
