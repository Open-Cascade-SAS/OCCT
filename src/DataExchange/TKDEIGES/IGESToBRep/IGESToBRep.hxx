// Created on: 1994-03-22
// Created by: Frederic UNTEREINER
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

#ifndef _IGESToBRep_HeaderFile
#define _IGESToBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class IGESToBRep_AlgoContainer;
class IGESData_IGESEntity;
class TopoDS_Edge;
class TopoDS_Face;

//! Provides tools in order to transfer IGES entities
//! to CAS.CADE.
class IGESToBRep
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates and initializes default AlgoContainer.
  Standard_EXPORT static void Init();

  //! Sets default AlgoContainer
  Standard_EXPORT static void SetAlgoContainer(
    const occ::handle<IGESToBRep_AlgoContainer>& aContainer);

  //! Returns default AlgoContainer
  Standard_EXPORT static occ::handle<IGESToBRep_AlgoContainer> AlgoContainer();

  //! Return True if the IGESEntity can be transferred by
  //! TransferCurveAndSurface.
  //! ex: All IGESEntity from IGESGeom
  Standard_EXPORT static bool IsCurveAndSurface(const occ::handle<IGESData_IGESEntity>& start);

  //! Return True if the IGESEntity can be transferred by
  //! TransferBasicCurve.
  //! ex: CircularArc, ConicArc, Line, CopiousData,
  //! BSplineCurve, SplineCurve... from IGESGeom :
  //! 104,110,112,126
  Standard_EXPORT static bool IsBasicCurve(const occ::handle<IGESData_IGESEntity>& start);

  //! Return True if the IGESEntity can be transferred by
  //! TransferBasicSurface.
  //! ex: BSplineSurface, SplineSurface... from IGESGeom :
  //! 114,128
  Standard_EXPORT static bool IsBasicSurface(const occ::handle<IGESData_IGESEntity>& start);

  //! Return True if the IGESEntity can be transferred by
  //! TransferTopoCurve.
  //! ex: all Curves from IGESGeom :
  //! all basic curves,102,130,142,144
  Standard_EXPORT static bool IsTopoCurve(const occ::handle<IGESData_IGESEntity>& start);

  //! Return True if the IGESEntity can be transferred by
  //! TransferTopoSurface.
  //! ex: All Surfaces from IGESGeom :
  //! all basic surfaces,108,118,120,122,141,143
  Standard_EXPORT static bool IsTopoSurface(const occ::handle<IGESData_IGESEntity>& start);

  //! Return True if the IGESEntity can be transferred by
  //! TransferBRepEntity.
  //! ex: VertexList, EdgeList, Loop, Face, Shell,
  //! Manifold Solid BRep Object from IGESSolid :
  //! 502, 504, 508, 510, 514, 186.
  Standard_EXPORT static bool IsBRepEntity(const occ::handle<IGESData_IGESEntity>& start);

  Standard_EXPORT static int IGESCurveToSequenceOfIGESCurve(
    const occ::handle<IGESData_IGESEntity>&                              curve,
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& sequence);

  Standard_EXPORT static bool TransferPCurve(const TopoDS_Edge& fromedge,
                                             const TopoDS_Edge& toedge,
                                             const TopoDS_Face& face);
};

#endif // _IGESToBRep_HeaderFile
