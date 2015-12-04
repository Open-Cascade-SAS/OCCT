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

#include <Standard_Real.hxx>
#include <gp_XY.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class gp_XY;
class gp_Pnt2d;
class gp_Pnt;


class IGESDimen_LeaderArrow;
DEFINE_STANDARD_HANDLE(IGESDimen_LeaderArrow, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Real height, const Standard_Real width, const Standard_Real depth, const gp_XY& position, const Handle(TColgp_HArray1OfXY)& segments);
  
  //! Changes FormNumber (indicates the Shape of the Arrow)
  //! Error if not in range [0-12]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns number of segments
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  //! returns ArrowHead height
  Standard_EXPORT Standard_Real ArrowHeadHeight() const;
  
  //! returns ArrowHead width
  Standard_EXPORT Standard_Real ArrowHeadWidth() const;
  
  //! returns Z depth
  Standard_EXPORT Standard_Real ZDepth() const;
  
  //! returns ArrowHead co-ordinates
  Standard_EXPORT gp_Pnt2d ArrowHead() const;
  
  //! returns ArrowHead co-ordinates after Transformation
  Standard_EXPORT gp_Pnt TransformedArrowHead() const;
  
  //! returns segment tail co-ordinates.
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT gp_Pnt2d SegmentTail (const Standard_Integer Index) const;
  
  //! returns segment tail co-ordinates after Transformation.
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT gp_Pnt TransformedSegmentTail (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_LeaderArrow,IGESData_IGESEntity)

protected:




private:


  Standard_Real theArrowHeadHeight;
  Standard_Real theArrowHeadWidth;
  Standard_Real theZDepth;
  gp_XY theArrowHead;
  Handle(TColgp_HArray1OfXY) theSegmentTails;


};







#endif // _IGESDimen_LeaderArrow_HeaderFile
