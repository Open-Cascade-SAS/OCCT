// Created on: 1993-12-23
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _ChFiKPart_ComputeData_HeaderFile
#define _ChFiKPart_ComputeData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class TopOpeBRepDS_DataStructure;
class ChFiDS_SurfData;
class Adaptor3d_HSurface;
class ChFiDS_Spine;
class gp_Pnt2d;


//! Methodes de classe   permettant de  remplir    une
//! SurfData dans  les cas  particuliers  de  conges
//! suivants:
//! - cylindre entre 2 surfaces planes,
//! - tore/sphere entre un plan et un cylindre othogonal,
//! - tore/sphere entre un plan et un cone othogonal,
//!
//! - tore entre un plan et une droite orthogonale (rotule).
class ChFiKPart_ComputeData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes a simple fillet in several particular
  //! cases.
  Standard_EXPORT static Standard_Boolean Compute (TopOpeBRepDS_DataStructure& DStr, Handle(ChFiDS_SurfData)& Data, const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const TopAbs_Orientation Or1, const TopAbs_Orientation Or2, const Handle(ChFiDS_Spine)& Sp, const Standard_Integer Iedge);
  
  //! Computes a toric or spheric corner fillet.
  Standard_EXPORT static Standard_Boolean ComputeCorner (TopOpeBRepDS_DataStructure& DStr, const Handle(ChFiDS_SurfData)& Data, const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const TopAbs_Orientation OrFace1, const TopAbs_Orientation OrFace2, const TopAbs_Orientation Or1, const TopAbs_Orientation Or2, const Standard_Real minRad, const Standard_Real majRad, const gp_Pnt2d& P1S1, const gp_Pnt2d& P2S1, const gp_Pnt2d& P1S2, const gp_Pnt2d& P2S2);
  
  //! Computes spheric corner fillet with non iso pcurve on S2.
  Standard_EXPORT static Standard_Boolean ComputeCorner (TopOpeBRepDS_DataStructure& DStr, const Handle(ChFiDS_SurfData)& Data, const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const TopAbs_Orientation OrFace1, const TopAbs_Orientation OrFace2, const TopAbs_Orientation Or1, const TopAbs_Orientation Or2, const Standard_Real Rad, const gp_Pnt2d& PS1, const gp_Pnt2d& P1S2, const gp_Pnt2d& P2S2);
  
  //! Computes a toric corner rotule.
  Standard_EXPORT static Standard_Boolean ComputeCorner (TopOpeBRepDS_DataStructure& DStr, const Handle(ChFiDS_SurfData)& Data, const Handle(Adaptor3d_HSurface)& S, const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2, const TopAbs_Orientation OfS, const TopAbs_Orientation OS, const TopAbs_Orientation OS1, const TopAbs_Orientation OS2, const Standard_Real Radius);




protected:





private:





};







#endif // _ChFiKPart_ComputeData_HeaderFile
