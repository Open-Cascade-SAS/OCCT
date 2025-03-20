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

#ifndef _IGESDimen_DiameterDimension_HeaderFile
#define _IGESDimen_DiameterDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;
class gp_Pnt2d;

class IGESDimen_DiameterDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_DiameterDimension, IGESData_IGESEntity)

//! defines DiameterDimension, Type <206> Form <0>
//! in package IGESDimen
//! Used for dimensioning diameters
class IGESDimen_DiameterDimension : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_DiameterDimension();

  //! This method is used to set the fields of the class
  //! DiameterDimension
  //! - aNote         : General Note Entity
  //! - aLeader       : First Leader Entity
  //! - anotherLeader : Second Leader Entity or a Null Handle.
  //! - aCenter       : Arc center coordinates
  Standard_EXPORT void Init(const Handle(IGESDimen_GeneralNote)& aNote,
                            const Handle(IGESDimen_LeaderArrow)& aLeader,
                            const Handle(IGESDimen_LeaderArrow)& anotherLeader,
                            const gp_XY&                         aCenter);

  //! returns the General Note Entity
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;

  //! returns the First Leader Entity
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) FirstLeader() const;

  //! returns False if theSecondleader is a Null Handle.
  Standard_EXPORT Standard_Boolean HasSecondLeader() const;

  //! returns the Second Leader Entity
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) SecondLeader() const;

  //! returns the Arc Center coordinates as Pnt2d from package gp
  Standard_EXPORT gp_Pnt2d Center() const;

  //! returns the Arc Center coordinates as Pnt2d from package gp
  //! after Transformation. (Z = 0.0 for Transformation)
  Standard_EXPORT gp_Pnt2d TransformedCenter() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_DiameterDimension, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_LeaderArrow) theFirstLeader;
  Handle(IGESDimen_LeaderArrow) theSecondLeader;
  gp_XY                         theCenter;
};

#endif // _IGESDimen_DiameterDimension_HeaderFile
