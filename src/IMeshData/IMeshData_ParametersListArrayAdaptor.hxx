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

#ifndef _IMeshData_ParametersListArrayAdaptor_HeaderFile
#define _IMeshData_ParametersListArrayAdaptor_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <IMeshData_ParametersList.hxx>

//! Auxiliary tool representing adaptor interface for child classes of 
//! IMeshData_ParametersList to be used in tools working on NCollection_Array structure.
template<class ParametersListPtrType>
class IMeshData_ParametersListArrayAdaptor : public Standard_Transient
{
public:

  //! Constructor. Initializes tool by the given parameters.
  IMeshData_ParametersListArrayAdaptor(
    const ParametersListPtrType& theParameters)
    : myParameters (theParameters)
  {
  }

  //! Destructor.
  virtual ~IMeshData_ParametersListArrayAdaptor()
  {
  }

  //! Returns lower index in parameters array.
  Standard_Integer Lower() const
  {
    return 0;
  }

  //! Returns upper index in parameters array.
  Standard_Integer Upper() const
  {
    return myParameters->ParametersNb() - 1;
  }

  //! Returns value of the given index.
  Standard_Real Value(const Standard_Integer theIndex) const
  {
    return myParameters->GetParameter(theIndex);
  }

private:

  IMeshData_ParametersListArrayAdaptor (
    const IMeshData_ParametersListArrayAdaptor<ParametersListPtrType>& theOther);

  void operator=(const IMeshData_ParametersListArrayAdaptor<ParametersListPtrType>& theOther);

  const ParametersListPtrType myParameters;
};

#endif