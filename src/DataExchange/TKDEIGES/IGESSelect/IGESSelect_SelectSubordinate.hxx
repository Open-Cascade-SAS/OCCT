// Created on: 1996-10-11
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

#ifndef _IGESSelect_SelectSubordinate_HeaderFile
#define _IGESSelect_SelectSubordinate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_SelectExtract.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! This selections uses Subordinate Status as sort criterium
//! It is an integer number which can be :
//! 0 Independent
//! 1 Physically Dependent
//! 2 Logically Dependent
//! 3 Both (recorded)
//! + to sort :
//! 4 : 1 or 3  ->  at least Physically
//! 5 : 2 or 3  ->  at least Logically
//! 6 : 1 or 2 or 3 -> any kind of dependence
//! (corresponds to 0 reversed)
class IGESSelect_SelectSubordinate : public IFSelect_SelectExtract
{

public:
  //! Creates a SelectSubordinate with a status to be sorted
  Standard_EXPORT IGESSelect_SelectSubordinate(const int status);

  //! Returns the status used for sorting
  Standard_EXPORT int Status() const;

  //! Returns True if <ent> is an IGES Entity with Subordinate
  //! Status matching the criterium
  Standard_EXPORT bool Sort(const int                                    rank,
                            const occ::handle<Standard_Transient>&       ent,
                            const occ::handle<Interface_InterfaceModel>& model) const override;

  //! Returns the Selection criterium : "IGES Entity, Independent"
  //! etc...
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectSubordinate, IFSelect_SelectExtract)

private:
  int thestatus;
};

#endif // _IGESSelect_SelectSubordinate_HeaderFile
