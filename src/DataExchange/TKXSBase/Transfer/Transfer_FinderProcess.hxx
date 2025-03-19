// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_FinderProcess_HeaderFile
#define _Transfer_FinderProcess_HeaderFile

#include <Transfer_ProcessForFinder.hxx>
#include <Interface_InterfaceModel.hxx>

class Interface_InterfaceModel;
class Transfer_TransientMapper;
class Transfer_Finder;

class Transfer_FinderProcess;
DEFINE_STANDARD_HANDLE(Transfer_FinderProcess, Transfer_ProcessForFinder)

//! Adds specific features to the generic definition :
//! PrintTrace is adapted
class Transfer_FinderProcess : public Transfer_ProcessForFinder
{

public:
  //! Sets FinderProcess at initial state, with an initial size
  Standard_EXPORT Transfer_FinderProcess(const Standard_Integer nb = 10000);

  //! Sets an InterfaceModel, which can be used during transfer
  //! for instance if a context must be managed, it is in the Model
  Standard_EXPORT void SetModel(const Handle(Interface_InterfaceModel)& model);

  //! Returns the Model which can be used for context
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;

  //! In the list of mapped items (between 1 and NbMapped),
  //! searches for the first mapped item which follows <num0>
  //! (not included) and which has an attribute named <name>
  //! The considered Attributes are those brought by Finders,i.e.
  //! by Input data.
  //! While NextItemWithAttribute works on Result data (Binders)
  //!
  //! Hence, allows such an iteration
  //!
  //! for (num = FP->NextMappedWithAttribute(name,0);
  //! num > 0;
  //! num = FP->NextMappedWithAttribute(name,num) {
  //! .. process mapped item <num>
  //! }
  Standard_EXPORT Standard_Integer NextMappedWithAttribute(const Standard_CString name,
                                                           const Standard_Integer num0) const;

  //! Returns a TransientMapper for a given Transient Object
  //! Either <obj> is already mapped, then its Mapper is returned
  //! Or it is not, then a new one is created then returned, BUT
  //! it is not mapped here (use Bind or FindElseBind to do this)
  Standard_EXPORT Handle(Transfer_TransientMapper) TransientMapper(
    const Handle(Standard_Transient)& obj) const;

  //! Specific printing to trace a Finder (by its method ValueType)
  Standard_EXPORT virtual void PrintTrace(const Handle(Transfer_Finder)& start,
                                          Standard_OStream&              S) const Standard_OVERRIDE;

  //! Prints statistics on a given output, according mode
  Standard_EXPORT void PrintStats(const Standard_Integer mode, Standard_OStream& S) const;

  DEFINE_STANDARD_RTTIEXT(Transfer_FinderProcess, Transfer_ProcessForFinder)

private:
  Handle(Interface_InterfaceModel) themodel;
};

#endif // _Transfer_FinderProcess_HeaderFile
