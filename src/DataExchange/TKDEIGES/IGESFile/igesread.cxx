/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#include "igesread.pxx"

static constexpr char THE_SECTION_NAMES[] = " SGDPT ";

namespace
{
//! Releases parser allocations unless ownership has passed to the caller.
class IgesParserGuard
{
public:
  ~IgesParserGuard()
  {
    if (myIsActive)
    {
      iges_finfile(0);
    }
  }

  //! Keeps parser allocations alive for traversal by the caller.
  void Release() { myIsActive = false; }

private:
  bool myIsActive = true;
};
} // namespace

int igesread_stream(void*                  theContext,
                    IGES_ReadBytesCallback theReadBytes,
                    int                    theSectionCounts[6],
                    bool                   theIsFNES)
{
  char anIGESRecord[100] = {0};
  int  aSectionNumber    = 0;
  int  aRecordNumber     = 0;
  int  aSection          = 0;
  int  aPreviousSection  = 0;
  char aSectionName[2]   = {0};

  int             aDirectoryStatus    = 0;
  int             aParameterStatus    = 0;
  char            aParameterSeparator = ',';
  char            aRecordDelimiter    = ';';
  IGES_InputState anInput             = {theContext, theReadBytes, {0}, 0, 0, false, 0};

  if (!theReadBytes || !theSectionCounts)
  {
    return -1;
  }
  iges_initfile();
  IgesParserGuard aParserGuard;
  for (std::size_t aSectionIndex = 1; aSectionIndex < 6; ++aSectionIndex)
  {
    theSectionCounts[aSectionIndex] = 0;
  }
  for (;;)
  {
    ++aRecordNumber;
    aSection = iges_lire_stream(&anInput, &aSectionNumber, anIGESRecord, theIsFNES);
    if (aSection == IGES_ReadStatus_Error)
    {
      return -1;
    }
    if (aSection <= 0 || aSection < aPreviousSection)
    {
      if (aSection == IGES_ReadStatus_End)
      {
        break;
      }
      aSectionName[0] = THE_SECTION_NAMES[aPreviousSection];
      IGESFile_Check2(0, "XSTEP_18", aRecordNumber, aSectionName);

      if (aPreviousSection == 0)
      {
        return -1;
      }
      ++theSectionCounts[aPreviousSection];
      continue;
    }
    ++theSectionCounts[aSection];
    aPreviousSection = aSection;
    if (aSectionNumber != theSectionCounts[aSection])
    {
      aSectionName[0] = THE_SECTION_NAMES[aPreviousSection];
      IGESFile_Check2(0, "XSTEP_19", aRecordNumber, aSectionName);
    }

    if (aSection == 1)
    {
      anIGESRecord[72] = '\0';
      iges_newparam(0, 72, anIGESRecord);
    }
    if (aSection == 2)
    {
      iges_setglobal();
      for (;;)
      {
        if (theSectionCounts[aSection] == 1)
        {
          std::size_t aParameterStart = 0;
          if (anIGESRecord[0] != ',')
          {
            aParameterSeparator = anIGESRecord[2];
            aParameterStart     = 3;
          }
          if (anIGESRecord[aParameterStart + 1] != aParameterSeparator)
          {
            aRecordDelimiter = anIGESRecord[aParameterStart + 3];
          }
        }
        iges_param(&aParameterStatus, anIGESRecord, aParameterSeparator, aRecordDelimiter, 72);
        if (aParameterStatus != 2)
        {
          break;
        }
      }
    }
    if (aSection == 3)
    {
      iges_Dsect(&aDirectoryStatus, aSectionNumber, anIGESRecord);
    }
    if (aSection == 4)
    {
      iges_Psect(aSectionNumber, anIGESRecord);
      for (;;)
      {
        iges_param(&aParameterStatus, anIGESRecord, aParameterSeparator, aRecordDelimiter, 64);
        if (aParameterStatus != 2)
        {
          break;
        }
      }
    }
  }

  if (theSectionCounts[5] == 0)
  {
    IGESFile_Check3(1, "XSTEP_20");
  }

  aParserGuard.Release();
  return 0;
}
