// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshData_ParametersList_HeaderFile
#define _IMeshData_ParametersList_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

//! Interface class representing list of parameters on curve.
class IMeshData_ParametersList : public Standard_Transient
{
public:
  //! Destructor.
  virtual ~IMeshData_ParametersList() {}

  //! Returns parameter with the given index.
  Standard_EXPORT virtual Standard_Real& GetParameter(const Standard_Integer theIndex) = 0;

  //! Returns number of parameters.
  Standard_EXPORT virtual Standard_Integer ParametersNb() const = 0;

  //! Clears parameters list.
  Standard_EXPORT virtual void Clear(const Standard_Boolean isKeepEndPoints) = 0;

  DEFINE_STANDARD_RTTIEXT(IMeshData_ParametersList, Standard_Transient)

protected:
  //! Constructor.
  IMeshData_ParametersList() {}

  //! Removes parameter with the given index.
  Standard_EXPORT virtual void removeParameter(const Standard_Integer theIndex) = 0;
};

#endif