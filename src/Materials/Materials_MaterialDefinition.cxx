// Created on: 1994-01-14
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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



