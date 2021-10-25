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
#include <Standard_Type.hxx>

#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESDimen_GeneralNote;
class IGESDimen_LeaderArrow;


class IGESDimen_GeneralLabel;
DEFINE_STANDARD_HANDLE(IGESDimen_GeneralLabel, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Handle(IGESDimen_HArray1OfLeaderArrow)& someLeaders);
  
  //! returns General Note Entity
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns Number of Leaders
  Standard_EXPORT Standard_Integer NbLeaders() const;
  
  //! returns Leader Entity
  //! raises exception if Index <= 0 or Index > NbLeaders()
  Standard_EXPORT Handle(IGESDimen_LeaderArrow) Leader (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_GeneralLabel,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Handle(IGESDimen_HArray1OfLeaderArrow) theLeaders;


};







#endif // _IGESDimen_GeneralLabel_HeaderFile
