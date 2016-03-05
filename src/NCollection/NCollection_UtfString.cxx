// Created on: 2016-02-23
// Created by: Kirill Gavrilov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <NCollection_UtfString.hxx>

#if !defined(__ANDROID__)
//=======================================================================
//function : ~NCollection_UtfStringTool
//purpose  :
//=======================================================================
NCollection_UtfStringTool::~NCollection_UtfStringTool()
{
  delete[] myWideBuffer;
}

//=======================================================================
//function : FromLocale()
//purpose  :
//=======================================================================
wchar_t* NCollection_UtfStringTool::FromLocale (const char* theString)
{
  if (myWideBuffer != NULL)
  {
    delete[] myWideBuffer;
    myWideBuffer = NULL;
  }

#if defined(_WIN32)
  // use WinAPI
  int aWideSize = MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, theString, -1, NULL, 0);
  if (aWideSize <= 0)
  {
    return NULL;
  }

  myWideBuffer = new wchar_t[aWideSize + 1];
  MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, theString, -1, myWideBuffer, aWideSize);
  myWideBuffer[aWideSize] = L'\0';
#else
  // this is size in bytes but should probably be enough to store string in wide chars
  // notice that these functions are sensitive to locale set by application!
  int aMbLen = mblen (theString, MB_CUR_MAX);
  if (aMbLen <= 0)
  {
    return NULL;
  }

  myWideBuffer = new wchar_t[aMbLen + 1];
  mbstowcs (myWideBuffer, theString, aMbLen);
  myWideBuffer[aMbLen] = L'\0';
#endif
  return myWideBuffer;
}

//=======================================================================
//function : ToLocale()
//purpose  :
//=======================================================================
bool NCollection_UtfStringTool::ToLocale (const wchar_t*         theWideString,
                                          char*                  theBuffer,
                                          const Standard_Integer theSizeBytes)
{
#if defined(_WIN32)
  int aMbBytes = WideCharToMultiByte (CP_ACP, 0, theWideString, -1, theBuffer, theSizeBytes, NULL, NULL);
#else
  std::size_t aMbBytes = std::wcstombs (theBuffer, theWideString, theSizeBytes);
#endif
  if (aMbBytes <= 0)
  {
    *theBuffer = '\0';
    return false;
  }
  return true;
}
#endif
