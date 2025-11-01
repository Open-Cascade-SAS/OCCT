// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Placement_HeaderFile
#define _TDataXtd_Placement_HeaderFile

#include <TDataStd_GenericEmpty.hxx>
class TDF_Label;

class TDataXtd_Placement;
DEFINE_STANDARD_HANDLE(TDataXtd_Placement, TDataStd_GenericEmpty)

class TDataXtd_Placement : public TDataStd_GenericEmpty
{

public:
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Find, or    create,   an Placement  attribute.     the
  //! Placement attribute is returned.
  //! Placement methods
  //! =================
  Standard_EXPORT static Handle(TDataXtd_Placement) Set(const TDF_Label& label);

  Standard_EXPORT TDataXtd_Placement();

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(TDataXtd_Placement, TDataStd_GenericEmpty)

protected:
private:
};

#endif // _TDataXtd_Placement_HeaderFile
