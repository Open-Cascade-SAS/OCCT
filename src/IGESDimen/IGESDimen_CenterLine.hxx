// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDimen_CenterLine_HeaderFile
#define _IGESDimen_CenterLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;

class IGESDimen_CenterLine;
DEFINE_STANDARD_HANDLE(IGESDimen_CenterLine, IGESData_IGESEntity)

//! defines CenterLine, Type <106> Form <20-21>
//! in package IGESDimen
//! Is an entity appearing as crosshairs or as a
//! construction between 2 positions
class IGESDimen_CenterLine : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_CenterLine();

  //! This method is used to set the fields of the class
  //! CenterLine
  //! - aDataType      : Interpretation Flag, always = 1
  //! - aZDisplacement : Common z displacement
  //! - dataPnts       : Data points (x and y)
  Standard_EXPORT void Init(const Standard_Integer            aDataType,
                            const Standard_Real               aZdisp,
                            const Handle(TColgp_HArray1OfXY)& dataPnts);

  //! Sets FormNumber to 20 if <mode> is True, 21 else
  Standard_EXPORT void SetCrossHair(const Standard_Boolean mode);

  //! returns Interpretation Flag : IP = 1.
  Standard_EXPORT Standard_Integer Datatype() const;

  //! returns Number of Data Points.
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! returns Common Z displacement.
  Standard_EXPORT Standard_Real ZDisplacement() const;

  //! returns the data point as Pnt from gp.
  //! raises exception if Index <= 0 or Index > NbPoints()
  Standard_EXPORT gp_Pnt Point(const Standard_Integer Index) const;

  //! returns the data point as Pnt from gp after Transformation.
  //! raises exception if Index <= 0 or Index > NbPoints()
  Standard_EXPORT gp_Pnt TransformedPoint(const Standard_Integer Index) const;

  //! returns True if Form is 20.
  Standard_EXPORT Standard_Boolean IsCrossHair() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_CenterLine, IGESData_IGESEntity)

protected:
private:
  Standard_Integer           theDatatype;
  Standard_Real              theZDisplacement;
  Handle(TColgp_HArray1OfXY) theDataPoints;
};

#endif // _IGESDimen_CenterLine_HeaderFile
