// Created on: 1996-01-30
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _LocOpe_Gluer_HeaderFile
#define _LocOpe_Gluer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopAbs_Orientation.hxx>
#include <LocOpe_Operation.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Face;
class TopoDS_Edge;

class LocOpe_Gluer
{
public:
  DEFINE_STANDARD_ALLOC

  LocOpe_Gluer();

  LocOpe_Gluer(const TopoDS_Shape& Sbase, const TopoDS_Shape& Snew);

  Standard_EXPORT void Init(const TopoDS_Shape& Sbase, const TopoDS_Shape& Snew);

  Standard_EXPORT void Bind(const TopoDS_Face& Fnew, const TopoDS_Face& Fbase);

  Standard_EXPORT void Bind(const TopoDS_Edge& Enew, const TopoDS_Edge& Ebase);

  LocOpe_Operation OpeType() const;

  Standard_EXPORT void Perform();

  Standard_Boolean IsDone() const;

  const TopoDS_Shape& ResultingShape() const;

  Standard_EXPORT const TopTools_ListOfShape& DescendantFaces(const TopoDS_Face& F) const;

  const TopoDS_Shape& BasisShape() const;

  const TopoDS_Shape& GluedShape() const;

  const TopTools_ListOfShape& Edges() const;

  const TopTools_ListOfShape& TgtEdges() const;

protected:
private:
  Standard_EXPORT void AddEdges();

  Standard_Boolean                    myDone;
  TopoDS_Shape                        mySb;
  TopoDS_Shape                        mySn;
  TopoDS_Shape                        myRes;
  TopAbs_Orientation                  myOri;
  LocOpe_Operation                    myOpe;
  TopTools_IndexedDataMapOfShapeShape myMapEF;
  TopTools_DataMapOfShapeShape        myMapEE;
  TopTools_DataMapOfShapeListOfShape  myDescF;
  TopTools_ListOfShape                myEdges;
  TopTools_ListOfShape                myTgtEdges;
};

#include <LocOpe_Gluer.lxx>

#endif // _LocOpe_Gluer_HeaderFile
