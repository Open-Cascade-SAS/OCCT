// Created on: 1994-03-14
// Created by: s:	Christophe GUYOT & Frederic UNTEREINER
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

#ifndef _IGESToBRep_CurveAndSurface_HeaderFile
#define _IGESToBRep_CurveAndSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>

class Geom_Surface;
class IGESData_IGESModel;
class Transfer_TransientProcess;
class TopoDS_Shape;
class IGESData_IGESEntity;
class Message_Msg;

//! Provides methods to transfer CurveAndSurface from IGES to CASCADE.
class IGESToBRep_CurveAndSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a tool CurveAndSurface ready to run, with
  //! epsilons set to 1.E-04, myModeTopo to True, the
  //! optimization of the continuity to False.
  Standard_EXPORT IGESToBRep_CurveAndSurface();

  //! Creates a tool CurveAndSurface ready to run.
  Standard_EXPORT IGESToBRep_CurveAndSurface(const double eps,
                                             const double epsGeom,
                                             const double epsCoeff,
                                             const bool   mode,
                                             const bool   modeapprox,
                                             const bool   optimized);

  //! Initializes the field of the tool CurveAndSurface with
  //! default creating values.
  Standard_EXPORT void Init();

  //! Changes the value of "myEps"
  void SetEpsilon(const double eps);

  //! Returns the value of "myEps"
  double GetEpsilon() const;

  //! Changes the value of "myEpsCoeff"
  void SetEpsCoeff(const double eps);

  //! Returns the value of "myEpsCoeff"
  double GetEpsCoeff() const;

  //! Changes the value of "myEpsGeom"
  Standard_EXPORT void SetEpsGeom(const double eps);

  //! Returns the value of "myEpsGeom"
  double GetEpsGeom() const;

  //! Changes the value of "myMinTol"
  void SetMinTol(const double mintol);

  //! Changes the value of "myMaxTol"
  void SetMaxTol(const double maxtol);

  //! Sets values of "myMinTol" and "myMaxTol" as follows
  //! myMaxTol = Max ("read.maxprecision.val", myEpsGeom * myUnitFactor)
  //! myMinTol = Precision::Confusion()
  //! Remark: This method is automatically invoked each time the values
  //! of "myEpsGeom" or "myUnitFactor" are changed
  Standard_EXPORT void UpdateMinMaxTol();

  //! Returns the value of "myMinTol"
  double GetMinTol() const;

  //! Returns the value of "myMaxTol"
  double GetMaxTol() const;

  //! Changes the value of "myModeApprox"
  void SetModeApprox(const bool mode);

  //! Returns the value of "myModeApprox"
  bool GetModeApprox() const;

  //! Changes the value of "myModeIsTopo"
  void SetModeTransfer(const bool mode);

  //! Returns the value of "myModeIsTopo"
  bool GetModeTransfer() const;

  //! Changes the value of "myContIsOpti"
  void SetOptimized(const bool optimized);

  //! Returns the value of "myContIsOpti"
  bool GetOptimized() const;

  //! Returns the value of " myUnitFactor"
  double GetUnitFactor() const;

  //! Changes the value of "mySurfaceCurve"
  void SetSurfaceCurve(const int ival);

  //! Returns the value of "mySurfaceCurve" 0 = value in
  //! file, 2 = keep 2d and compute 3d, 3 = keep 3d and
  //! compute 2d
  int GetSurfaceCurve() const;

  //! Set the value of "myModel"
  Standard_EXPORT void SetModel(const occ::handle<IGESData_IGESModel>& model);

  //! Returns the value of "myModel"
  occ::handle<IGESData_IGESModel> GetModel() const;

  //! Changes the value of "myContinuity"
  //! if continuity = 0 do nothing else
  //! if continuity = 1 try C1
  //! if continuity = 2 try C2
  void SetContinuity(const int continuity);

  //! Returns the value of "myContinuity"
  int GetContinuity() const;

  //! Set the value of "myMsgReg"
  void SetTransferProcess(const occ::handle<Transfer_TransientProcess>& TP);

  //! Returns the value of "myMsgReg"
  occ::handle<Transfer_TransientProcess> GetTransferProcess() const;

  //! Returns the result of the transfert of any IGES Curve
  //! or Surface Entity. If the transfer has failed, this
  //! member return a NullEntity.
  Standard_EXPORT TopoDS_Shape
    TransferCurveAndSurface(const occ::handle<IGESData_IGESEntity>& start,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Returns the result of the transfert the geometry of
  //! any IGESEntity. If the transfer has failed, this
  //! member return a NullEntity.
  Standard_EXPORT TopoDS_Shape
    TransferGeometry(const occ::handle<IGESData_IGESEntity>& start,
                     const Message_ProgressRange&            theProgress = Message_ProgressRange());

  //! Records a new Fail message
  void SendFail(const occ::handle<IGESData_IGESEntity>& start, const Message_Msg& amsg);

  //! Records a new Warning message
  void SendWarning(const occ::handle<IGESData_IGESEntity>& start, const Message_Msg& amsg);

  //! Records a new Information message from the definition
  //! of a Msg (Original+Value)
  void SendMsg(const occ::handle<IGESData_IGESEntity>& start, const Message_Msg& amsg);

  //! Returns True if start was already treated and has a result in "myMap"
  //! else returns False.
  Standard_EXPORT bool HasShapeResult(const occ::handle<IGESData_IGESEntity>& start) const;

  //! Returns the result of the transfer of the IGESEntity "start" contained
  //! in "myMap" . (if HasShapeResult is True).
  Standard_EXPORT TopoDS_Shape GetShapeResult(const occ::handle<IGESData_IGESEntity>& start) const;

  //! set in "myMap" the result of the transfer of the IGESEntity "start".
  Standard_EXPORT void SetShapeResult(const occ::handle<IGESData_IGESEntity>& start,
                                      const TopoDS_Shape&                     result);

  //! Returns the number of shapes results contained in "myMap" for the
  //! IGESEntity start (type VertexList or EdgeList).
  Standard_EXPORT int NbShapeResult(const occ::handle<IGESData_IGESEntity>& start) const;

  //! Returns the numth result of the IGESEntity start (type VertexList or
  //! EdgeList) in "myMap". (if NbShapeResult is not null).
  Standard_EXPORT TopoDS_Shape GetShapeResult(const occ::handle<IGESData_IGESEntity>& start,
                                              const int                               num) const;

  //! set in "myMap" the result of the transfer of the entity of the
  //! IGESEntity start (type VertexList or EdgeList).
  Standard_EXPORT void AddShapeResult(const occ::handle<IGESData_IGESEntity>& start,
                                      const TopoDS_Shape&                     result);

  Standard_EXPORT void SetSurface(const occ::handle<Geom_Surface>& theSurface);

  Standard_EXPORT occ::handle<Geom_Surface> Surface() const;

  Standard_EXPORT double GetUVResolution();

private:
  double                                 myEps;
  double                                 myEpsCoeff;
  double                                 myEpsGeom;
  double                                 myMinTol;
  double                                 myMaxTol;
  bool                                   myModeIsTopo;
  bool                                   myModeApprox;
  bool                                   myContIsOpti;
  double                                 myUnitFactor;
  int                                    mySurfaceCurve;
  int                                    myContinuity;
  occ::handle<Geom_Surface>              mySurface;
  double                                 myUVResolution;
  bool                                   myIsResolCom;
  occ::handle<IGESData_IGESModel>        myModel;
  occ::handle<Transfer_TransientProcess> myTP;
};

#include <IGESToBRep_CurveAndSurface.lxx>

#endif // _IGESToBRep_CurveAndSurface_HeaderFile
