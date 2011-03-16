// File:	Dynamic_BooleanParameter.cxx
// Created:	Mon Jan 24 13:34:15 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@meteox>
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
