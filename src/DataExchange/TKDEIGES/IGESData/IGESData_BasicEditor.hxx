// Created on: 1995-08-25
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _IGESData_BasicEditor_HeaderFile
#define _IGESData_BasicEditor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Interface_GeneralLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class IGESData_Protocol;
class IGESData_IGESModel;
class IGESData_IGESEntity;

//! This class provides various functions of basic edition,
//! such as :
//! - setting header unit (WARNING : DOES NOT convert entities)
//! - computation of the status (Subordinate, UseFlag) of entities
//! of IGES Entities on a whole model
//! - auto correction of IGES Entities, defined both by DirChecker
//! and by specific service AutoCorrect
//! (this auto correction performs non-ambigious, rather logic,
//! editions)
class IGESData_BasicEditor
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Basic Editor which should be initialized via Init() method.
  Standard_EXPORT IGESData_BasicEditor();

  //! Creates a Basic Editor, with a new IGESModel, ready to run
  Standard_EXPORT IGESData_BasicEditor(const occ::handle<IGESData_Protocol>& protocol);

  //! Creates a Basic Editor for IGES Data, ready to run
  Standard_EXPORT IGESData_BasicEditor(const occ::handle<IGESData_IGESModel>& model,
                                       const occ::handle<IGESData_Protocol>&  protocol);

  //! Initialize a Basic Editor, with a new IGESModel, ready to run
  Standard_EXPORT void Init(const occ::handle<IGESData_Protocol>& protocol);

  //! Initialize a Basic Editor for IGES Data, ready to run
  Standard_EXPORT void Init(const occ::handle<IGESData_IGESModel>& model,
                            const occ::handle<IGESData_Protocol>&  protocol);

  //! Returns the designated model
  Standard_EXPORT occ::handle<IGESData_IGESModel> Model() const;

  //! Sets a new unit from its flag (param 14 of Global Section)
  //! Returns True if done, False if <flag> is incorrect
  Standard_EXPORT bool SetUnitFlag(const int flag);

  //! Sets a new unit from its value in meters (rounded to the
  //! closest one, max gap 1%)
  //! Returns True if done, False if <val> is too far from a
  //! suitable value
  Standard_EXPORT bool SetUnitValue(const double val);

  //! Sets a new unit from its name (param 15 of Global Section)
  //! Returns True if done, False if <name> is incorrect
  //! Remark : if <flag> has been set to 3 (user defined), <name>
  //! is then free
  Standard_EXPORT bool SetUnitName(const char* const name);

  //! Applies unit value to convert header data : Resolution,
  //! MaxCoord, MaxLineWeight
  //! Applies unit only once after SetUnit... has been called,
  //! if <enforce> is given as True.
  //! It can be called just before writing the model to a file,
  //! i.e. when definitive values are finally known
  Standard_EXPORT void ApplyUnit(const bool enforce = false);

  //! Performs the re-computation of status on the whole model
  //! (Subordinate Status and Use Flag of each IGES Entity), which
  //! can have required values according the way they are referenced
  //! (see definitions of Logical use, Physical use, etc...)
  Standard_EXPORT void ComputeStatus();

  //! Performs auto-correction on an IGESEntity
  //! Returns True if something has changed, False if nothing done.
  //!
  //! Works with the specific IGES Services : DirChecker which
  //! allows to correct data in "Directory Part" of Entities (such
  //! as required values for status, or references to be null), and
  //! the specific IGES service OwnCorrect, which is specialised for
  //! each type of entity.
  Standard_EXPORT bool AutoCorrect(const occ::handle<IGESData_IGESEntity>& ent);

  //! Performs auto-correction on the whole Model
  //! Returns the count of modified entities
  Standard_EXPORT int AutoCorrectModel();

  //! From the name of unit, computes flag number, 0 if incorrect
  //! (in this case, user defined entity remains possible)
  Standard_EXPORT static int UnitNameFlag(const char* const name);

  //! From the flag of unit, determines value in MM, 0 if incorrect
  Standard_EXPORT static double UnitFlagValue(const int flag);

  //! From the flag of unit, determines its name, "" if incorrect
  Standard_EXPORT static const char* UnitFlagName(const int flag);

  //! From the flag of IGES version, returns name, "" if incorrect
  Standard_EXPORT static const char* IGESVersionName(const int flag);

  //! Returns the maximum allowed value for IGESVersion Flag
  Standard_EXPORT static int IGESVersionMax();

  //! From the flag of drafting standard, returns name, "" if incorrect
  Standard_EXPORT static const char* DraftingName(const int flag);

  //! Returns the maximum allowed value for Drafting Flag
  Standard_EXPORT static int DraftingMax();

  //! Returns Flag corresponding to the scaling theValue.
  //! Returns 0 if there's no such flag.
  Standard_EXPORT static int GetFlagByValue(const double theValue);

private:
  bool           theunit;
  occ::handle<IGESData_Protocol>  theproto;
  occ::handle<IGESData_IGESModel> themodel;
  Interface_GeneralLib       theglib;
  IGESData_SpecificLib       theslib;
};

#endif // _IGESData_BasicEditor_HeaderFile
