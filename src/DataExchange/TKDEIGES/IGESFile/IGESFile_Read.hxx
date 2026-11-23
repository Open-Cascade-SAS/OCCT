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

#ifndef IGESFile_Read_HeaderFile
#define IGESFile_Read_HeaderFile

#include <IGESData_FileRecognizer.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <Standard_IStream.hxx>

//! Reads an IGES file into a model.
//! @param[in] theFileName path to the IGES file
//! @param[out] theModel model receiving the file contents
//! @param[in] theProtocol IGES protocol used to load the model
//! @return 0 on success, or a negative value when the file cannot be read
Standard_EXPORT int IGESFile_Read(char*                                  theFileName,
                                  const occ::handle<IGESData_IGESModel>& theModel,
                                  const occ::handle<IGESData_Protocol>&  theProtocol);

//! Reads IGES data from an existing C++ stream into a model.
//! The stream is consumed as binary data and may have exceptions enabled.
//! @param[in] theName source name used for diagnostics
//! @param[in,out] theIStream stream supplying IGES bytes
//! @param[out] theModel model receiving the stream contents
//! @param[in] theProtocol IGES protocol used to load the model
//! @return 0 on success, or a negative value on invalid input or a stream failure
Standard_EXPORT int IGESFile_Read(const char*                            theName,
                                  Standard_IStream&                      theIStream,
                                  const occ::handle<IGESData_IGESModel>& theModel,
                                  const occ::handle<IGESData_Protocol>&  theProtocol);

//! Reads an FNES-encoded IGES file into a model.
//! @param[in] theFileName path to the FNES file
//! @param[out] theModel model receiving the file contents
//! @param[in] theProtocol IGES protocol used to load the model
//! @return 0 on success, or a negative value when the file cannot be read
Standard_EXPORT int IGESFile_ReadFNES(char*                                  theFileName,
                                      const occ::handle<IGESData_IGESModel>& theModel,
                                      const occ::handle<IGESData_Protocol>&  theProtocol);

//! Reads an IGES or FNES file into a model using an optional recognizer.
//! @param[in] theFileName path to the input file
//! @param[out] theModel model receiving the file contents
//! @param[in] theProtocol IGES protocol used to load the model
//! @param[in] theRecognizer optional entity recognizer
//! @param[in] theIsFNES true to decode FNES records
//! @return 0 on success, or a negative value when the file cannot be read
Standard_EXPORT int IGESFile_Read(char*                                       theFileName,
                                  const occ::handle<IGESData_IGESModel>&      theModel,
                                  const occ::handle<IGESData_Protocol>&       theProtocol,
                                  const occ::handle<IGESData_FileRecognizer>& theRecognizer,
                                  bool                                        theIsFNES = false);

//! Reads IGES or FNES data from an existing C++ stream using an optional recognizer.
//! The stream is consumed as binary data and partial data read while an EOF exception is raised is
//! retained.
//! @param[in] theName source name used for diagnostics
//! @param[in,out] theIStream stream supplying IGES bytes
//! @param[out] theModel model receiving the stream contents
//! @param[in] theProtocol IGES protocol used to load the model
//! @param[in] theRecognizer optional entity recognizer
//! @param[in] theIsFNES true to decode FNES records
//! @return 0 on success, or a negative value on invalid input or a stream failure
Standard_EXPORT int IGESFile_Read(const char*                                 theName,
                                  Standard_IStream&                           theIStream,
                                  const occ::handle<IGESData_IGESModel>&      theModel,
                                  const occ::handle<IGESData_Protocol>&       theProtocol,
                                  const occ::handle<IGESData_FileRecognizer>& theRecognizer,
                                  bool                                        theIsFNES = false);

#endif
