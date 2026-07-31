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

#ifndef _IGESDimen_GeneralLabel_HeaderFile
#define _IGESDimen_GeneralLabel_HeaderFile

#include <Standard.hxx>

#include <IGESDimen_LeaderArrow.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;

//! defines GeneralLabel, Type <210> Form <0>
//! in package IGESDimen
//! Used for general labeling with leaders
class IGESDimen_GeneralLabel : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_GeneralLabel();

  //! This method is used to set the fields of the class
  //! GeneralLabel
  //! - aNote       : General Note Entity
  //! - someLeaders : Associated Leader Entities
  Standard_EXPORT void Init(
    const occ::handle<IGESDimen_GeneralNote>&                                   aNote,
    const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>& someLeaders);

  //! returns General Note Entity
  Standard_EXPORT occ::handle<IGESDimen_GeneralNote> Note() const;

  //! returns Number of Leaders
  Standard_EXPORT int NbLeaders() const;

  //! returns Leader Entity
  //! raises exception if Index <= 0 or Index > NbLeaders()
  Standard_EXPORT occ::handle<IGESDimen_LeaderArrow> Leader(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_GeneralLabel, IGESData_IGESEntity)

private:
  occ::handle<IGESDimen_GeneralNote>                                   theNote;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>> theLeaders;
};

#endif // _IGESDimen_GeneralLabel_HeaderFile
