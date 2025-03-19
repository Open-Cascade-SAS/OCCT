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

#ifndef _IGESSelect_ChangeLevelList_HeaderFile
#define _IGESSelect_ChangeLevelList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_IntParam;
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_ChangeLevelList;
DEFINE_STANDARD_HANDLE(IGESSelect_ChangeLevelList, IGESSelect_ModelModifier)

//! Changes Level List (in directory part) to a new single value
//! Only entities attached to a LevelListEntity are considered
//! If OldNumber is defined, only entities whose LevelList
//! contains its Value are processed. Else all LevelLists are.
//!
//! Remark : this concerns the Directory Part only. The Level List
//! Entities themselves (their content) are not affected.
//!
//! If NewNumber is defined (positive or zero), it gives the new
//! value for Level Number. Else, the first value of the LevelList
//! is set as new LevelNumber
class IGESSelect_ChangeLevelList : public IGESSelect_ModelModifier
{

public:
  //! Creates a ChangeLevelList, not yet defined
  //! (see SetOldNumber and SetNewNumber)
  Standard_EXPORT IGESSelect_ChangeLevelList();

  //! Returns True if OldNumber is defined : then, only entities
  //! which have a LevelList which contains the value are processed.
  //! Else, all entities attached to a LevelList are.
  Standard_EXPORT Standard_Boolean HasOldNumber() const;

  //! Returns the parameter for OldNumber. If not defined (Null
  //! Handle), it will be interpreted as "all level lists"
  Standard_EXPORT Handle(IFSelect_IntParam) OldNumber() const;

  //! Sets a parameter for OldNumber
  Standard_EXPORT void SetOldNumber(const Handle(IFSelect_IntParam)& param);

  //! Returns True if NewNumber is defined : then, it gives the new
  //! value for Level Number. Else, the first value of the LevelList
  //! is used as new Level Number.
  Standard_EXPORT Standard_Boolean HasNewNumber() const;

  //! Returns the parameter for NewNumber. If not defined (Null
  //! Handle), it will be interpreted as "new value 0"
  Standard_EXPORT Handle(IFSelect_IntParam) NewNumber() const;

  //! Sets a parameter for NewNumber
  Standard_EXPORT void SetNewNumber(const Handle(IFSelect_IntParam)& param);

  //! Specific action : considers selected target entities :
  //! If OldNumber is not defined, all entities attached to a
  //! Level List
  //! If OldNumber is defined (value not negative), entities with a
  //! Level List which contains this value
  //! Attaches all these entities to value given by NewNumber, or
  //! the first value of the Level List
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which begins by
  //! "Changes Level Lists containing <old>", or
  //! "Changes all Level Lists in D.E.", and ends by
  //! " to Number <new>"  or  " to Number = first value in List"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_ChangeLevelList, IGESSelect_ModelModifier)

protected:
private:
  Handle(IFSelect_IntParam) theold;
  Handle(IFSelect_IntParam) thenew;
};

#endif // _IGESSelect_ChangeLevelList_HeaderFile
