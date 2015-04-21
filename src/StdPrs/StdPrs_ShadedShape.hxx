// Created on: 1993-09-23
// Created by: Jean Louis FRENKEL
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

#ifndef _StdPrs_ShadedShape_HeaderFile
#define _StdPrs_ShadedShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <StdPrs_Volume.hxx>
#include <Standard_Boolean.hxx>
class Prs3d_Presentation;
class TopoDS_Shape;
class gp_Pnt2d;
class BRep_Builder;
class TopoDS_Compound;


//! Auxiliary procedures to prepare Shaded presentation of specified shape.
class StdPrs_ShadedShape  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Shades <theShape>.
  //! @param theVolumeType defines the way how to interpret input shapes - as Closed volumes (to activate back-face
  //! culling and capping plane algorithms), as Open volumes (shells or solids with holes)
  //! or to perform Autodetection (would split input shape into two groups)
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& thePresentation, const TopoDS_Shape& theShape, const Handle(Prs3d_Drawer)& theDrawer, const StdPrs_Volume theVolume = StdPrs_Volume_Autodetection);
  
  //! Shades <theShape> with texture coordinates.
  //! @param theVolumeType defines the way how to interpret input shapes - as Closed volumes (to activate back-face
  //! culling and capping plane algorithms), as Open volumes (shells or solids with holes)
  //! or to perform Autodetection (would split input shape into two groups)
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& thePresentation, const TopoDS_Shape& theShape, const Handle(Prs3d_Drawer)& theDrawer, const Standard_Boolean theHasTexels, const gp_Pnt2d& theUVOrigin, const gp_Pnt2d& theUVRepeat, const gp_Pnt2d& theUVScale, const StdPrs_Volume theVolume = StdPrs_Volume_Autodetection);
  
  //! Searches closed and unclosed subshapes in shape structure and puts them
  //! into two compounds for separate processing of closed and unclosed sub-shapes
  Standard_EXPORT static void ExploreSolids (const TopoDS_Shape& theShape, const BRep_Builder& theBuilder, TopoDS_Compound& theClosed, TopoDS_Compound& theOpened, const Standard_Boolean theIgnore1DSubShape);




protected:





private:





};







#endif // _StdPrs_ShadedShape_HeaderFile
