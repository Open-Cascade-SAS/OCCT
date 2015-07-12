// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_Vector_HeaderFile
#define _Graphic3d_Vector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Standard_Boolean.hxx>
class Graphic3d_VectorError;


//! This class allows the creation and update
//! of a 3D vector.
class Graphic3d_Vector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a vector with 1.0, 0.0, 0.0 coordinates.
  Standard_EXPORT Graphic3d_Vector();
  
  //! Creates a vector with <AX>, <AY>, <AZ> coordinates.
  Standard_EXPORT Graphic3d_Vector(const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ);
  
  //! Creates a vector from 2 points <APoint1> and <APoint2>.
  Standard_EXPORT Graphic3d_Vector(const Graphic3d_Vertex& APoint1, const Graphic3d_Vertex& APoint2);
  
  //! Normalises <me>.
  //! Category: Methods to modify the class definition
  //! Warning: Raises VectorError if <me> is null.
  Standard_EXPORT void Normalize();
  
  //! Modifies the coordinates of the vector <me>.
  Standard_EXPORT void SetCoord (const Standard_Real Xnew, const Standard_Real Ynew, const Standard_Real Znew);
  
  //! Modifies the X coordinate of the vector <me>.
  Standard_EXPORT void SetXCoord (const Standard_Real Xnew);
  
  //! Modifies the Y coordinate of the vector <me>.
  Standard_EXPORT void SetYCoord (const Standard_Real Ynew);
  
  //! Modifies the Z coordinate of the vector <me>.
  Standard_EXPORT void SetZCoord (const Standard_Real Znew);
  
  //! Returns the coordinates of the vector <me>.
  Standard_EXPORT void Coord (Standard_Real& AX, Standard_Real& AY, Standard_Real& AZ) const;
  
  //! Returns Standard_True if <me> has length 1.
  Standard_EXPORT Standard_Boolean IsNormalized() const;
  
  //! Returns Standard_True if <me> has length zero.
  Standard_EXPORT Standard_Boolean LengthZero() const;
  
  //! Returns the X coordinates of the vector <me>.
  Standard_EXPORT Standard_Real X() const;
  
  //! Returns the Y coordinate of the vector <me>.
  Standard_EXPORT Standard_Real Y() const;
  
  //! Returns the Z coordinate of the vector <me>.
  Standard_EXPORT Standard_Real Z() const;
  
  //! Returns Standard_True if the vector <AV1> and
  //! <AV2> are parallel.
  Standard_EXPORT static Standard_Boolean IsParallel (const Graphic3d_Vector& AV1, const Graphic3d_Vector& AV2);
  
  //! Returns the norm of the vector <AX>, <AY>, <AZ>.
  Standard_EXPORT static Standard_Real NormeOf (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ);
  
  //! Returns the norm of the vector <AVector>.
  Standard_EXPORT static Standard_Real NormeOf (const Graphic3d_Vector& AVector);




protected:





private:



  Standard_ShortReal MyX;
  Standard_ShortReal MyY;
  Standard_ShortReal MyZ;
  Standard_ShortReal MyNorme;


};







#endif // _Graphic3d_Vector_HeaderFile
