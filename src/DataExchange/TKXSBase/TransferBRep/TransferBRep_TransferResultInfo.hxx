// Created on: 1999-08-11
// Created by: Roman LYGIN
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

#ifndef _TransferBRep_TransferResultInfo_HeaderFile
#define _TransferBRep_TransferResultInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>

//! Data structure for storing information on transfer result.
//! At the moment it dispatches information for the following types:
//! - result,
//! - result + warning(s),
//! - result + fail(s),
//! - result + warning(s) + fail(s)
//! - no result,
//! - no result + warning(s),
//! - no result + fail(s),
//! - no result + warning(s) + fail(s),
class TransferBRep_TransferResultInfo : public Standard_Transient
{

public:
  //! Creates object with all fields nullified.
  Standard_EXPORT TransferBRep_TransferResultInfo();

  //! Resets all the fields.
  Standard_EXPORT void Clear();

  int& Result();

  int& ResultWarning();

  int& ResultFail();

  int& ResultWarningFail();

  int& NoResult();

  int& NoResultWarning();

  int& NoResultFail();

  int& NoResultWarningFail();

  DEFINE_STANDARD_RTTIEXT(TransferBRep_TransferResultInfo, Standard_Transient)

private:
  int myR;
  int myRW;
  int myRF;
  int myRWF;
  int myNR;
  int myNRW;
  int myNRF;
  int myNRWF;
};

#include <TransferBRep_TransferResultInfo.lxx>

#endif // _TransferBRep_TransferResultInfo_HeaderFile
