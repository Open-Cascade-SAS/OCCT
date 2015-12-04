// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_ConeFrustum_HeaderFile
#define _IGESSolid_ConeFrustum_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_XYZ;
class gp_Pnt;
class gp_Dir;


class IGESSolid_ConeFrustum;
DEFINE_STANDARD_HANDLE(IGESSolid_ConeFrustum, IGESData_IGESEntity)

//! defines ConeFrustum, Type <156> Form Number <0>
//! in package IGESSolid
//! The Cone Frustum is defined by the center of the
//! larger circular face of the frustum, its radius, a unit
//! vector in the axis direction, a height in this direction
//! and a second circular face with radius which is lesser
//! than the first face.
class IGESSolid_ConeFrustum : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_ConeFrustum();
  
  //! This method is used to set the fields of the class
  //! ConeFrustum
  //! - Ht     : the Height of cone
  //! - R1     : Radius of the larger face
  //! - R2     : Radius of the smaller face (default 0)
  //! - Center : Center of the larger face (default (0,0,0))
  //! - anAxis : Unit vector in axis direction (default (0,0,1))
  Standard_EXPORT void Init (const Standard_Real Ht, const Standard_Real R1, const Standard_Real R2, const gp_XYZ& Center, const gp_XYZ& anAxis);
  
  //! returns the height of the cone frustum
  Standard_EXPORT Standard_Real Height() const;
  
  //! returns the radius of the larger face of the cone frustum
  Standard_EXPORT Standard_Real LargerRadius() const;
  
  //! returns the radius of the second face of the cone frustum
  Standard_EXPORT Standard_Real SmallerRadius() const;
  
  //! returns the center of the larger face of the cone frustum
  Standard_EXPORT gp_Pnt FaceCenter() const;
  
  //! returns the center of the larger face of the cone frustum
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedFaceCenter() const;
  
  //! returns the direction of the axis of the cone frustum
  Standard_EXPORT gp_Dir Axis() const;
  
  //! returns the direction of the axis of the cone frustum
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedAxis() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_ConeFrustum,IGESData_IGESEntity)

protected:




private:


  Standard_Real theHeight;
  Standard_Real theR1;
  Standard_Real theR2;
  gp_XYZ theFaceCenter;
  gp_XYZ theAxis;


};







#endif // _IGESSolid_ConeFrustum_HeaderFile
