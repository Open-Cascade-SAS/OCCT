// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_Options_HeaderFile
#define _BOPAlgo_Options_HeaderFile

#include <Message_Report.hxx>
#include <Standard_OStream.hxx>

#include <NCollection_BaseAllocator.hxx>

class Message_ProgressScope;

//! The class provides the following options for the algorithms in Boolean Component:
//! - *Memory allocation tool* - tool for memory allocations;
//! - *Error and warning reporting* - allows recording warnings and errors occurred 
//!                              during the operation.
//!                              Error means that the algorithm has failed.
//! - *Parallel processing mode* - provides the possibility to perform operation in parallel mode;
//! - *Fuzzy tolerance* - additional tolerance for the operation to detect
//!                       touching or coinciding cases;
//! - *Using the Oriented Bounding Boxes* - Allows using the Oriented Bounding Boxes of the shapes
//!                          for filtering the intersections.
//!
class BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPAlgo_Options();

  //! Constructor with allocator
  Standard_EXPORT BOPAlgo_Options(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Destructor
  Standard_EXPORT virtual ~BOPAlgo_Options();

  //! Returns allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const
  {
    return myAllocator;
  }

  //! Clears all warnings and errors, and any data cached by the algorithm.
  //! User defined options are not cleared.
  virtual void Clear()
  {
    myReport->Clear();
  }

public:
  //!@name Error reporting mechanism

  //! Adds the alert as error (fail)
  void AddError (const Handle(Message_Alert)& theAlert)
  {
    myReport->AddAlert (Message_Fail, theAlert);
  }

  //! Adds the alert as warning
  void AddWarning (const Handle(Message_Alert)& theAlert)
  {
    myReport->AddAlert (Message_Warning, theAlert);
  }

  //! Returns true if algorithm has failed
  Standard_Boolean HasErrors() const
  {
    return ! myReport->GetAlerts(Message_Fail).IsEmpty();
  }

  //! Returns true if algorithm has generated error of specified type
  Standard_Boolean HasError (const Handle(Standard_Type)& theType) const
  {
    return myReport->HasAlert(theType, Message_Fail);
  }

  //! Returns true if algorithm has generated some warning alerts
  Standard_Boolean HasWarnings() const
  {
    return ! myReport->GetAlerts(Message_Warning).IsEmpty();
  }

  //! Returns true if algorithm has generated warning of specified type
  Standard_Boolean HasWarning (const Handle(Standard_Type)& theType) const
  {
    return myReport->HasAlert(theType, Message_Warning);
  }

  //! Returns report collecting all errors and warnings
  const Handle(Message_Report)& GetReport () const { return myReport; }

  //! Dumps the error status into the given stream
  Standard_EXPORT void DumpErrors(Standard_OStream& theOS) const;

  //! Dumps the warning statuses into the given stream
  Standard_EXPORT void DumpWarnings(Standard_OStream& theOS) const;

  //! Clears the warnings of the algorithm
  void ClearWarnings()
  {
    myReport->Clear (Message_Warning);
  }

public:
  //!@name Parallel processing mode

  //! Gets the global parallel mode
  Standard_EXPORT static Standard_Boolean GetParallelMode();

  //! Sets the global parallel mode
  Standard_EXPORT static void SetParallelMode(const Standard_Boolean theNewMode);

  //! Set the flag of parallel processing
  //! if <theFlag> is true  the parallel processing is switched on
  //! if <theFlag> is false the parallel processing is switched off
  void SetRunParallel(const Standard_Boolean theFlag)
  {
    myRunParallel = theFlag;
  }

  //! Returns the flag of parallel processing
  Standard_Boolean RunParallel() const
  {
    return myRunParallel;
  }

public:
  //!@name Fuzzy tolerance

  //! Sets the additional tolerance
  Standard_EXPORT void SetFuzzyValue(const Standard_Real theFuzz);

  //! Returns the additional tolerance
  Standard_Real FuzzyValue() const
  {
    return myFuzzyValue;
  }

public:
  //!@name Usage of Oriented Bounding boxes

  //! Enables/Disables the usage of OBB
  void SetUseOBB(const Standard_Boolean theUseOBB)
  {
    myUseOBB = theUseOBB;
  }

  //! Returns the flag defining usage of OBB
  Standard_Boolean UseOBB() const
  {
    return myUseOBB;
  }

protected:

  //! Adds error to the report if the break signal was caught. Returns true in this case, false otherwise.
  Standard_EXPORT Standard_Boolean UserBreak(const Message_ProgressScope& thePS);

protected:

  Handle(NCollection_BaseAllocator) myAllocator;
  Handle(Message_Report) myReport;
  Standard_Boolean myRunParallel;
  Standard_Real myFuzzyValue;
  Standard_Boolean myUseOBB;

};

#endif // _BOPAlgo_Options_HeaderFile
