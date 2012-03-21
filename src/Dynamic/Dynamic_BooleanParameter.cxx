// Created on: 1994-01-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
