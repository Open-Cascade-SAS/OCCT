// Created on: 1999-06-10
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TFunction_Function_HeaderFile
#define _TFunction_Function_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_GUID.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TDF_RelocationTable;
class TDF_DataSet;

class TFunction_Function;
DEFINE_STANDARD_HANDLE(TFunction_Function, TDF_Attribute)

//! Provides the following two services
//! -   a link to an evaluation driver
//! -   the means of providing a link between a
//! function and an evaluation driver.
class TFunction_Function : public TDF_Attribute
{

public:
  //! Static methods:
  //! ==============
  //! Finds or Creates a function attribute on the label <L>.
  //! Returns the function attribute.
  Standard_EXPORT static Handle(TFunction_Function) Set(const TDF_Label& L);

  //! Finds or Creates a function attribute on the label <L>.
  //! Sets a driver ID to the function.
  //! Returns the function attribute.
  Standard_EXPORT static Handle(TFunction_Function) Set(const TDF_Label&     L,
                                                        const Standard_GUID& DriverID);

  //! Returns the GUID for functions.
  //! Returns a function found on the label.
  //! Instance methods:
  //! ================
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT TFunction_Function();

  //! Returns the GUID for this function's driver.
  const Standard_GUID& GetDriverGUID() const { return myDriverGUID; }

  //! Sets the driver for this function as that
  //! identified by the GUID guid.
  Standard_EXPORT void SetDriverGUID(const Standard_GUID& guid);

  //! Returns true if the execution failed
  Standard_Boolean Failed() const { return myFailure != 0; }

  //! Sets the failed index.
  Standard_EXPORT void SetFailure(const Standard_Integer mode = 0);

  //! Returns an index of failure if the execution of this function failed.
  //! If this integer value is 0, no failure has occurred.
  //! Implementation of Attribute methods:
  //! ===================================
  Standard_Integer GetFailure() const { return myFailure; }

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)&       into,
                                     const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual void References(const Handle(TDF_DataSet)& aDataSet) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TFunction_Function, TDF_Attribute)

private:
  Standard_GUID    myDriverGUID;
  Standard_Integer myFailure;
};

#endif // _TFunction_Function_HeaderFile
