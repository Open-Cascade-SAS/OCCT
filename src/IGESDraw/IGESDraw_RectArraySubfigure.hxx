// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_RectArraySubfigure_HeaderFile
#define _IGESDraw_RectArraySubfigure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt;


class IGESDraw_RectArraySubfigure;
DEFINE_STANDARD_HANDLE(IGESDraw_RectArraySubfigure, IGESData_IGESEntity)

//! Defines IGES Rectangular Array Subfigure Instance Entity,
//! Type <412> Form Number <0> in package IGESDraw
//! Used to produce copies of object called the base entity,
//! arranging them in equally spaced rows and columns
class IGESDraw_RectArraySubfigure : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDraw_RectArraySubfigure();
  
  //! This method is used to set the fields of the class
  //! RectArraySubfigure
  //! - aBase         : a base entity which is replicated
  //! - aScale        : Scale Factor
  //! - aCorner       : lower left hand corner for the entire array
  //! - nbCols        : Number of columns of the array
  //! - nbRows        : Number of rows of the array
  //! - hDisp         : Column separations
  //! - vtDisp        : Row separation
  //! - rotationAngle : Rotation angle specified in radians
  //! - allDont       : DO-DON'T flag to control which portion
  //! to display
  //! - allNumPos     : List of positions to be or not to be
  //! displayed
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aBase, const Standard_Real aScale, const gp_XYZ& aCorner, const Standard_Integer nbCols, const Standard_Integer nbRows, const Standard_Real hDisp, const Standard_Real vtDisp, const Standard_Real rotationAngle, const Standard_Integer doDont, const Handle(TColStd_HArray1OfInteger)& allNumPos);
  
  //! returns the base entity, copies of which are produced
  Standard_EXPORT Handle(IGESData_IGESEntity) BaseEntity() const;
  
  //! returns the scale factor
  Standard_EXPORT Standard_Real ScaleFactor() const;
  
  //! returns coordinates of lower left hand corner for the entire array
  Standard_EXPORT gp_Pnt LowerLeftCorner() const;
  
  //! returns Transformed coordinates of lower left corner for the array
  Standard_EXPORT gp_Pnt TransformedLowerLeftCorner() const;
  
  //! returns number of columns in the array
  Standard_EXPORT Standard_Integer NbColumns() const;
  
  //! returns number of rows in the array
  Standard_EXPORT Standard_Integer NbRows() const;
  
  //! returns horizontal distance between columns
  Standard_EXPORT Standard_Real ColumnSeparation() const;
  
  //! returns vertical distance between rows
  Standard_EXPORT Standard_Real RowSeparation() const;
  
  //! returns rotation angle in radians
  Standard_EXPORT Standard_Real RotationAngle() const;
  
  //! returns True if (ListCount = 0) i.e., all elements to be displayed
  Standard_EXPORT Standard_Boolean DisplayFlag() const;
  
  //! returns 0 if all replicated entities to be displayed
  Standard_EXPORT Standard_Integer ListCount() const;
  
  //! returns 0 if half or fewer of the elements of  the array are defined
  //! 1 if half or more of the elements are defined
  Standard_EXPORT Standard_Boolean DoDontFlag() const;
  
  //! returns whether Index is to be processed (DO)
  //! or not to be processed(DON'T)
  //! if (ListCount = 0) return theDoDontFlag
  Standard_EXPORT Standard_Boolean PositionNum (const Standard_Integer Index) const;
  
  //! returns the Index'th value position
  //! raises exception if Index <= 0 or Index > ListCount()
  Standard_EXPORT Standard_Integer ListPosition (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_RectArraySubfigure,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theBaseEntity;
  Standard_Real theScaleFactor;
  gp_XYZ theLowerLeftCorner;
  Standard_Integer theNbColumns;
  Standard_Integer theNbRows;
  Standard_Real theColumnSeparation;
  Standard_Real theRowSeparation;
  Standard_Real theRotationAngle;
  Standard_Boolean theDoDontFlag;
  Handle(TColStd_HArray1OfInteger) thePositions;


};







#endif // _IGESDraw_RectArraySubfigure_HeaderFile
