// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_Node_HeaderFile
#define _IGESAppli_Node_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESGeom_TransformationMatrix;
class gp_XYZ;
class gp_Pnt;
class IGESData_TransfEntity;


class IGESAppli_Node;
DEFINE_STANDARD_HANDLE(IGESAppli_Node, IGESData_IGESEntity)

//! defines Node, Type <134> Form <0>
//! in package IGESAppli
//! Geometric point used in the definition of a finite element.
class IGESAppli_Node : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_Node();
  
  //! This method is used to set the fields of the class Node
  //! - aCoord       : Nodal Coordinates
  //! - aCoordSystem : the Nodal Displacement Coordinate
  //! System Entity (default 0 is Global
  //! Cartesian Coordinate system)
  Standard_EXPORT void Init (const gp_XYZ& aCoord, const Handle(IGESGeom_TransformationMatrix)& aCoordSystem);
  
  //! returns the nodal coordinates
  Standard_EXPORT gp_Pnt Coord() const;
  
  //! returns TransfEntity if a Nodal Displacement Coordinate
  //! System Entity is defined
  //! else (for Global Cartesien) returns Null Handle
  Standard_EXPORT Handle(IGESData_TransfEntity) System() const;
  
  //! Computes & returns the Type of Coordinate System :
  //! 0 GlobalCartesian, 1 Cartesian, 2 Cylindrical, 3 Spherical
  Standard_EXPORT Standard_Integer SystemType() const;
  
  //! returns the Nodal coordinates after transformation
  Standard_EXPORT gp_Pnt TransformedNodalCoord() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_Node,IGESData_IGESEntity)

protected:




private:


  gp_XYZ theCoord;
  Handle(IGESGeom_TransformationMatrix) theSystem;


};







#endif // _IGESAppli_Node_HeaderFile
