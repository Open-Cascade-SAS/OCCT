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

#ifndef _IGESDimen_LeaderArrow_HeaderFile
#define _IGESDimen_LeaderArrow_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;
class gp_Pnt;

//! defines LeaderArrow, Type <214> Form <1-12>
//! in package IGESDimen
//! Consists of one or more line segments except when
//! leader is part of an angular dimension, with links to
//! presumed text item
class IGESDimen_LeaderArrow : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_LeaderArrow();

  //! This method is used to set the fields of the class
  //! LeaderArrow
  //! - height      : ArrowHead height
  //! - width       : ArrowHead width
  //! - depth       : Z Depth
  //! - position    : ArrowHead coordinates
  //! - segments    : Segment tail coordinate pairs
  Standard_EXPORT void Init(const double                                   height,
                            const double                                   width,
                            const double                                   depth,
                            const gp_XY&                                   position,
                            const occ::handle<NCollection_HArray1<gp_XY>>& segments);

  //! Changes FormNumber (indicates the Shape of the Arrow)
  //! Error if not in range [0-12]
  Standard_EXPORT void SetFormNumber(const int form);

  //! returns number of segments
  Standard_EXPORT int NbSegments() const;

  //! returns ArrowHead height
  Standard_EXPORT double ArrowHeadHeight() const;

  //! returns ArrowHead width
  Standard_EXPORT double ArrowHeadWidth() const;

  //! returns Z depth
  Standard_EXPORT double ZDepth() const;

  //! returns ArrowHead coordinates
  Standard_EXPORT gp_Pnt2d ArrowHead() const;

  //! returns ArrowHead coordinates after Transformation
  Standard_EXPORT gp_Pnt TransformedArrowHead() const;

  //! returns segment tail coordinates.
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT gp_Pnt2d SegmentTail(const int Index) const;

  //! returns segment tail coordinates after Transformation.
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT gp_Pnt TransformedSegmentTail(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_LeaderArrow, IGESData_IGESEntity)

private:
  double                                  theArrowHeadHeight;
  double                                  theArrowHeadWidth;
  double                                  theZDepth;
  gp_XY                                   theArrowHead;
  occ::handle<NCollection_HArray1<gp_XY>> theSegmentTails;
};

#endif // _IGESDimen_LeaderArrow_HeaderFile
