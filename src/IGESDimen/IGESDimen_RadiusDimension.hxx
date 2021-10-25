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

#ifndef _IGESDimen_RadiusDimension_HeaderFile
#define _IGESDimen_RadiusDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;
class gp_Pnt2d;
class gp_Pnt;


class IGESDimen_RadiusDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_RadiusDimension, IGESData_IGESEntity)

//! Defines IGES Radius Dimension, type <222> Form <0, 1>,
//! in package IGESDimen.
//! A Radius Dimension Entity consists of a General Note, a
//! leader, and an arc center point. A second form of this
//! entity accounts for the occasional need to have two
//! leader entities referenced.
class IGESDimen_RadiusDimension : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_RadiusDimension();
  
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESDimen_LeaderArrow)& anArrow, const gp_XY& arcCenter, const Handle(IGESDimen_LeaderArrow)& anotherArrow);
  
  //! Allows to change Form Number
  //! (1 admits null arrow)
  Standard_EXPORT void InitForm (const Standard_Integer form);
  
  //! returns the General Note entity
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns the Leader Arrow entity
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) Leader() const;
  
  //! returns the coordinates of the Arc Center
  Standard_EXPORT gp_Pnt2d Center() const;
  
  //! returns the coordinates of the Arc Center after Transformation
  //! (Z coord taken from ZDepth of Leader Entity)
  Standard_EXPORT gp_Pnt TransformedCenter() const;
  
  //! returns True if form is 1, False if 0
  Standard_EXPORT Standard_Boolean HasLeader2() const;
  
  //! returns Null handle if Form is 0
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) Leader2() const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_RadiusDimension,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_LeaderArrow) theLeaderArrow;
  gp_XY theCenter;
  Handle(IGESDimen_LeaderArrow) theLeader2;


};







#endif // _IGESDimen_RadiusDimension_HeaderFile
