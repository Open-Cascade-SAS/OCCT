// Created on: 1994-08-25
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESSelect_ChangeLevelNumber_HeaderFile
#define _IGESSelect_ChangeLevelNumber_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
#include <Standard_Boolean.hxx>
class IFSelect_IntParam;
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;


class IGESSelect_ChangeLevelNumber;
DEFINE_STANDARD_HANDLE(IGESSelect_ChangeLevelNumber, IGESSelect_ModelModifier)

//! Changes Level Number (as null or single) to a new single value
//! Entities attached to a LevelListEntity are ignored
//! Entities considered can be, either all Entities but those
//! attached to a LevelListEntity, or Entities attached to a
//! specific Level Number (0 for not defined).
//!
//! Remark : this concerns the Directory Part only. The Level List
//! Entities themselves (their content) are not affected.
class IGESSelect_ChangeLevelNumber : public IGESSelect_ModelModifier
{

public:

  
  //! Creates a ChangeLevelNumber, not yet defined
  //! (see SetOldNumber and SetNewNumber)
  Standard_EXPORT IGESSelect_ChangeLevelNumber();
  
  //! Returns True if OldNumber is defined : then, only entities
  //! attached to the value of OldNumber will be considered. Else,
  //! all entities but those attached to a Level List will be.
  Standard_EXPORT Standard_Boolean HasOldNumber() const;
  
  //! Returns the parameter for OldNumber. If not defined (Null
  //! Handle), it will be interpreted as "all level numbers"
  Standard_EXPORT Handle(IFSelect_IntParam) OldNumber() const;
  
  //! Sets a parameter for OldNumber
  Standard_EXPORT void SetOldNumber (const Handle(IFSelect_IntParam)& param);
  
  //! Returns the parameter for NewNumber. If not defined (Null
  //! Handle), it will be interpreted as "new value 0"
  Standard_EXPORT Handle(IFSelect_IntParam) NewNumber() const;
  
  //! Sets a parameter for NewNumber
  Standard_EXPORT void SetNewNumber (const Handle(IFSelect_IntParam)& param);
  
  //! Specific action : considers selected target entities :
  //! If OldNumber is not defined, all entities but those attached
  //! to a Level List
  //! If OldNumber is defined (value not negative), entities with a
  //! defined Level Number (can be zero)
  //! Attaches all these entities to value given by NewNumber, or
  //! zero if not defined
  Standard_EXPORT void Performing (IFSelect_ContextModif& ctx, const Handle(IGESData_IGESModel)& target, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  //! Returns a text which is
  //! "Changes Level Number <old> to <new>" , or
  //! "Changes all Levels Numbers positive and zero to <new>"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_ChangeLevelNumber,IGESSelect_ModelModifier)

protected:




private:


  Handle(IFSelect_IntParam) theold;
  Handle(IFSelect_IntParam) thenew;


};







#endif // _IGESSelect_ChangeLevelNumber_HeaderFile
