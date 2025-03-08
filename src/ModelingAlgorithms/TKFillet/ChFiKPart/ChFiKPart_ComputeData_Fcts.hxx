// Created on: 1995-05-22
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiKPart_ComputeData_Fcts_HeaderFile
#define _ChFiKPart_ComputeData_Fcts_HeaderFile

#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

Standard_Real ChFiKPart_InPeriod(const Standard_Real U,
                                 const Standard_Real UFirst,
                                 const Standard_Real ULast,
                                 const Standard_Real Eps);

Handle(Geom2d_BSplineCurve) ChFiKPart_PCurve(const gp_Pnt2d&     UV1,
                                             const gp_Pnt2d&     UV2,
                                             const Standard_Real Pardeb,
                                             const Standard_Real Parfin);

void ChFiKPart_ProjPC(const GeomAdaptor_Curve&   Cg,
                      const GeomAdaptor_Surface& Sg,
                      Handle(Geom2d_Curve)&      Pcurv);

Standard_EXPORT Standard_Integer ChFiKPart_IndexCurveInDS(const Handle(Geom_Curve)&   C,
                                                          TopOpeBRepDS_DataStructure& DStr);

Standard_EXPORT Standard_Integer ChFiKPart_IndexSurfaceInDS(const Handle(Geom_Surface)& S,
                                                            TopOpeBRepDS_DataStructure& DStr);

#endif
