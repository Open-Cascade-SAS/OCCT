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

#ifndef _IGESSelect_Dumper_HeaderFile
#define _IGESSelect_Dumper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SessionDumper.hxx>
class IFSelect_SessionFile;
class Standard_Transient;
class TCollection_AsciiString;

//! Dumper from IGESSelect takes into account, for SessionFile, the
//! classes defined in the package IGESSelect : Selections,
//! Dispatches, Modifiers
class IGESSelect_Dumper : public IFSelect_SessionDumper
{

public:
  //! Creates a Dumper and puts it into the Library of Dumper
  Standard_EXPORT IGESSelect_Dumper();

  //! Write the Own Parameters of Types defined in package IGESSelect
  //! Returns True if <item> has been processed, False else
  Standard_EXPORT bool WriteOwn(IFSelect_SessionFile&                  file,
                                const occ::handle<Standard_Transient>& item) const override;

  //! Recognizes and Read Own Parameters for Types of package
  //! IGESSelect. Returns True if done and <item> created, False else
  Standard_EXPORT bool ReadOwn(IFSelect_SessionFile&            file,
                               const TCollection_AsciiString&   type,
                               occ::handle<Standard_Transient>& item) const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_Dumper, IFSelect_SessionDumper)
};

#endif // _IGESSelect_Dumper_HeaderFile
