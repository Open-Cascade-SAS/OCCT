// Created on: 2012-06-27
// Created by: Dmitry BOBYLEV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _TopTools_MutexForShapeProvider_HeaderFile
#define _TopTools_MutexForShapeProvider_HeaderFile

#include <TopoDS_TShape.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_ShapeEnum.hxx>

class Standard_Mutex;
class TopoDS_Shape;

//! Class TopTools_MutexForShapeProvider
//!   This class is used to create and store mutexes associated with shapes.
class TopTools_MutexForShapeProvider
{
public:
  //! Constructor
  Standard_EXPORT TopTools_MutexForShapeProvider();

  //! Destructor
  Standard_EXPORT ~TopTools_MutexForShapeProvider();

  //! Creates and associates mutexes with each sub-shape of type theType in theShape.
  Standard_EXPORT void CreateMutexesForSubShapes(const TopoDS_Shape&    theShape,
                                                 const TopAbs_ShapeEnum theType);

  //! Creates and associates mutex with theShape
  Standard_EXPORT void CreateMutexForShape(const TopoDS_Shape& theShape);

  //! Returns pointer to mutex associated with theShape.
  //! In case when mutex not found returns NULL.
  Standard_EXPORT Standard_Mutex* GetMutex(const TopoDS_Shape& theShape) const;

  //! Removes all mutexes
  Standard_EXPORT void RemoveAllMutexes();

private:
  //! This method should not be called (prohibited).
  TopTools_MutexForShapeProvider(const TopTools_MutexForShapeProvider&);
  //! This method should not be called (prohibited).
  TopTools_MutexForShapeProvider& operator=(const TopTools_MutexForShapeProvider&);

  NCollection_DataMap<Handle(TopoDS_TShape), Standard_Mutex*> myMap;
};

#endif
