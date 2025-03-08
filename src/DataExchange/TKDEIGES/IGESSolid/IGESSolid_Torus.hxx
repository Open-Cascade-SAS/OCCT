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

#ifndef _IGESSolid_Torus_HeaderFile
#define _IGESSolid_Torus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class gp_Dir;

class IGESSolid_Torus;
DEFINE_STANDARD_HANDLE(IGESSolid_Torus, IGESData_IGESEntity)

//! defines Torus, Type <160> Form Number <0>
//! in package IGESSolid
//! A Torus is a solid formed by revolving a circular disc
//! about a specified coplanar axis.
class IGESSolid_Torus : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_Torus();

  //! This method is used to set the fields of the class Torus
  //! - R1     : distance from center of torus to center
  //! of circular disc to be revolved
  //! - R2     : radius of circular disc
  //! - aPoint : center point coordinates (default (0,0,0))
  //! - anAxis : unit vector in axis direction (default (0,0,1))
  Standard_EXPORT void Init(const Standard_Real R1,
                            const Standard_Real R2,
                            const gp_XYZ&       aPoint,
                            const gp_XYZ&       anAxisdir);

  //! returns the distance from the center of torus to the center of
  //! the disc to be revolved
  Standard_EXPORT Standard_Real MajorRadius() const;

  //! returns the radius of the disc to be revolved
  Standard_EXPORT Standard_Real DiscRadius() const;

  //! returns the center of torus
  Standard_EXPORT gp_Pnt AxisPoint() const;

  //! returns the center of torus after applying TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedAxisPoint() const;

  //! returns direction of the axis
  Standard_EXPORT gp_Dir Axis() const;

  //! returns direction of the axis after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedAxis() const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_Torus, IGESData_IGESEntity)

protected:
private:
  Standard_Real theR1;
  Standard_Real theR2;
  gp_XYZ        thePoint;
  gp_XYZ        theAxis;
};

#endif // _IGESSolid_Torus_HeaderFile
