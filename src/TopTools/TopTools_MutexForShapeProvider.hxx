// Created on: 2012-06-27
// Created by: Dmitry BOBYLEV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef _TopTools_MutexForShapeProvider_HeaderFile
#define _TopTools_MutexForShapeProvider_HeaderFile

#include <Handle_TopoDS_TShape.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_ShapeEnum.hxx>

class Standard_Mutex;
class TopoDS_Shape;

//! Class TopTools_MutexForShapeProvider 
//!   This class is used to create and store mutexes associated with shapes.
class TopTools_MutexForShapeProvider
{
  friend Standard_Boolean IsEqual(const Handle_TopoDS_TShape & theFirstHandle,
                                  const Handle_TopoDS_TShape & theSecondHandle);

public:
  //! Constructor
  Standard_EXPORT TopTools_MutexForShapeProvider();

  //! Destructor
  Standard_EXPORT ~TopTools_MutexForShapeProvider();

  //! Creates and associates mutexes with each sub-shape of type theType in theShape.
  Standard_EXPORT void CreateMutexesForSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType);

  //! Creates and associates mutex with theShape
  Standard_EXPORT void CreateMutexForShape(const TopoDS_Shape& theShape);

  //! Returns pointer to mutex associated with theShape.
  //! In case when mutex not found returns NULL.
  Standard_EXPORT Standard_Mutex* GetMutex(const TopoDS_Shape& theShape) const;

  //! Removes all mutexes
  Standard_EXPORT void RemoveAllMutexes();

private:
  //! This method should not be called (prohibited).
  TopTools_MutexForShapeProvider (const TopTools_MutexForShapeProvider &);
  //! This method should not be called (prohibited).
  TopTools_MutexForShapeProvider & operator = (const TopTools_MutexForShapeProvider &);


  NCollection_DataMap<Handle_TopoDS_TShape, Standard_Mutex *> myMap;

};

inline Standard_Boolean IsEqual(const Handle_TopoDS_TShape & theFirstHandle,
                                const Handle_TopoDS_TShape & theSecondHandle)
{
  return (theFirstHandle == theSecondHandle);
}

#endif