// Created on: 2013-01-17
// Created by: Kirill GAVRILOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <Standard_CLocaleSentry.hxx>

#include <Standard_TypeDef.hxx>

#include <cstring>

namespace
{

  //! CLocalePtr - static object representing C locale
  class CLocalePtr
  {
  public:

    CLocalePtr()
    #ifdef HAVE_XLOCALE_H
    : myLocale (newlocale (LC_ALL_MASK, "C", NULL))
    #elif defined(_WIN32)
    : myLocale (_create_locale (LC_ALL, "C"))
    #else
    : myLocale (NULL)
    #endif
    {}

    ~CLocalePtr()
    {
    #ifdef HAVE_XLOCALE_H
      freelocale (myLocale);
    #elif defined(_WIN32)
      _free_locale (myLocale);
    #endif
    }

  public:

    Standard_CLocaleSentry::clocale_t myLocale;

  };

  static CLocalePtr theCLocale;

};

// =======================================================================
// function : GetCLocale
// purpose  :
// =======================================================================
Standard_CLocaleSentry::clocale_t Standard_CLocaleSentry::GetCLocale()
{
  return theCLocale.myLocale;
}

// =======================================================================
// function : Standard_CLocaleSentry
// purpose  :
// =======================================================================
Standard_CLocaleSentry::Standard_CLocaleSentry()
#ifdef HAVE_XLOCALE_H
: myPrevLocale (uselocale (theCLocale.myLocale)) // switch to C locale within this thread only using xlocale API
#else
: myPrevLocale (setlocale (LC_ALL, 0))
#if defined(_MSC_VER) && (_MSC_VER > 1400)
, myPrevTLocaleState (_configthreadlocale (_ENABLE_PER_THREAD_LOCALE))
#endif
#endif
{
#ifndef HAVE_XLOCALE_H
  const char* aPrevLocale = (const char* )myPrevLocale;
  if (aPrevLocale[0] == 'C' && aPrevLocale[1] == '\0')
  {
    myPrevLocale = NULL; // already C locale
    return;
  }
  // copy string as following setlocale calls may invalidate returned pointer
  Standard_Size aLen = std::strlen (aPrevLocale) + 1;
  myPrevLocale = new char[aLen];
  memcpy (myPrevLocale, aPrevLocale, aLen);

  setlocale (LC_ALL, "C");
#endif
}

// =======================================================================
// function : ~Standard_CLocaleSentry
// purpose  :
// =======================================================================
Standard_CLocaleSentry::~Standard_CLocaleSentry()
{
#ifdef HAVE_XLOCALE_H
  uselocale ((locale_t )myPrevLocale);
#else
  if (myPrevLocale != NULL)
  {
    const char* aPrevLocale = (const char* )myPrevLocale;
    setlocale (LC_ALL, aPrevLocale);
    delete[] aPrevLocale;
  }
#if defined(_MSC_VER) && (_MSC_VER > 1400)
  if (myPrevTLocaleState != _ENABLE_PER_THREAD_LOCALE)
  {
    _configthreadlocale (myPrevTLocaleState);
  }
#endif
#endif
}
