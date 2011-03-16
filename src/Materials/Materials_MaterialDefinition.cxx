// File:	Materials_MaterialDefinition.cxx
// Created:	Fri Jan 14 10:25:48 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@meteox>
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
  Standard_Integer fr,i;
  char value1[80],value2[80],value3[80];
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_ObjectParameter) objectparameter;

  if     (!strcasecmp(atype,"Materials_Color"))
    {
      for(i=0; i<80; i++)value1[i] = 0;
      for(i=0; i<80; i++)value2[i] = 0;
      for(i=0; i<80; i++)value3[i] = 0;
      //      fr = sscanf(avalue,"%s%s%s",&value1,&value2,&value3);
      fr = sscanf(avalue,"%s%s%s",value1,value2,value3);
      
      Handle(Materials_Color) pcolor =
	new Materials_Color(Quantity_Color(atof(value1),
					    atof(value2),
					    atof(value3),
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



