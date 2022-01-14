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

#ifndef _IGESSolid_Ellipsoid_HeaderFile
#define _IGESSolid_Ellipsoid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class gp_Dir;


class IGESSolid_Ellipsoid;
DEFINE_STANDARD_HANDLE(IGESSolid_Ellipsoid, IGESData_IGESEntity)

//! defines Ellipsoid, Type <168> Form Number <0>
//! in package IGESSolid
//! The ellipsoid is a solid bounded by the surface defined
//! by:
//! X^2       Y^2       Z^2
//! -----  +  -----  +  -----  =  1
//! LX^2      LY^2      LZ^2
class IGESSolid_Ellipsoid : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_Ellipsoid();
  
  //! This method is used to set the fields of the class
  //! Ellipsoid
  //! - aSize      : Lengths in the local X,Y,Z directions
  //! - aCenter    : Center point of ellipsoid (default (0,0,0))
  //! - anXAxis    : Unit vector defining local X-axis
  //! default (1,0,0)
  //! - anZAxis    : Unit vector defining local Z-axis
  //! default (0,0,1)
  Standard_EXPORT void Init (const gp_XYZ& aSize, const gp_XYZ& aCenter, const gp_XYZ& anXAxis, const gp_XYZ& anZAxis);
  
  //! returns the size
  Standard_EXPORT gp_XYZ Size() const;
  
  //! returns the length in the local X-direction
  Standard_EXPORT Standard_Real XLength() const;
  
  //! returns the length in the local Y-direction
  Standard_EXPORT Standard_Real YLength() const;
  
  //! returns the length in the local Z-direction
  Standard_EXPORT Standard_Real ZLength() const;
  
  //! returns the center of the ellipsoid
  Standard_EXPORT gp_Pnt Center() const;
  
  //! returns the center of the ellipsoid after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCenter() const;
  
  //! returns the vector corresponding to the local X-direction
  Standard_EXPORT gp_Dir XAxis() const;
  
  //! returns the vector corresponding to the local X-direction
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedXAxis() const;
  
  //! returns the vector corresponding to the local Y-direction
  //! which is got by taking cross product of ZAxis and XAxis
  Standard_EXPORT gp_Dir YAxis() const;
  
  //! returns the vector corresponding to the local Y-direction
  //! (which is got by taking cross product of ZAxis and XAxis)
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedYAxis() const;
  
  //! returns the vector corresponding to the local Z-direction
  Standard_EXPORT gp_Dir ZAxis() const;
  
  //! returns the vector corresponding to the local Z-direction
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedZAxis() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_Ellipsoid,IGESData_IGESEntity)

protected:




private:


  gp_XYZ theSize;
  gp_XYZ theCenter;
  gp_XYZ theXAxis;
  gp_XYZ theZAxis;


};







#endif // _IGESSolid_Ellipsoid_HeaderFile
