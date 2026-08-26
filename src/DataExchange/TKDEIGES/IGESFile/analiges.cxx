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

#include <cstdlib>

static constexpr int THE_DECIMAL_POWERS[] =
  {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

//! Decodes an integer stored in a fixed-width field.
//! @param[in] theLine source record
//! @param[in] theStart zero-based start of the field
//! @param[in] theLength number of characters in the field
//! @return decoded integer value
static int IGES_decode(const char* theLine, int theStart, int theLength)
{
  int       aValue        = 0;
  const int aLastPosition = theStart + theLength - 1;
  for (int anIndex = 0; anIndex < theLength; ++anIndex)
  {
    const char aCharacter = theLine[aLastPosition - anIndex];
    switch (aCharacter)
    {
      case ' ':
        return aValue;
      case '+':
      case '0':
        break;
      case '-':
        aValue = -aValue;
        break;
      default:
        aValue += (aCharacter - '0') * THE_DECIMAL_POWERS[anIndex];
        break;
    }
  }
  return aValue;
}

//! Copies a fixed-width string and appends a null terminator.
//! @param[in] theLine source record
//! @param[in] theStart zero-based start of the field
//! @param[in] theLength number of characters to copy
//! @param[out] theDestination destination buffer
static void IGES_copstr(const char* theLine, int theStart, int theLength, char* theDestination)
{
  for (int anIndex = 0; anIndex < theLength; ++anIndex)
  {
    theDestination[anIndex] = theLine[theStart + anIndex];
  }
  theDestination[theLength] = '\0';
}

void iges_Dsect(int* theDirectoryStatus, int theSectionNumber, char* theLine)
{
  dirpart* aCurrentPart;
  if (*theDirectoryStatus == 0)
  {
    iges_newpart(theSectionNumber);
    aCurrentPart        = iges_get_curp();
    aCurrentPart->typ   = IGES_decode(theLine, 0, 8);
    aCurrentPart->poi   = IGES_decode(theLine, 8, 8);
    aCurrentPart->pdef  = IGES_decode(theLine, 16, 8);
    aCurrentPart->tra   = IGES_decode(theLine, 24, 8);
    aCurrentPart->niv   = IGES_decode(theLine, 32, 8);
    aCurrentPart->vue   = IGES_decode(theLine, 40, 8);
    aCurrentPart->trf   = IGES_decode(theLine, 48, 8);
    aCurrentPart->aff   = IGES_decode(theLine, 56, 8);
    aCurrentPart->blk   = IGES_decode(theLine, 64, 2);
    aCurrentPart->sub   = IGES_decode(theLine, 66, 2);
    aCurrentPart->use   = IGES_decode(theLine, 68, 2);
    aCurrentPart->her   = IGES_decode(theLine, 70, 2);
    *theDirectoryStatus = 1;
  }
  else if (*theDirectoryStatus == 1)
  {
    aCurrentPart       = iges_get_curp();
    aCurrentPart->typ2 = IGES_decode(theLine, 0, 8);
    aCurrentPart->epa  = IGES_decode(theLine, 8, 8);
    aCurrentPart->col  = IGES_decode(theLine, 16, 8);
    aCurrentPart->nbl  = IGES_decode(theLine, 24, 8);
    aCurrentPart->form = IGES_decode(theLine, 32, 8);
    IGES_copstr(theLine, 40, 8, aCurrentPart->res1);
    IGES_copstr(theLine, 48, 8, aCurrentPart->res2);
    IGES_copstr(theLine, 56, 8, aCurrentPart->nom);
    IGES_copstr(theLine, 64, 8, aCurrentPart->num);
    *theDirectoryStatus = 0;
  }
}

void iges_Psect(int theSectionNumber, char theLine[80])
{
  const int aDirectoryNumber = std::atoi(&theLine[65]);
  theLine[64]                = '\0';
  iges_curpart(aDirectoryNumber);
  static_cast<void>(theSectionNumber);
}

static thread_local int                THE_HOLLERITH_LENGTH = 0;
static thread_local int                THE_PARAMETER_OFFSET = 0;
static thread_local int                THE_CONTINUATION     = 0;
static thread_local IGES_ParameterType THE_PARAMETER_TYPE;

void iges_param(int*  theParameterStatus,
                char* theLine,
                char  theSeparator,
                char  theTerminator,
                int   theLineLength)
{
  int  aParameterIndex;
  int  aValueStart;
  int  aLookaheadIndex;
  char aParameter[80];
  char aCharacter;

  if (*theParameterStatus == 0)
  {
    THE_CONTINUATION = 0;
  }
  if (*theParameterStatus != 2)
  {
    THE_PARAMETER_OFFSET = 0;
  }
  if (*theParameterStatus < 3)
  {
    THE_HOLLERITH_LENGTH = 0;
  }
  else
  {
    THE_PARAMETER_OFFSET = THE_HOLLERITH_LENGTH;
    if (THE_PARAMETER_OFFSET > theLineLength)
    {
      iges_addparam(theLineLength, theLine);
      THE_HOLLERITH_LENGTH -= theLineLength;
      return;
    }
    else
    {
      iges_addparam(THE_HOLLERITH_LENGTH, theLine);
      THE_HOLLERITH_LENGTH = 0;
    }
  }

  aValueStart        = 0;
  THE_PARAMETER_TYPE = IGES_ParameterType_Void;
  for (aParameterIndex = 0; (aCharacter = theLine[THE_PARAMETER_OFFSET + aParameterIndex]) != '\0';
       aParameterIndex++)
  {
    if (aCharacter == theSeparator)
    {
      *theParameterStatus         = 2;
      aParameter[aParameterIndex] = '\0';
      if (THE_CONTINUATION == 0)
      {
        iges_newparam(THE_PARAMETER_TYPE,
                      aParameterIndex - aValueStart + 1,
                      &aParameter[aValueStart]);
      }
      else if (THE_CONTINUATION > 0)
      {
        iges_addparam(aParameterIndex - aValueStart + 1, &aParameter[aValueStart]);
      }
      THE_CONTINUATION = 0;
      for (aLookaheadIndex = aParameterIndex + 1;
           (aCharacter = theLine[THE_PARAMETER_OFFSET + aLookaheadIndex]) != '\0';
           aLookaheadIndex++)
      {
        if (aCharacter != ' ')
        {
          THE_PARAMETER_OFFSET += aParameterIndex + 1;
          return;
        }
      }
      *theParameterStatus = 1;
      return;
    }
    if (aCharacter == theTerminator)
    {
      *theParameterStatus         = 1;
      aParameter[aParameterIndex] = '\0';
      if (THE_CONTINUATION == 0)
      {
        iges_newparam(THE_PARAMETER_TYPE,
                      aParameterIndex - aValueStart + 1,
                      &aParameter[aValueStart]);
      }
      else if (THE_CONTINUATION > 0)
      {
        iges_addparam(aParameterIndex - aValueStart + 1, &aParameter[aValueStart]);
      }
      THE_CONTINUATION = 0;
      return;
    }
    aParameter[aParameterIndex] = aCharacter;

    if (aCharacter >= '0' && aCharacter <= '9')
    {
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Integer)
      {
        continue;
      }
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Void)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Integer;
      }
      else if (THE_PARAMETER_TYPE == IGES_ParameterType_IncompleteExponent)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_RealExponent;
      }
    }
    else if (aCharacter == '+' || aCharacter == '-')
    {
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Void)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_SignedInteger;
      }
      else if (THE_PARAMETER_TYPE != IGES_ParameterType_IncompleteExponent
               && THE_PARAMETER_TYPE != IGES_ParameterType_IntegerExponent)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
      }
    }
    else if (aCharacter == '.')
    {
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Void)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Real;
      }
      else if (THE_PARAMETER_TYPE == IGES_ParameterType_Integer
               || THE_PARAMETER_TYPE == IGES_ParameterType_SignedInteger)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Real;
      }
      else
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
      }
    }
    else if (aCharacter == 'E' || aCharacter == 'e' || aCharacter == 'D' || aCharacter == 'd')
    {
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Real)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_IncompleteExponent;
      }
      else if (THE_PARAMETER_TYPE == IGES_ParameterType_Integer
               || THE_PARAMETER_TYPE == IGES_ParameterType_SignedInteger)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_IntegerExponent;
      }
      else
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
      }
    }
    else if (aCharacter == 'H')
    {
      if (THE_PARAMETER_TYPE != IGES_ParameterType_Integer)
      {
        THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
        continue;
      }
      THE_PARAMETER_TYPE   = IGES_ParameterType_Text;
      THE_HOLLERITH_LENGTH = 0;
      for (aLookaheadIndex = aValueStart; aLookaheadIndex < aParameterIndex; aLookaheadIndex++)
      {
        if (aParameter[aLookaheadIndex] >= '0' && aParameter[aLookaheadIndex] <= '9')
        {
          THE_HOLLERITH_LENGTH = THE_HOLLERITH_LENGTH * 10 + (aParameter[aLookaheadIndex] - '0');
        }
        else
        {
          THE_HOLLERITH_LENGTH = 0;
          break;
        }
      }
      if (THE_PARAMETER_OFFSET + aParameterIndex + THE_HOLLERITH_LENGTH >= theLineLength)
      {
        for (aLookaheadIndex = 1;
             aLookaheadIndex < theLineLength - THE_PARAMETER_OFFSET - aParameterIndex;
             aLookaheadIndex++)
        {
          aParameter[aParameterIndex + aLookaheadIndex] =
            theLine[THE_PARAMETER_OFFSET + aParameterIndex + aLookaheadIndex];
        }
        aParameter[theLineLength - THE_PARAMETER_OFFSET] = '\0';
        THE_HOLLERITH_LENGTH =
          THE_PARAMETER_OFFSET + aParameterIndex + THE_HOLLERITH_LENGTH + 1 - theLineLength;
        *theParameterStatus = 3;
        iges_newparam(THE_PARAMETER_TYPE, theLineLength - aValueStart, &aParameter[aValueStart]);
        THE_CONTINUATION = 1;
        return;
      }
      else
      {
        for (aLookaheadIndex = 1; aLookaheadIndex <= THE_HOLLERITH_LENGTH; aLookaheadIndex++)
        {
          aParameter[aParameterIndex + aLookaheadIndex] =
            theLine[THE_PARAMETER_OFFSET + aParameterIndex + aLookaheadIndex];
        }
        aParameterIndex += THE_HOLLERITH_LENGTH;
      }
    }
    else if (aCharacter == ' ')
    {
      if (THE_PARAMETER_TYPE == IGES_ParameterType_Void)
      {
        aValueStart = aParameterIndex + 1;
      }
      else
      {
        for (aLookaheadIndex = aParameterIndex + 1;
             (aCharacter = theLine[THE_PARAMETER_OFFSET + aLookaheadIndex]) != '\0';
             aLookaheadIndex++)
        {
          if (aCharacter == theSeparator || aCharacter == theTerminator)
          {
            break;
          }
          if (aCharacter != ' ')
          {
            THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
            break;
          }
        }
      }
    }
    else
    {
      THE_PARAMETER_TYPE = IGES_ParameterType_Misc;
    }
  }

  *theParameterStatus         = 1;
  aParameter[aParameterIndex] = '\0';
  THE_CONTINUATION            = -1;
  if (aParameterIndex > aValueStart)
  {
    iges_newparam(THE_PARAMETER_TYPE, aParameterIndex - aValueStart + 1, &aParameter[aValueStart]);
  }
}
