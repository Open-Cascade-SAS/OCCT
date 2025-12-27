// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepCheck_Result_HeaderFile
#define _BRepCheck_Result_HeaderFile

#include <Standard.hxx>

#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <Standard_MemoryUtils.hxx>

#include <mutex>

class BRepCheck_Result : public Standard_Transient
{

public:
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT virtual void InContext(const TopoDS_Shape& ContextShape) = 0;

  Standard_EXPORT virtual void Minimum() = 0;

  Standard_EXPORT virtual void Blind() = 0;

  Standard_EXPORT void SetFailStatus(const TopoDS_Shape& S);

  const NCollection_List<BRepCheck_Status>& Status() const { return *myMap(myShape); }

  bool IsMinimum() const { return myMin; }

  bool IsBlind() const { return myBlind; }

  Standard_EXPORT void InitContextIterator();

  bool MoreShapeInContext() const { return myIter.More(); }

  const TopoDS_Shape& ContextualShape() const { return myIter.Key(); }

  const NCollection_List<BRepCheck_Status>& StatusOnShape() const { return *myIter.Value(); }

  Standard_EXPORT void NextShapeInContext();

  Standard_EXPORT void SetParallel(bool theIsParallel);

  bool IsStatusOnShape(const TopoDS_Shape& theShape) const
  {
    return myMap.IsBound(theShape);
  }

  const NCollection_List<BRepCheck_Status>& StatusOnShape(const TopoDS_Shape& theShape) const
  {
    return *myMap.Find(theShape);
  }

  friend class BRepCheck_ParallelAnalyzer;

  DEFINE_STANDARD_RTTIEXT(BRepCheck_Result, Standard_Transient)

protected:
  Standard_EXPORT BRepCheck_Result();

protected:
  TopoDS_Shape                         myShape;
  bool                     myMin;
  bool                     myBlind;
  NCollection_DataMap<TopoDS_Shape, Handle(NCollection_Shared<NCollection_List<BRepCheck_Status>>), TopTools_ShapeMapHasher> myMap;
  mutable std::unique_ptr<std::mutex>  myMutex;

private:
  std::unique_ptr<std::mutex>& GetMutex() { return myMutex; }

private:
  NCollection_DataMap<TopoDS_Shape, Handle(NCollection_Shared<NCollection_List<BRepCheck_Status>>), TopTools_ShapeMapHasher>::Iterator myIter;
};

#endif // _BRepCheck_Result_HeaderFile
