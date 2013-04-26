// Created on: 1994-01-14
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


#include <Materials_MaterialDefinition.ixx>
#include <Dynamic_ObjectParameter.hxx>
#include <Materials_Color.hxx>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
//#ifdef WNT
//#define strcasecmp _stricoll
#include <stdio.h>
//#endif

//=======================================================================
//function : Materials_MaterialDefinition
//purpose  : 
//=======================================================================

Materials_MaterialDefinition::Materials_MaterialDefinition()
  : Dynamic_FuzzyDefinitionsDictionary()
{}

//=======================================================================
//function : Switch
//purpose  : 
//=======================================================================

Handle(Dynamic_Parameter) Materials_MaterialDefinition::Switch(
  const Standard_CString aname,
  const Standard_CString atype,
  const Standard_CString avalue) const
{
  Standard_Integer fr;
  char value1[81],value2[81],value3[81];
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_ObjectParameter) objectparameter;

  if     (!strcasecmp(atype,"Materials_Color"))
    {
      memset(value1,0,sizeof(value1));
      memset(value2,0,sizeof(value2));
      memset(value3,0,sizeof(value3));

      fr = sscanf(avalue,"%80s%80s%80s",value1,value2,value3);
      
      Handle(Materials_Color) pcolor =
	new Materials_Color(Quantity_Color(Atof(value1),
					    Atof(value2),
					    Atof(value3),
					    Quantity_TOC_RGB));
      objectparameter = new Dynamic_ObjectParameter(aname,pcolor);
      parameter = *(Handle_Dynamic_Parameter*)&objectparameter;
    }

  else
    {
      parameter = Dynamic_FuzzyDefinitionsDictionary::Switch(aname,atype,avalue);
    }

  return parameter;
}



