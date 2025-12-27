// Created on: 1999-03-22
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

#ifndef _TopOpeBRepTool_mkTondgE_HeaderFile
#define _TopOpeBRepTool_mkTondgE_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

class TopOpeBRepTool_mkTondgE
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_mkTondgE();

  Standard_EXPORT bool Initialize(const TopoDS_Edge& dgE,
                                              const TopoDS_Face& F,
                                              const gp_Pnt2d&    uvi,
                                              const TopoDS_Face& Fi);

  Standard_EXPORT bool SetclE(const TopoDS_Edge& clE);

  Standard_EXPORT bool IsT2d() const;

  Standard_EXPORT bool SetRest(const double pari, const TopoDS_Edge& Ei);

  Standard_EXPORT int GetAllRest(NCollection_List<TopoDS_Shape>& lEi);

  Standard_EXPORT bool MkTonE(int& mkT,
                                          double&    par1,
                                          double&    par2);

  Standard_EXPORT bool MkTonE(const TopoDS_Edge& Ei,
                                          int&  mkT,
                                          double&     par1,
                                          double&     par2);

private:
  TopoDS_Edge                 mydgE;
  TopoDS_Face                 myF;
  TopoDS_Edge                 myclE;
  gp_Dir                      mydirINcle;
  TopoDS_Face                 myFi;
  gp_Pnt2d                    myuvi;
  bool            isT2d;
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> myEpari;
  bool            hasRest;
  gp_Dir                      myngf;
  gp_Dir                      myngfi;
};

#endif // _TopOpeBRepTool_mkTondgE_HeaderFile
