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

#ifndef _IGESSolid_Block_HeaderFile
#define _IGESSolid_Block_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class gp_Dir;

class IGESSolid_Block;
DEFINE_STANDARD_HANDLE(IGESSolid_Block, IGESData_IGESEntity)

//! defines Block, Type <150> Form Number <0>
//! in package IGESSolid
//! The Block is a rectangular parallelopiped, defined with
//! one vertex at (X1, Y1, Z1) and three edges lying along
//! the local +X, +Y, +Z axes.
class IGESSolid_Block : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_Block();

  //! This method is used to set the fields of the class Block
  //! - aSize   : Length in each local directions
  //! - aCorner : Corner point coordinates. Default (0,0,0)
  //! - aXAxis  : Unit vector defining local X-axis
  //! default (1,0,0)
  //! - aZAxis  : Unit vector defining local Z-axis
  //! default (0,0,1)
  Standard_EXPORT void Init(const gp_XYZ& aSize,
                            const gp_XYZ& aCorner,
                            const gp_XYZ& aXAxis,
                            const gp_XYZ& aZAxis);

  //! returns the size of the block
  Standard_EXPORT gp_XYZ Size() const;

  //! returns the length of the Block along the local X-direction
  Standard_EXPORT Standard_Real XLength() const;

  //! returns the length of the Block along the local Y-direction
  Standard_EXPORT Standard_Real YLength() const;

  //! returns the length of the Block along the local Z-direction
  Standard_EXPORT Standard_Real ZLength() const;

  //! returns the corner point coordinates of the Block
  Standard_EXPORT gp_Pnt Corner() const;

  //! returns the corner point coordinates of the Block after applying
  //! the TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCorner() const;

  //! returns the direction defining the local X-axis
  Standard_EXPORT gp_Dir XAxis() const;

  //! returns the direction defining the local X-axis after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Dir TransformedXAxis() const;

  //! returns the direction defining the local Y-axis
  //! it is the cross product of ZAxis and XAxis
  Standard_EXPORT gp_Dir YAxis() const;

  //! returns the direction defining the local Y-axis after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Dir TransformedYAxis() const;

  //! returns the direction defining the local X-axis
  Standard_EXPORT gp_Dir ZAxis() const;

  //! returns the direction defining the local Z-axis after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Dir TransformedZAxis() const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_Block, IGESData_IGESEntity)

protected:
private:
  gp_XYZ theSize;
  gp_XYZ theCorner;
  gp_XYZ theXAxis;
  gp_XYZ theZAxis;
};

#endif // _IGESSolid_Block_HeaderFile
