// Created on: 1993-01-11
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

#ifndef _IGESDimen_PointDimension_HeaderFile
#define _IGESDimen_PointDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;
class IGESData_IGESEntity;
class IGESGeom_CircularArc;
class IGESGeom_CompositeCurve;


class IGESDimen_PointDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_PointDimension, IGESData_IGESEntity)

//! defines IGES Point Dimension, Type <220> Form <0>,
//! in package IGESDimen
//! A Point Dimension Entity consists of a leader, text, and
//! an optional circle or hexagon enclosing the text
//! IGES specs for this entity mention SimpleClosedPlanarCurve
//! Entity(106/63)which is not listed in LIST.Text In the sequel
//! we have ignored this & considered only the other two entity
//! for representing the hexagon or circle enclosing the text.
class IGESDimen_PointDimension : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_PointDimension();
  
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESDimen_LeaderArrow)& anArrow, const Handle(IGESData_IGESEntity)& aGeom);
  
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) LeaderArrow() const;
  
  //! returns the type of geometric entity.
  //! 0 if no hexagon or circle encloses the text
  //! 1 if CircularArc
  //! 2 if CompositeCurve
  //! 3 otherwise
  Standard_EXPORT Standard_Integer GeomCase() const;
  
  //! returns the Geometry Entity, Null handle if GeomCase(me) .eq. 0
  Standard_EXPORT Handle(IGESData_IGESEntity) Geom() const;
  
  //! returns Null handle if GeomCase(me) .ne. 1
  Standard_EXPORT Handle(IGESGeom_CircularArc) CircularArc() const;
  
  //! returns Null handle if GeomCase(me) .ne. 2
  Standard_EXPORT Handle(IGESGeom_CompositeCurve) CompositeCurve() const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_PointDimension,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_LeaderArrow) theLeader;
  Handle(IGESData_IGESEntity) theGeom;


};







#endif // _IGESDimen_PointDimension_HeaderFile
