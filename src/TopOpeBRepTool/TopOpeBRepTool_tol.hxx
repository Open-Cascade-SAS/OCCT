// Created on: 1997-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#ifndef _TopOpeBRepTool_tol_HeaderFile
#define _TopOpeBRepTool_tol_HeaderFile

#include <Bnd_Box.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopOpeBRepTool_define.hxx>

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
