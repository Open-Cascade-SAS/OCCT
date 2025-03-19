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

class TransferBRep_TransferResultInfo;
DEFINE_STANDARD_HANDLE(TransferBRep_TransferResultInfo, Standard_Transient)

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

  Standard_Integer& Result();

  Standard_Integer& ResultWarning();

  Standard_Integer& ResultFail();

  Standard_Integer& ResultWarningFail();

  Standard_Integer& NoResult();

  Standard_Integer& NoResultWarning();

  Standard_Integer& NoResultFail();

  Standard_Integer& NoResultWarningFail();

  DEFINE_STANDARD_RTTIEXT(TransferBRep_TransferResultInfo, Standard_Transient)

protected:
private:
  Standard_Integer myR;
  Standard_Integer myRW;
  Standard_Integer myRF;
  Standard_Integer myRWF;
  Standard_Integer myNR;
  Standard_Integer myNRW;
  Standard_Integer myNRF;
  Standard_Integer myNRWF;
};

#include <TransferBRep_TransferResultInfo.lxx>

#endif // _TransferBRep_TransferResultInfo_HeaderFile
