// Created on: 1994-06-03
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

#ifndef _IGESSelect_WorkLibrary_HeaderFile
#define _IGESSelect_WorkLibrary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_WorkLibrary.hxx>
#include <Standard_Integer.hxx>
class Interface_InterfaceModel;
class Interface_Protocol;
class IFSelect_ContextWrite;
class IGESData_Protocol;
class Standard_Transient;

//! Performs Read and Write an IGES File with an IGES Model
class IGESSelect_WorkLibrary : public IFSelect_WorkLibrary
{

public:
  //! Creates a IGES WorkLibrary
  //! If <modefnes> is given as True, it will work for FNES
  Standard_EXPORT IGESSelect_WorkLibrary(const bool modefnes = false);

  //! Reads a IGES File and returns a IGES Model (into <mod>),
  //! or lets <mod> "Null" in case of Error
  //! Returns 0 if OK, 1 if Read Error, -1 if File not opened
  Standard_EXPORT int
    ReadFile(const char*            name,
             occ::handle<Interface_InterfaceModel>& model,
             const occ::handle<Interface_Protocol>& protocol) const override;

  //! Writes a File from a IGES Model (brought by <ctx>)
  //! Returns False (and writes no file) if <ctx> is not for IGES
  Standard_EXPORT bool WriteFile(IFSelect_ContextWrite& ctx) const override;

  //! Defines a protocol to be adequate for IGES
  //! (encompasses ALL the IGES norm including IGESSolid, IGESAppli)
  Standard_EXPORT static occ::handle<IGESData_Protocol> DefineProtocol();

  //! Dumps an IGES Entity with an IGES Dumper. <level> is the one
  //! used by IGESDumper.
  Standard_EXPORT virtual void DumpEntity(const occ::handle<Interface_InterfaceModel>& model,
                                          const occ::handle<Interface_Protocol>&       protocol,
                                          const occ::handle<Standard_Transient>&       entity,
                                          Standard_OStream&                       S,
                                          const int level) const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_WorkLibrary, IFSelect_WorkLibrary)

private:
  bool themodefnes;
};

#endif // _IGESSelect_WorkLibrary_HeaderFile
