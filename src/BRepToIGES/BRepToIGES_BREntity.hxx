// Created on: 1994-11-15
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

#ifndef _BRepToIGES_BREntity_HeaderFile
#define _BRepToIGES_BREntity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Message_ProgressRange.hxx>

class IGESData_IGESModel;
class Transfer_FinderProcess;
class IGESData_IGESEntity;
class TopoDS_Shape;
class Standard_Transient;


//! provides methods to transfer BRep entity from CASCADE to IGES.
class BRepToIGES_BREntity 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a tool BREntity
  Standard_EXPORT BRepToIGES_BREntity();
  
  //! Initializes the field of the tool BREntity with
  //! default creating values.
  Standard_EXPORT void Init();
  
  //! Set the value of "TheModel"
  Standard_EXPORT void SetModel (const Handle(IGESData_IGESModel)& model);
  
  //! Returns the value of "TheModel"
  Standard_EXPORT Handle(IGESData_IGESModel) GetModel() const;
  
  //! Returns the value of the UnitFlag of the header of the model
  //! in meters.
  Standard_EXPORT Standard_Real GetUnit() const;
  
  //! Set the value of "TheMap"
  Standard_EXPORT void SetTransferProcess (const Handle(Transfer_FinderProcess)& TP);
  
  //! Returns the value of "TheMap"
  Standard_EXPORT Handle(Transfer_FinderProcess) GetTransferProcess() const;
  
  //! Returns the result of the transfert of any Shape
  //! If  the transfer has  failed, this member return a NullEntity.
  Standard_EXPORT virtual Handle(IGESData_IGESEntity) TransferShape
                   (const TopoDS_Shape& start,
                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Records a new Fail message
  Standard_EXPORT void AddFail (const TopoDS_Shape& start, const Standard_CString amess);
  
  //! Records a new Warning message
  Standard_EXPORT void AddWarning (const TopoDS_Shape& start, const Standard_CString amess);
  
  //! Records a new Fail message
  Standard_EXPORT void AddFail (const Handle(Standard_Transient)& start, const Standard_CString amess);
  
  //! Records a new Warning message
  Standard_EXPORT void AddWarning (const Handle(Standard_Transient)& start, const Standard_CString amess);
  
  //! Returns True if start was already treated and has a result in "TheMap"
  //! else returns False.
  Standard_EXPORT Standard_Boolean HasShapeResult (const TopoDS_Shape& start) const;
  
  //! Returns the result of the transfer of the Shape "start" contained
  //! in "TheMap" . (if HasShapeResult is True).
  Standard_EXPORT Handle(Standard_Transient) GetShapeResult (const TopoDS_Shape& start) const;
  
  //! set in "TheMap" the result of the transfer of the Shape "start".
  Standard_EXPORT void SetShapeResult (const TopoDS_Shape& start, const Handle(Standard_Transient)& result);
  
  //! Returns True if start was already treated and has a result in "TheMap"
  //! else returns False.
  Standard_EXPORT Standard_Boolean HasShapeResult (const Handle(Standard_Transient)& start) const;
  
  //! Returns the result of the transfer of the Transient "start" contained
  //! in "TheMap" . (if HasShapeResult is True).
  Standard_EXPORT Handle(Standard_Transient) GetShapeResult (const Handle(Standard_Transient)& start) const;
  
  //! set in "TheMap" the result of the transfer of the Transient "start".
  Standard_EXPORT void SetShapeResult (const Handle(Standard_Transient)& start, const Handle(Standard_Transient)& result);
  
  //! Returns mode for conversion of surfaces
  //! (value of parameter write.convertsurface.mode)
  Standard_EXPORT Standard_Boolean GetConvertSurfaceMode() const;
  
  //! Returns mode for writing pcurves
  //! (value of parameter write.surfacecurve.mode)
  Standard_EXPORT Standard_Boolean GetPCurveMode() const;
  
  Standard_EXPORT virtual ~BRepToIGES_BREntity();

private:
  Handle(IGESData_IGESModel) TheModel;
  Standard_Real TheUnitFactor;
  Standard_Boolean myConvSurface;
  Standard_Boolean myPCurveMode;
  Handle(Transfer_FinderProcess) TheMap;
};

#endif // _BRepToIGES_BREntity_HeaderFile
