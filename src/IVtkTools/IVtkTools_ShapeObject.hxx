// Created on: 2011-10-27 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#ifndef __IVTKTOOLS_SHAPEOBJECT_H__
#define __IVTKTOOLS_SHAPEOBJECT_H__

#include <IVtkTools.hxx>
#include <IVtkOCC_Shape.hxx>
#include <vtkDataObject.h>
#include <vtkSetGet.h>
#include <vtkWeakPointer.h>

class vtkActor;
class vtkDataSet;
class vtkInformationObjectBaseKey;
class IVtkTools_ShapeDataSource;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // avoid warning C4251: "class needs to have dll-interface..."
#endif

//! @class IVtkTools_ShapeObject
//! @brief VTK holder class for OCC shapes to pass them through pipelines.
//!
//! It is descendent of vtkObject (data). Logically it is a one of milestones of VTK pipeline.
//! It stores data of OCC shape (the OccShape instance) in vtkInformation object of vtkDataObject.
//! Then pass it to the actors through pipelines,
//! so selection logic can access OccShape easily given the actor instance.
class IVtkTools_EXPORT IVtkTools_ShapeObject :  public vtkDataObject
{
public:
  vtkTypeMacro (IVtkTools_ShapeObject, vtkObject)

  static IVtkTools_ShapeObject* New(); 

  //! Get OCC shape source from VTK data from actor's information object by key.
  static vtkSmartPointer<IVtkTools_ShapeDataSource> GetShapeSource (vtkActor* theActor);

  //! Get OCC shape from VTK data from actor's information object by key.
  static IVtkOCC_Shape::Handle GetOccShape (vtkActor* theActor);

  //! Static method to set OCC shape source to VTK dataset in information object 
  //! with key.
  static void SetShapeSource (IVtkTools_ShapeDataSource* theDataSource, vtkDataSet* theData);

  //! Static method to set OCC shape source to VTK actor in information object 
  //! with key.
  static void SetShapeSource (IVtkTools_ShapeDataSource* theDataSource, vtkActor* theActor);

  typedef vtkInformationObjectBaseKey* KeyPtr;

  //! Static method used by shape selection logic in order to establish
  //! a connection from vtkActor to OccShape instance.
  //! @return vtkInformationKey for retrieving OccShape instance from the actor
  static KeyPtr getKey();

  //! OCC shape source setter.
  void SetShapeSource (IVtkTools_ShapeDataSource* theDataSource);

  //! OCC shape source getter.
  IVtkTools_ShapeDataSource* GetShapeSource() const;

protected:
  IVtkTools_ShapeObject();
  virtual ~IVtkTools_ShapeObject();

private: // not copyable
  IVtkTools_ShapeObject (const IVtkTools_ShapeObject&);
  IVtkTools_ShapeObject& operator= (const IVtkTools_ShapeObject&);

private: // OCC
  vtkWeakPointer<IVtkTools_ShapeDataSource> myShapeSource;

  static KeyPtr myKey;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // __IVTKTOOLS_SHAPEOBJECT_H__
