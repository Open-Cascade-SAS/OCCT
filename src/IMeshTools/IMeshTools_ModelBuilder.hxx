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

#ifndef _IMeshTools_ModelBuilder_HeaderFile
#define _IMeshTools_ModelBuilder_HeaderFile

#include <Message_Algorithm.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <IMeshData_Model.hxx>

struct IMeshTools_Parameters;

//! Interface class represents API for tool building discrete model.
//!
//! The following statuses should be used by default:
//! Message_Done1 - model has been successfully built.
//! Message_Fail1 - empty shape.
//! Message_Fail2 - model has not been build due to unexpected reason.
class IMeshTools_ModelBuilder : public Message_Algorithm
{
public:
  //! Destructor.
  virtual ~IMeshTools_ModelBuilder() {}

  //! Exceptions protected method to create discrete model for the given shape.
  //! Returns nullptr in case of failure.
  Handle(IMeshData_Model) Perform(const TopoDS_Shape&          theShape,
                                  const IMeshTools_Parameters& theParameters)
  {
    ClearStatus();

    try
    {
      OCC_CATCH_SIGNALS

      return performInternal(theShape, theParameters);
    }
    catch (Standard_Failure const&)
    {
      SetStatus(Message_Fail2);
      return NULL;
    }
  }

  DEFINE_STANDARD_RTTIEXT(IMeshTools_ModelBuilder, Message_Algorithm)

protected:
  //! Constructor.
  IMeshTools_ModelBuilder() {}

  //! Creates discrete model for the given shape.
  //! Returns nullptr in case of failure.
  Standard_EXPORT virtual Handle(IMeshData_Model) performInternal(
    const TopoDS_Shape&          theShape,
    const IMeshTools_Parameters& theParameters) = 0;
};

#endif