// Created on: 1994-01-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Historique :
// CRD : 03/07/97 : Portage Windows NT.


#include <Dynamic_BooleanParameter.ixx>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef WNT
//#define strcasecmp _stricoll
#include <stdio.h>
#endif

//=======================================================================
//function : Dynamic_BooleanParameter
//purpose  : 
//=======================================================================

Dynamic_BooleanParameter::Dynamic_BooleanParameter
  (const Standard_CString aparameter)
     : Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_BooleanParameter
//purpose  : 
//=======================================================================

Dynamic_BooleanParameter::Dynamic_BooleanParameter
  (const Standard_CString aparameter,
   const Standard_Boolean avalue)
     : Dynamic_Parameter(aparameter)
{
  thevalue = avalue;
}

//=======================================================================
//function : Dynamic_BooleanParameter
//purpose  : 
//=======================================================================

Dynamic_BooleanParameter::Dynamic_BooleanParameter
  (const Standard_CString aparameter,
   const Standard_CString avalue)
     : Dynamic_Parameter(aparameter)
{
  if     (!strcasecmp(avalue,"Standard_True")) thevalue = Standard_True;
  else if(!strcasecmp(avalue,"Standard_False")) thevalue = Standard_False;
  else cout<<"BooleanParameter ("<<avalue<<") n'existe pas"<<endl;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_BooleanParameter::Value() const
{
  return thevalue;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_BooleanParameter::Value(const Standard_Boolean avalue) 
{
  thevalue = avalue;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_BooleanParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
  astream<<" "<<thevalue;
}
