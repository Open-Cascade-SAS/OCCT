// Created on: 1992-11-05
// Created by: Remi LEQUETTE
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

#ifndef _BRepPrim_Cone_HeaderFile
#define _BRepPrim_Cone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Revolution.hxx>
class gp_Ax2;
class gp_Pnt;
class TopoDS_Face;


//! Implement the cone primitive.
class BRepPrim_Cone  : public BRepPrim_Revolution
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! the STEP definition
  //! Angle = semi-angle of the cone
  //! Position : the coordinate system
  //! Height : height of the cone.
  //! Radius : radius of truncated face at z = 0
  //!
  //! The apex is on z < 0
  //!
  //! Errors : Height < Resolution
  //! Angle < Resolution / Height
  //! Angle > PI/2 - Resolution / Height
  Standard_EXPORT BRepPrim_Cone(const Standard_Real Angle, const gp_Ax2& Position, const Standard_Real Height, const Standard_Real Radius = 0);
  
  //! infinite cone at origin on Z negative
  Standard_EXPORT BRepPrim_Cone(const Standard_Real Angle);
  
  //! infinite cone at Apex on Z negative
  Standard_EXPORT BRepPrim_Cone(const Standard_Real Angle, const gp_Pnt& Apex);
  
  //! infinite cone with Axes
  Standard_EXPORT BRepPrim_Cone(const Standard_Real Angle, const gp_Ax2& Axes);
  
  //! create a  Cone at origin  on Z axis, of height  H,
  //! radius R1 at Z = 0, R2 at  Z = H, X is  the origin
  //! of angles.  If R1 or  R2 is 0   there is  an apex.
  //! Otherwise, it is a truncated cone.
  //!
  //! Error  : R1 and R2  < Resolution
  //! R1 or R2 negative
  //! Abs(R1-R2) < Resolution
  //! H < Resolution
  //! H negative
  Standard_EXPORT BRepPrim_Cone(const Standard_Real R1, const Standard_Real R2, const Standard_Real H);
  
  //! same as above but at a given point
  Standard_EXPORT BRepPrim_Cone(const gp_Pnt& Center, const Standard_Real R1, const Standard_Real R2, const Standard_Real H);
  
  //! same as above with given axes system.
  Standard_EXPORT BRepPrim_Cone(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real H);
  
  //! The surface normal should be directed  towards the
  //! outside.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const Standard_OVERRIDE;




protected:





private:

  
  Standard_EXPORT void SetMeridian();
  
  Standard_EXPORT void SetParameters (const Standard_Real R1, const Standard_Real R2, const Standard_Real H);


  Standard_Real myHalfAngle;
  Standard_Real myRadius;


};







#endif // _BRepPrim_Cone_HeaderFile
