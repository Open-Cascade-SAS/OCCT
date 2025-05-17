// Created on: 2002-04-03
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef OSD_PerfMeter_HeaderFile
#define OSD_PerfMeter_HeaderFile

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>

//! This class enables measuring the CPU time between two points of code execution, regardless of
//! the scope of these points of code. A meter is identified by its name (string). So multiple
//! objects in various places of user code may point to the same meter. The results will be printed
//! on stdout upon finish of the program. For details see OSD_PerfMeter.h
class OSD_PerfMeter
{
public:
  //! Constructs a void meter (to further call Init and Start).
  Standard_EXPORT OSD_PerfMeter() = default;

  //! Constructs and starts (if autoStart is true) the named meter.
  //! @param theMeterName Name of the meter. If the meter with such name was already created,
  //!        and hasn't been killed, it will be used.
  //! @param theToAutoStart If true, the meter will be started immediately after creation.
  //!        Otherwise, the user should call Start() method to start the meter.
  //!        Note that if meter already exists, theToAutoStart == true will reset it.
  Standard_EXPORT OSD_PerfMeter(const TCollection_AsciiString& theMeterName,
                                const bool                     theToAutoStart = true);

  //! Destructs this meter handler. Note that the meter itself is not destructed
  //! and will be printed on stdout upon finish of the program. The meter with the name
  //! Specified in the constructor can still be used in other places of the code.
  Standard_EXPORT ~OSD_PerfMeter();

  //! Prepares the named meter. If the meter with such name was already created,
  //! it will be used. Otherwise, a new meter will be created.
  Standard_EXPORT void Init(const TCollection_AsciiString& theMeterName);

  //! Starts the meter. If the meter was already started, it will be reset.
  //! Note that the meter with the name specified in the constructor can still be used
  //! in other places of the code.
  Standard_EXPORT void Start() const;

  //! Stops the meter.
  Standard_EXPORT void Stop() const;

  //! Returns the elapsed time in seconds since the meter was started.
  Standard_EXPORT double Elapsed() const;

  //! Outputs the meter data and resets it to initial state.
  Standard_EXPORT void Kill() const;

  //! Prints the data of this meter.
  Standard_EXPORT TCollection_AsciiString Print() const;

  //! Prints the data of all meters with non-zero elapsed time.
  Standard_EXPORT static TCollection_AsciiString PrintALL();

  //! Resets all meters.
  Standard_EXPORT static void ResetALL();

private:
  TCollection_AsciiString myMeterName;
};

#endif // OSD_PerfMeter_HeaderFile
