// File:	TopOpeBRepTool_tol.hxx
// Created:	Tue Apr  1 11:22:56 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

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
