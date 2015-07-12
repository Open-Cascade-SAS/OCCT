// Created on: 1996-07-26
// Created by: Maria PUMBORIOS
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

#ifndef _FilletSurf_Builder_HeaderFile
#define _FilletSurf_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <FilletSurf_InternalBuilder.hxx>
#include <FilletSurf_StatusDone.hxx>
#include <FilletSurf_ErrorTypeStatus.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <FilletSurf_StatusType.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class TopoDS_Shape;
class Geom_Surface;
class TopoDS_Face;
class Geom_Curve;
class Geom2d_Curve;
class Geom_TrimmedCurve;


//! API giving the  following  geometric information about fillets
//! list of corresponding NUBS surfaces
//! for each surface:
//! the 2  support faces
//! on each face: the 3d curve and the corresponding 2d curve
//! the 2d curves on the fillet
//! status of start and end section of the fillet
//! first and last parameter on edge of the fillet.
class FilletSurf_Builder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! initialize  of the informations necessary for  the
  //! computation of  the fillet on the
  //! Shape S from a list of edges E and a radius R.
  //!
  //! Ta is the angular tolerance
  //! Tapp3d is the 3d approximation tolerance
  //! Tapp2d is the 2d approximation tolerance
  Standard_EXPORT FilletSurf_Builder(const TopoDS_Shape& S, const TopTools_ListOfShape& E, const Standard_Real R, const Standard_Real Ta = 1.0e-2, const Standard_Real Tapp3d = 1.0e-4, const Standard_Real Tapp2d = 1.0e-5);
  
  //! ---Purpose computation  of the fillet (list of NUBS)
  Standard_EXPORT void Perform();
  
  Standard_EXPORT void Simulate();
  
  //! gives the status about the computation of the fillet
  //! returns:
  //! IsOK :no problem during the computation
  //! IsNotOk: no result is produced
  //! IsPartial: the result is partial
  Standard_EXPORT FilletSurf_StatusDone IsDone() const;
  
  //! gives    informations     about  error   status     if
  //! IsDone=IsNotOk
  //! returns
  //! EdgeNotG1: the edges are not G1
  //! FacesNotG1 : two connected faces on a same support are
  //! not  G1
  //! EdgeNotOnShape: the  edge   is  not on  shape
  //! NotSharpEdge: the  edge is not sharp
  //! PbFilletCompute: problem during the computation of the fillet
  Standard_EXPORT FilletSurf_ErrorTypeStatus StatusError() const;
  
  //! gives the number of NUBS surfaces  of the Fillet.
  Standard_EXPORT Standard_Integer NbSurface() const;
  
  //! gives the NUBS surface of index Index.
  Standard_EXPORT const Handle(Geom_Surface)& SurfaceFillet (const Standard_Integer Index) const;
  
  //! gives  the  3d  tolerance reached during approximation
  //! of surface of index Index
  Standard_EXPORT Standard_Real TolApp3d (const Standard_Integer Index) const;
  
  //! gives the first support  face relative to SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Face& SupportFace1 (const Standard_Integer Index) const;
  
  //! gives the second support  face relative to SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Face& SupportFace2 (const Standard_Integer Index) const;
  
  //! gives  the 3d curve  of SurfaceFillet(Index)  on SupportFace1(Index)
  Standard_EXPORT const Handle(Geom_Curve)& CurveOnFace1 (const Standard_Integer Index) const;
  
  //! gives the     3d  curve of  SurfaceFillet(Index) on SupportFace2(Index)
  Standard_EXPORT const Handle(Geom_Curve)& CurveOnFace2 (const Standard_Integer Index) const;
  
  //! gives the  PCurve associated to CurvOnSup1(Index)  on the support face
  Standard_EXPORT const Handle(Geom2d_Curve)& PCurveOnFace1 (const Standard_Integer Index) const;
  
  //! gives the PCurve associated to CurveOnFace1(Index) on the Fillet
  Standard_EXPORT const Handle(Geom2d_Curve)& PCurve1OnFillet (const Standard_Integer Index) const;
  
  //! gives the PCurve  associated to CurveOnSup2(Index) on  the  support face
  Standard_EXPORT const Handle(Geom2d_Curve)& PCurveOnFace2 (const Standard_Integer Index) const;
  
  //! gives the PCurve  associated to CurveOnSup2(Index) on  the  fillet
  Standard_EXPORT const Handle(Geom2d_Curve)& PCurve2OnFillet (const Standard_Integer Index) const;
  
  //! gives the parameter of the fillet  on the first edge.
  Standard_EXPORT Standard_Real FirstParameter() const;
  
  //! gives the  parameter of the fillet  on the last edge
  Standard_EXPORT Standard_Real LastParameter() const;
  
  Standard_EXPORT FilletSurf_StatusType StartSectionStatus() const;
  
  Standard_EXPORT FilletSurf_StatusType EndSectionStatus() const;
  
  Standard_EXPORT Standard_Integer NbSection (const Standard_Integer IndexSurf) const;
  
  Standard_EXPORT void Section (const Standard_Integer IndexSurf, const Standard_Integer IndexSec, Handle(Geom_TrimmedCurve)& Circ) const;




protected:





private:



  FilletSurf_InternalBuilder myIntBuild;
  FilletSurf_StatusDone myisdone;
  FilletSurf_ErrorTypeStatus myerrorstatus;


};







#endif // _FilletSurf_Builder_HeaderFile
