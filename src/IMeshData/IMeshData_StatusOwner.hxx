// Created on: 2016-06-23
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

#ifndef _IMeshData_StatusOwner_HeaderFile
#define _IMeshData_StatusOwner_HeaderFile

#include <IMeshData_Status.hxx>

//! Extension interface class providing status functionality.
class IMeshData_StatusOwner
{
public:

  //! Destructor.
  virtual ~IMeshData_StatusOwner()
  {
  }

  //! Returns true in case if status is strictly equal to the given value.
  Standard_Boolean IsEqual(const IMeshData_Status theValue) const
  {
    return (myStatus == theValue);
  }

  //! Returns true in case if status is set.
  Standard_Boolean IsSet(const IMeshData_Status theValue) const
  {
    return (myStatus & theValue) != 0;
  }

  //! Adds status to status flags of a face.
  void SetStatus(const IMeshData_Status theValue)
  {
    myStatus |= theValue;
  }

  //! Adds status to status flags of a face.
  void UnsetStatus(const IMeshData_Status theValue)
  {
    myStatus &= ~theValue;
  }

  //! Returns complete status mask.
  Standard_Integer GetStatusMask() const
  {
    return myStatus;
  }

protected:

  //! Constructor. Initializes default status.
  IMeshData_StatusOwner()
    : myStatus(IMeshData_NoError)
  {
  }

private:

  Standard_Integer myStatus;
};

#endif
