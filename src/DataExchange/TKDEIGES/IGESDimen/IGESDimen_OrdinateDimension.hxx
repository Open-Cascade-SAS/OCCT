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

#ifndef _IGESDimen_OrdinateDimension_HeaderFile
#define _IGESDimen_OrdinateDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_WitnessLine;
class IGESDimen_LeaderArrow;

class IGESDimen_OrdinateDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_OrdinateDimension, IGESData_IGESEntity)

//! defines IGES Ordinate Dimension, Type <218> Form <0, 1>,
//! in package IGESDimen
//! Note   : The ordinate dimension entity is used to
//! indicate dimensions from a common base line.
//! Dimensioning is only permitted along the XT
//! or YT axis.
class IGESDimen_OrdinateDimension : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_OrdinateDimension();

  Standard_EXPORT void Init(const Handle(IGESDimen_GeneralNote)& aNote,
                            const Standard_Boolean               aType,
                            const Handle(IGESDimen_WitnessLine)& aLine,
                            const Handle(IGESDimen_LeaderArrow)& anArrow);

  //! returns True if Witness Line and False if Leader (only for Form 0)
  Standard_EXPORT Standard_Boolean IsLine() const;

  //! returns True if Leader and False if Witness Line (only for Form 0)
  Standard_EXPORT Standard_Boolean IsLeader() const;

  //! returns the General Note entity associated.
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;

  //! returns the Witness Line associated or Null handle
  Standard_EXPORT Handle(IGESDimen_WitnessLine) WitnessLine() const;

  //! returns the Leader associated or Null handle
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) Leader() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_OrdinateDimension, IGESData_IGESEntity)

protected:
private:
  Handle(IGESDimen_GeneralNote) theNote;
  Standard_Boolean              isItLine;
  Handle(IGESDimen_WitnessLine) theWitnessLine;
  Handle(IGESDimen_LeaderArrow) theLeader;
};

#endif // _IGESDimen_OrdinateDimension_HeaderFile
