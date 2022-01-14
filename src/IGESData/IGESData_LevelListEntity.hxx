// Created on: 1992-04-07
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_LevelListEntity_HeaderFile
#define _IGESData_LevelListEntity_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>


class IGESData_LevelListEntity;
DEFINE_STANDARD_HANDLE(IGESData_LevelListEntity, IGESData_IGESEntity)

//! defines required type for LevelList in directory part
//! an effective LevelList entity must inherits it
class IGESData_LevelListEntity : public IGESData_IGESEntity
{

public:

  
  //! Must return the count of levels
  Standard_EXPORT virtual Standard_Integer NbLevelNumbers() const = 0;
  
  //! returns the Level Number of <me>, indicated by <num>
  //! raises an exception if num is out of range
  Standard_EXPORT virtual Standard_Integer LevelNumber (const Standard_Integer num) const = 0;
  
  //! returns True if <level> is in the list
  Standard_EXPORT Standard_Boolean HasLevelNumber (const Standard_Integer level) const;




  DEFINE_STANDARD_RTTIEXT(IGESData_LevelListEntity,IGESData_IGESEntity)

protected:




private:




};







#endif // _IGESData_LevelListEntity_HeaderFile
