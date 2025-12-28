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

#ifndef _IGESDraw_CircArraySubfigure_HeaderFile
#define _IGESDraw_CircArraySubfigure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;

//! Defines IGES Circular Array Subfigure Instance Entity,
//! Type <414> Form Number <0> in package IGESDraw
//!
//! Used to produce copies of object called the base entity,
//! arranging them around the edge of an imaginary circle
//! whose center and radius are specified
class IGESDraw_CircArraySubfigure : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDraw_CircArraySubfigure();

  //! This method is used to set the fields of the class
  //! CircArraySubfigure
  //! - aBase     : Base entity
  //! - aNumLocs  : Total number of possible instance locations
  //! - aCenter   : Coordinates of Center of imaginary circle
  //! - aRadius   : Radius of imaginary circle
  //! - aStAngle  : Start angle in radians
  //! - aDelAngle : Delta angle in radians
  //! - aFlag     : DO-DON'T flag to control which portion to
  //! display
  //! - allNumPos : All position to be or not to be processed
  Standard_EXPORT void Init(const occ::handle<IGESData_IGESEntity>&      aBase,
                            const int                  aNumLocs,
                            const gp_XYZ&                           aCenter,
                            const double                     aRadius,
                            const double                     aStAngle,
                            const double                     aDelAngle,
                            const int                  aFlag,
                            const occ::handle<NCollection_HArray1<int>>& allNumPos);

  //! returns the base entity, copies of which are produced
  Standard_EXPORT occ::handle<IGESData_IGESEntity> BaseEntity() const;

  //! returns total number of possible instance locations
  Standard_EXPORT int NbLocations() const;

  //! returns the center of the imaginary circle
  Standard_EXPORT gp_Pnt CenterPoint() const;

  //! returns the Transformed center of the imaginary circle
  Standard_EXPORT gp_Pnt TransformedCenterPoint() const;

  //! returns the radius of the imaginary circle
  Standard_EXPORT double CircleRadius() const;

  //! returns the start angle in radians
  Standard_EXPORT double StartAngle() const;

  //! returns the delta angle in radians
  Standard_EXPORT double DeltaAngle() const;

  //! returns 0 if all elements to be displayed
  Standard_EXPORT int ListCount() const;

  //! returns True if (ListCount = 0) all elements are to be displayed
  Standard_EXPORT bool DisplayFlag() const;

  //! returns 0 if half or fewer of the elements of the array are defined
  //! returns 1 if half or more of the elements are defined
  Standard_EXPORT bool DoDontFlag() const;

  //! returns whether Index is to be processed (DO)
  //! or not to be processed(DON'T)
  //! if (ListCount = 0) return theDoDontFlag
  //! raises exception if Index <= 0 or Index > ListCount().
  Standard_EXPORT bool PositionNum(const int Index) const;

  //! returns the Index'th value position
  //! raises exception if Index <= 0 or Index > ListCount().
  Standard_EXPORT int ListPosition(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_CircArraySubfigure, IGESData_IGESEntity)

private:
  occ::handle<IGESData_IGESEntity>      theBaseEntity;
  int                 theNbLocations;
  gp_XYZ                           theCenter;
  double                    theRadius;
  double                    theStartAngle;
  double                    theDeltaAngle;
  bool                 theDoDontFlag;
  occ::handle<NCollection_HArray1<int>> thePositions;
};

#endif // _IGESDraw_CircArraySubfigure_HeaderFile
