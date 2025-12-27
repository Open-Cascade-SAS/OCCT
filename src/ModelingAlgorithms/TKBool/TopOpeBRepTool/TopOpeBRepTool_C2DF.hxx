// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_C2DF_HeaderFile
#define _TopOpeBRepTool_C2DF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
class Geom2d_Curve;

class TopOpeBRepTool_C2DF
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_C2DF();

  Standard_EXPORT TopOpeBRepTool_C2DF(const occ::handle<Geom2d_Curve>& PC,
                                      const double         f2d,
                                      const double         l2d,
                                      const double         tol,
                                      const TopoDS_Face&          F);

  Standard_EXPORT void SetPC(const occ::handle<Geom2d_Curve>& PC,
                             const double         f2d,
                             const double         l2d,
                             const double         tol);

  Standard_EXPORT void SetFace(const TopoDS_Face& F);

  Standard_EXPORT const occ::handle<Geom2d_Curve>& PC(double& f2d,
                                                 double& l2d,
                                                 double& tol) const;

  Standard_EXPORT const TopoDS_Face& Face() const;

  Standard_EXPORT bool IsPC(const occ::handle<Geom2d_Curve>& PC) const;

  Standard_EXPORT bool IsFace(const TopoDS_Face& F) const;

private:
  occ::handle<Geom2d_Curve> myPC;
  double        myf2d;
  double        myl2d;
  double        mytol;
  TopoDS_Face          myFace;
};

#endif // _TopOpeBRepTool_C2DF_HeaderFile
