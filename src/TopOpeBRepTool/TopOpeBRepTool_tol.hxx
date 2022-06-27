// Created on: 1997-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_tol_HeaderFile
#define _TopOpeBRepTool_tol_HeaderFile

#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>

Standard_EXPORT void FTOL_FaceTolerances
(const Bnd_Box& B1,const Bnd_Box& B2,
 const TopoDS_Face& myFace1,const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1,const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1,Standard_Real& myTol2,
 Standard_Real& Deflection,Standard_Real& MaxUV);

Standard_EXPORT void FTOL_FaceTolerances3d
(const TopoDS_Face& myFace1,const TopoDS_Face& myFace2,Standard_Real& Tol);

Standard_EXPORT void FTOL_FaceTolerances3d
(const Bnd_Box& B1,const Bnd_Box& B2,
 const TopoDS_Face& myFace1,const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1,const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1,Standard_Real& myTol2,
 Standard_Real& Deflection,Standard_Real& MaxUV);

Standard_EXPORT void FTOL_FaceTolerances2d
(const Bnd_Box& B1,const Bnd_Box& B2,
 const TopoDS_Face& myFace1,const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1,const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1,Standard_Real& myTol2);

#endif
