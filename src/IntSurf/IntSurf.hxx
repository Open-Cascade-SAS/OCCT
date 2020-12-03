// Created on: 1992-08-24
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_HeaderFile
#define _IntSurf_HeaderFile

#include <Adaptor3d_Surface.hxx>

class IntSurf_Transition;
class gp_Dir;
class gp_Vec;

//! This package provides resources for
//! all the packages concerning the intersection
//! between surfaces.
class IntSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes the transition of the intersection point
  //! between the two lines.
  //! TgFirst is the tangent vector of the first line.
  //! TgSecond is the tangent vector of the second line.
  //! Normal is the direction used to orientate the cross
  //! product TgFirst^TgSecond.
  //! TFirst is the transition of the point on the first line.
  //! TSecond is the transition of the point on the second line.
  Standard_EXPORT static void MakeTransition (const gp_Vec& TgFirst, const gp_Vec& TgSecond, const gp_Dir& Normal, IntSurf_Transition& TFirst, IntSurf_Transition& TSecond);

  //! Fills theArrOfPeriod array by the period values of theFirstSurf and theSecondSurf.
  //! [0] = U-period of theFirstSurf,
  //! [1] = V-period of theFirstSurf,
  //! [2] = U-period of theSecondSurf,
  //! [3] = V-period of theSecondSurf.
  //!
  //! If surface is not periodic in correspond direction then
  //! its period is considered to be equal to 0.
  Standard_EXPORT static void SetPeriod(const Handle(Adaptor3d_Surface)& theFirstSurf,
                                        const Handle(Adaptor3d_Surface)& theSecondSurf,
                                        Standard_Real theArrOfPeriod[4]);



protected:





private:




friend class IntSurf_PntOn2S;
friend class IntSurf_Couple;
friend class IntSurf_LineOn2S;
friend class IntSurf_Quadric;
friend class IntSurf_QuadricTool;
friend class IntSurf_PathPoint;
friend class IntSurf_PathPointTool;
friend class IntSurf_InteriorPoint;
friend class IntSurf_InteriorPointTool;
friend class IntSurf_Transition;

};







#endif // _IntSurf_HeaderFile
