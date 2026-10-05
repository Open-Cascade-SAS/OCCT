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

// dce 21.01.99 : move of general message to IGESToBRep_Reader

#include <cstdlib>
// declarations of basic C programs:
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_GeneralModule.hxx>
#include <Interface_Check.hxx>

//  To handle exceptions:
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

// basic definition, to include for use
#include <IGESFile_Read.hxx>

#include <OSD_FileSystem.hxx>

#include "igesread.pxx"

// MGE 16/06/98
// To use Msg class
#include <Message_Msg.hxx>

// internal breakdown to facilitate error recovery
static thread_local int THE_RECOVERY_ENTITY    = 0;
static thread_local int THE_RECOVERY_PARAMETER = 0;

//! Returns the diagnostic check associated with the calling thread.
//! @return thread-local diagnostic check
static occ::handle<Interface_Check>& checkread()
{
  static thread_local occ::handle<Interface_Check> aCheck = new Interface_Check;
  return aCheck;
}

//! Loads the parser's start and global sections into reader data.
//! @param[in,out] theReaderData reader data receiving header parameters
static void IGESFile_ReadHeader(const occ::handle<IGESData_IGESReaderData>& theReaderData);

//! Loads the parser's directory entries and parameters into reader data.
//! @param[in,out] theReaderData reader data receiving entity records
static void IGESFile_ReadContent(const occ::handle<IGESData_IGESReaderData>& theReaderData);

//! Sends a parser diagnostic.
//! @param[in] theMode diagnostic severity
//! @param[in] theMessage diagnostic message
static void IGESFile_Check(int theMode, Message_Msg& theMessage);

//  Correspondence between igesread types and Interface_ParamFile types ...
static constexpr Interface_ParamType THE_PARAMETER_TYPES[IGES_ParameterType_NbTypes] = {
  Interface_ParamVoid,
  Interface_ParamMisc,
  Interface_ParamText,
  Interface_ParamInteger,
  Interface_ParamInteger,
  Interface_ParamReal,
  Interface_ParamMisc,
  Interface_ParamReal,
  Interface_ParamEnum};

namespace
{
struct IGESFile_StreamContext
{
  Standard_IStream& Stream;
};

//! Reads a byte block from a C++ input stream.
//! @param[in] theContext stream context
//! @param[out] theBuffer destination buffer
//! @param[in] theBufferSize destination capacity
//! @return positive byte count, IGES_ReadStatus_End, or IGES_ReadStatus_Error
int readBytesFromStream(void* theContext, char* theBuffer, std::size_t theBufferSize)
{
  if (theContext == nullptr || theBuffer == nullptr || theBufferSize == 0)
  {
    return IGES_ReadStatus_Error;
  }

  IGESFile_StreamContext& aContext = *static_cast<IGESFile_StreamContext*>(theContext);
  try
  {
    aContext.Stream.read(theBuffer, static_cast<std::streamsize>(theBufferSize));
    const std::streamsize aBytesRead = aContext.Stream.gcount();
    if (aBytesRead > 0)
    {
      return static_cast<int>(aBytesRead);
    }
    return aContext.Stream.eof() ? IGES_ReadStatus_End : IGES_ReadStatus_Error;
  }
  catch (const std::ios_base::failure&)
  {
    const std::streamsize aBytesRead = aContext.Stream.gcount();
    if (aBytesRead > 0)
    {
      return static_cast<int>(aBytesRead);
    }
    return aContext.Stream.eof() ? IGES_ReadStatus_End : IGES_ReadStatus_Error;
  }
}
} // namespace

//  New way: Protocol is sufficient

int IGESFile_Read(char*                                  theFileName,
                  const occ::handle<IGESData_IGESModel>& theModel,
                  const occ::handle<IGESData_Protocol>&  theProtocol)
{
  occ::handle<IGESData_FileRecognizer> aRecognizer;
  return IGESFile_Read(theFileName, theModel, theProtocol, aRecognizer, false);
}

int IGESFile_Read(const char*                            theName,
                  Standard_IStream&                      theIStream,
                  const occ::handle<IGESData_IGESModel>& theModel,
                  const occ::handle<IGESData_Protocol>&  theProtocol)
{
  occ::handle<IGESData_FileRecognizer> aRecognizer;
  return IGESFile_Read(theName, theIStream, theModel, theProtocol, aRecognizer, false);
}

int IGESFile_ReadFNES(char*                                  theFileName,
                      const occ::handle<IGESData_IGESModel>& theModel,
                      const occ::handle<IGESData_Protocol>&  theProtocol)
{
  occ::handle<IGESData_FileRecognizer> aRecognizer;
  return IGESFile_Read(theFileName, theModel, theProtocol, aRecognizer, true);
}

//  Old way: with Recognizer

int IGESFile_Read(char*                                       theFileName,
                  const occ::handle<IGESData_IGESModel>&      theModel,
                  const occ::handle<IGESData_Protocol>&       theProtocol,
                  const occ::handle<IGESData_FileRecognizer>& theRecognizer,
                  bool                                        theIsFNES)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aStream =
    aFileSystem->OpenIStream(theFileName, std::ios::in | std::ios::binary);
  if (!aStream)
  {
    return -1;
  }
  return IGESFile_Read(theFileName, *aStream, theModel, theProtocol, theRecognizer, theIsFNES);
}

int IGESFile_Read(const char*                                 theName,
                  Standard_IStream&                           theIStream,
                  const occ::handle<IGESData_IGESModel>&      theModel,
                  const occ::handle<IGESData_Protocol>&       theProtocol,
                  const occ::handle<IGESData_FileRecognizer>& theRecognizer,
                  bool                                        theIsFNES)
{
  static_cast<void>(theName);
  //====================================
  Message_Msg Msg1  = Message_Msg("XSTEP_1");
  Message_Msg Msg15 = Message_Msg("XSTEP_15");
  //====================================

  int lesect[6];

  // Sending of message : Beginning of the reading
  IGESFile_Check(2, Msg1);

  checkread()->Clear();
  IGESFile_StreamContext aContext = {theIStream};
  const int aResult = igesread_stream(&aContext, readBytesFromStream, lesect, theIsFNES);

  if (aResult != 0)
  {
    return aResult;
  }

  //  Loading results into an IGESReader

  int nbparts, nbparams;
  iges_stats(&nbparts, &nbparams); // and performs necessary initializations
  occ::handle<IGESData_IGESReaderData> IR =
    //    new IGESData_IGESReaderData (nbparts, nbparams);
    new IGESData_IGESReaderData((lesect[3] + 1) / 2, nbparams);
  {
    {
      try
      {
        OCC_CATCH_SIGNALS
        IGESFile_ReadHeader(IR);
      } // end attempt 1 (global)
      catch (Standard_Failure const&)
      {
        // Sending of message : Internal error during the header reading
        Message_Msg Msg11 = Message_Msg("XSTEP_11");
        IGESFile_Check(1, Msg11);
      }
    }

    {
      try
      {
        OCC_CATCH_SIGNALS
        if (nbparts > 0)
        {
          IGESFile_ReadContent(IR);
        }

        // Sending of message : Loaded data
      } // end attempt 2 (entities)
      catch (Standard_Failure const&)
      {
        // Sending of message : Internal error during the content reading
        if (THE_RECOVERY_PARAMETER == 0)
        {
          Message_Msg Msg13 = Message_Msg("XSTEP_13");
          Msg13.Arg(THE_RECOVERY_ENTITY);
          IGESFile_Check(1, Msg13);
        }
        else
        {
          Message_Msg Msg14 = Message_Msg("XSTEP_14");
          Msg14.Arg(THE_RECOVERY_ENTITY);
          Msg14.Arg(THE_RECOVERY_PARAMETER);
          IGESFile_Check(1, Msg14);
        }
      }
    }
  }

  int nbr = IR->NbRecords();
  // Sending of message : Number of total loaded entities
  Msg15.Arg(nbr);
  IGESFile_Check(2, Msg15);
  iges_finfile(1);
  IGESData_IGESReaderTool IT(IR, theProtocol);
  IT.Prepare(theRecognizer);
  IT.SetErrorHandle(true);

  // Sending of message : Loading of Model : Beginning
  IT.LoadModel(theModel);
  if (theModel->Protocol().IsNull())
  {
    theModel->SetProtocol(theProtocol);
  }
  iges_finfile(2);

  //  Now, the check
  // Nb warning in global section.
  int nbWarn = checkread()->NbWarnings(), nbFail = checkread()->NbFails();
  const occ::handle<Interface_Check>& oldglob = theModel->GlobalCheck();
  if (nbWarn + nbFail > 0)
  {
    checkread()->GetMessages(oldglob);
    theModel->SetGlobalCheck(checkread());
  }

  checkread()->Trace(0, 1);

  return 0;
}

// Internal breakdown

void IGESFile_ReadHeader(const occ::handle<IGESData_IGESReaderData>& theReaderData)
{
  int   l = 0; // szv#4:S4163:12Mar99 i,j,k not needed
  char* parval;
  int   typarg;
  //  first the start lines (comments)
  // szv#4:S4163:12Mar99 optimized
  /*
    while ( (j = iges_lirparam(&typarg,&parval)) != 0) {
      k = -1;
      for (int j = 72; j >= 0; j --) {
        if (parval[j] > 32) {  k = j;  break;  }
      }
      parval[k+1] = '\0';
      if (k >= 0 || l > 0) IR->AddStartLine (parval);
      l ++;
    }
    //  then the Global Section
    iges_setglobal();
    while ( (i = iges_lirparam(&typarg,&parval)) != 0) {
      IR->AddGlobal(THE_PARAMETER_TYPES[typarg], parval);
    }
  */
  while (iges_lirparam(&typarg, &parval) != 0)
  {
    int j; // svv Jan11 2000 : porting on DEC
    for (j = 72; j >= 0; j--)
    {
      if (parval[j] > 32)
      {
        break;
      }
    }
    parval[j + 1] = '\0';
    if (j >= 0 || l > 0)
    {
      theReaderData->AddStartLine(parval);
    }
    l++;
  }
  //  then the Global Section
  iges_setglobal();
  while (iges_lirparam(&typarg, &parval) != 0)
  {
    theReaderData->AddGlobal(THE_PARAMETER_TYPES[typarg], parval);
  }
  theReaderData->SetGlobalSection();
}

void IGESFile_ReadContent(const occ::handle<IGESData_IGESReaderData>& theReaderData)
{
  char *res1, *res2, *nom, *num;
  char* parval;
  int*  v;
  int   typarg;
  int   nbparam;

  int ns; // szv#4:S4163:12Mar99 i unused
  while ((ns = iges_lirpart(&v, &res1, &res2, &nom, &num, &nbparam)) != 0)
  {
    THE_RECOVERY_PARAMETER = 0;
    THE_RECOVERY_ENTITY    = (ns + 1) / 2; // numero entite
    theReaderData->SetDirPart(THE_RECOVERY_ENTITY,
                              v[0],
                              v[1],
                              v[2],
                              v[3],
                              v[4],
                              v[5],
                              v[6],
                              v[7],
                              v[8],
                              v[9],
                              v[10],
                              v[11],
                              v[12],
                              v[13],
                              v[14],
                              v[15],
                              v[16],
                              res1,
                              res2,
                              nom,
                              num);
    while (iges_lirparam(&typarg, &parval) != 0)
    { // szv#4:S4163:12Mar99 `i=` not needed
      ++THE_RECOVERY_PARAMETER;
      if (typarg == IGES_ParameterType_Integer || typarg == IGES_ParameterType_SignedInteger)
      {
        int nument = std::atoi(parval);
        if (nument < 0)
        {
          nument = -nument;
        }
        if (nument & 1)
        {
          nument = (nument + 1) / 2;
        }
        else
        {
          nument = 0;
        }
        theReaderData->AddParam(THE_RECOVERY_ENTITY, parval, THE_PARAMETER_TYPES[typarg], nument);
      }
      else
      {
        theReaderData->AddParam(THE_RECOVERY_ENTITY, parval, THE_PARAMETER_TYPES[typarg]);
      }
    }
    theReaderData->InitParams(THE_RECOVERY_ENTITY);
    iges_nextpart();
  }
}

void IGESFile_Check(int theMode, Message_Msg& theMessage)
{
  // MGE 20/07/98
  switch (theMode)
  {
    case 0:
      checkread()->SendFail(theMessage);
      break;
    case 1:
      checkread()->SendWarning(theMessage);
      break;
    case 2:
      checkread()->SendMsg(theMessage);
      break;
    default:
      checkread()->SendMsg(theMessage);
  }
  // checkread().Trace(3,-1);
}

void IGESFile_Check2(int theMode, const char* theCode, int theNumber, const char* theText)
{
  // MGE 20/07/98
  Message_Msg aMessage(theCode);
  aMessage.Arg(theNumber);
  aMessage.Arg(theText);

  switch (theMode)
  {
    case 0:
      checkread()->SendFail(aMessage);
      break;
    case 1:
      checkread()->SendWarning(aMessage);
      break;
    case 2:
      checkread()->SendMsg(aMessage);
      break;
    default:
      checkread()->SendMsg(aMessage);
  }
  // checkread().Trace(3,-1);
}

void IGESFile_Check3(int theMode, const char* theCode)
{
  // MGE 20/07/98
  Message_Msg aMessage(theCode);
  switch (theMode)
  {
    case 0:
      checkread()->SendFail(aMessage);
      break;
    case 1:
      checkread()->SendWarning(aMessage);
      break;
    case 2:
      checkread()->SendMsg(aMessage);
      break;
    default:
      checkread()->SendMsg(aMessage);
  }
  // checkread().Trace(3,-1);
}
