// Created on: 1995-12-15
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

#ifndef _BRepCheck_Face_HeaderFile
#define _BRepCheck_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepCheck_Status.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepCheck_Result.hxx>
class TopoDS_Face;
class TopoDS_Shape;

class BRepCheck_Face : public BRepCheck_Result
{

public:
  Standard_EXPORT BRepCheck_Face(const TopoDS_Face& F);

  Standard_EXPORT void InContext(const TopoDS_Shape& ContextShape) override;

  Standard_EXPORT void Minimum() override;

  Standard_EXPORT void Blind() override;

  Standard_EXPORT BRepCheck_Status IntersectWires(const bool Update = false);

  Standard_EXPORT BRepCheck_Status ClassifyWires(const bool Update = false);

  Standard_EXPORT BRepCheck_Status OrientationOfWires(const bool Update = false);

  Standard_EXPORT void SetUnorientable();

  //! Sets status of Face;
  Standard_EXPORT void SetStatus(const BRepCheck_Status theStatus);

  Standard_EXPORT bool IsUnorientable() const;

  Standard_EXPORT bool GeometricControls() const;

  Standard_EXPORT void GeometricControls(const bool B);

  DEFINE_STANDARD_RTTIEXT(BRepCheck_Face, BRepCheck_Result)

private:
  bool             myIntdone;
  BRepCheck_Status myIntres;
  bool             myImbdone;
  BRepCheck_Status myImbres;
  bool             myOridone;
  BRepCheck_Status myOrires;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
       myMapImb;
  bool myGctrl;
};

#endif // _BRepCheck_Face_HeaderFile
