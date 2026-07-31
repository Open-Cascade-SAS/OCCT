// Created on: 1999-01-13
// Created by: Xuan PHAM PHU
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_CLASSI_HeaderFile
#define _TopOpeBRepTool_CLASSI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box2d.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopOpeBRepTool_face.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
class TopoDS_Shape;
class Bnd_Box2d;
class TopOpeBRepTool_face;

class TopOpeBRepTool_CLASSI
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_CLASSI();

  Standard_EXPORT void Init2d(const TopoDS_Face& Fref);

  Standard_EXPORT bool HasInit2d() const;

  Standard_EXPORT bool Add2d(const TopoDS_Shape& S);

  Standard_EXPORT bool GetBox2d(const TopoDS_Shape& S, Bnd_Box2d& Box2d);

  Standard_EXPORT int ClassiBnd2d(const TopoDS_Shape& S1,
                                  const TopoDS_Shape& S2,
                                  const double        tol,
                                  const bool          checklarge);

  Standard_EXPORT int Classip2d(const TopoDS_Shape& S1,
                                const TopoDS_Shape& S2,
                                const int           stabnd2d12);

  Standard_EXPORT bool Getface(const TopoDS_Shape& S, TopOpeBRepTool_face& fa) const;

  Standard_EXPORT bool Classilist(
    const NCollection_List<TopoDS_Shape>& lS,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      mapgreasma);

private:
  TopoDS_Face                                                                     myFref;
  NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box2d>                             mymapsbox2d;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_face, TopTools_ShapeMapHasher> mymapsface;
};

#endif // _TopOpeBRepTool_CLASSI_HeaderFile
