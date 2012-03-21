// Created on: 2010-08-27
// Created by: Paul SUPRYATKIN
// Copyright (c) 2010-2012 OPEN CASCADE SAS
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


#include <OSD_Localizer.hxx>
#include <Standard.hxx>
#include <locale.h>

OSD_Localizer::OSD_Localizer(const Standard_Integer Category,const Standard_CString Locale )
{
  SetLocale( Category, Locale );
}


void OSD_Localizer::Restore()
{
  setlocale( myCategory, myLocale );
}


void OSD_Localizer::SetLocale(const Standard_Integer Category,const Standard_CString Locale )
{
  myLocale = setlocale( Category, 0 );
  myCategory = Category;
  setlocale( Category, Locale );
}

Standard_CString OSD_Localizer::Locale() const
{
  return myLocale;
}

Standard_Integer OSD_Localizer::Category() const
{
  return myCategory;
}


