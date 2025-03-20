// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_CopiousData_HeaderFile
#define _IGESGeom_CopiousData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class gp_Vec;

class IGESGeom_CopiousData;
DEFINE_STANDARD_HANDLE(IGESGeom_CopiousData, IGESData_IGESEntity)

//! defines IGESCopiousData, Type <106> Form <1-3,11-13,63>
//! in package IGESGeom
//! This entity stores data points in the form of pairs,
//! triples, or sextuples. An interpretation flag value
//! signifies which of these forms is being used.
class IGESGeom_CopiousData : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_CopiousData();

  //! This method is used to set the fields of the class
  //! CopiousData
  //! - aDataType : Specifies whether data is a pair or a triple
  //! or a sextuple.
  //! - aZPlane   : Common Z value for all points if datatype = 1
  //! - allData   : Data to be read in groups of 2, 3 or 6
  Standard_EXPORT void Init(const Standard_Integer               aDataType,
                            const Standard_Real                  aZPlane,
                            const Handle(TColStd_HArray1OfReal)& allData);

  //! Sets Copious Data to be a Polyline if <mode> is True
  //! (Form = 11-12-13) or a Set of Points else (Form 1-2-3)
  Standard_EXPORT void SetPolyline(const Standard_Boolean mode);

  //! Sets Copious Data to be a Closed Path 2D (Form 63)
  //! Warning : DataType is not checked and must be set to ONE by Init
  Standard_EXPORT void SetClosedPath2D();

  //! Returns True if <me> is a Set of Points (Form 1-2-3)
  Standard_EXPORT Standard_Boolean IsPointSet() const;

  //! Returns True if <me> is a Polyline (Form 11-12-13)
  Standard_EXPORT Standard_Boolean IsPolyline() const;

  //! Returns True if <me> is a Closed Path 2D (Form 63)
  Standard_EXPORT Standard_Boolean IsClosedPath2D() const;

  //! returns data type
  //! 1 = XY ( with common Z given by plane)
  //! 2 = XYZ ( point)
  //! 3 = XYZ + Vec(XYZ) (point + normal vector)
  Standard_EXPORT Standard_Integer DataType() const;

  //! returns the number of tuples
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Returns an individual Data, given the N0 of the Point
  //! and the B0 of the Coordinate (according DataType)
  Standard_EXPORT Standard_Real Data(const Standard_Integer NumPoint,
                                     const Standard_Integer NumData) const;

  //! If datatype = 1, then returns common z value for all data
  //! else returns 0
  Standard_EXPORT Standard_Real ZPlane() const;

  //! returns the coordinates of the point specified by the anIndex
  //! raises exception if anIndex <= 0 or anIndex > NbPoints()
  Standard_EXPORT gp_Pnt Point(const Standard_Integer anIndex) const;

  //! returns the coordinates of the point specified by the anIndex
  //! after applying Transf. Matrix
  //! raises exception if anIndex <= 0 or anIndex > NbPoints()
  Standard_EXPORT gp_Pnt TransformedPoint(const Standard_Integer anIndex) const;

  //! returns i, j, k values if 3-tuple else returns (0, 0, 0)
  //! raises exception if anIndex <= 0 or anIndex > NbPoints()
  Standard_EXPORT gp_Vec Vector(const Standard_Integer anIndex) const;

  //! returns transformed vector if 3-tuple else returns (0, 0, 0)
  //! raises exception if anIndex <= 0 or anIndex > NbPoints()
  Standard_EXPORT gp_Vec TransformedVector(const Standard_Integer anIndex) const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_CopiousData, IGESData_IGESEntity)

protected:
private:
  Standard_Integer              theDataType;
  Standard_Real                 theZPlane;
  Handle(TColStd_HArray1OfReal) theData;
};

#endif // _IGESGeom_CopiousData_HeaderFile
