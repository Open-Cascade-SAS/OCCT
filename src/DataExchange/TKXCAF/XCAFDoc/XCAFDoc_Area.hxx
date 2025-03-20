// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Area_HeaderFile
#define _XCAFDoc_Area_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TDataStd_Real.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;

class XCAFDoc_Area;
DEFINE_STANDARD_HANDLE(XCAFDoc_Area, TDataStd_Real)

//! attribute to store area
class XCAFDoc_Area : public TDataStd_Real
{

public:
  //! class methods
  //! =============
  Standard_EXPORT XCAFDoc_Area();

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Sets a value of volume
  Standard_EXPORT void Set(const Standard_Real vol);

  //! Find, or create, an Area attribute and set its value
  Standard_EXPORT static Handle(XCAFDoc_Area) Set(const TDF_Label& label, const Standard_Real area);

  Standard_EXPORT Standard_Real Get() const;

  //! Returns volume of area as argument and success status
  //! returns false if no such attribute at the <label>
  Standard_EXPORT static Standard_Boolean Get(const TDF_Label& label, Standard_Real& area);

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_Area, TDataStd_Real)
};

#endif // _XCAFDoc_Area_HeaderFile
