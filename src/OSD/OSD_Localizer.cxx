// File     : OSD_Localizer.cdl
// Created  : 27 August 2010
// Author   : Paul SUPRYATKIN
// Copyright: Open CASCADE 2010

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


