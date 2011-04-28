// File:	Dynamic_FuzzyDefinitionsDictionary.cxx
// Created:	Wed Jun 24 12:49:59 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@phobox>
// History :
// CRD : 03/07/97 : Porting Windows NT.

#include <Standard_Stream.hxx>

#include <Dynamic_FuzzyDefinitionsDictionary.ixx>
#include <Dynamic_FuzzyDefinition.hxx>
#include <Dynamic_BooleanParameter.hxx>
#include <Dynamic_IntegerParameter.hxx>
#include <Dynamic_RealParameter.hxx>
#include <Dynamic_StringParameter.hxx>
#include <Dynamic_ObjectParameter.hxx>
#include <Dynamic_InstanceParameter.hxx>
#include <TCollection_AsciiString.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if defined (HAVE_SYS_STAT_H) || defined (WNT)
# include <sys/stat.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef WNT
//#define strcasecmp _stricoll
#define stat _stat
#endif

//=======================================================================
//function : Dynamic_FuzzyDefinitionsDictionary
//purpose  : 
//=======================================================================

Dynamic_FuzzyDefinitionsDictionary::Dynamic_FuzzyDefinitionsDictionary()
{
}

//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

void Dynamic_FuzzyDefinitionsDictionary::Creates(const Standard_CString afilename)
{
  Standard_Integer fr,i,begin,end,endline;
  char line[255];
  char name[80];
  char type[80];
  char value[80],value1[80],value2[80],value3[80];
  Handle(Dynamic_FuzzyDefinition) fuzzydefinition;
  Handle(Dynamic_Parameter) parameter;
  
 struct stat buf;

  ifstream file(afilename);
  if(!file)
    {
      cout<<"unable to open "<<afilename<<" for input"<<endl;
      return;
    }
  
  thefilename = new TCollection_HAsciiString(afilename);

  if(!stat(afilename,&buf)) thetime = buf.st_ctime;

  thesequenceoffuzzydefinitions = new Dynamic_SequenceOfFuzzyDefinitions();
  
  for(;;)
    {
      for(i=0; i<255; i++) line[i] = 0;

      file.getline(line,255);
      if(!file)break;

      i = 254;
      while( i >= 0 && ( line[i] == ' ' || !line[i]))line[i--] = 0;
      fr = i+1;
      if(fr <= 1)continue;

      if(line[0] != ' ')
	{
	  fuzzydefinition = new Dynamic_FuzzyDefinition(line);
	  thesequenceoffuzzydefinitions->Append(fuzzydefinition);
	}
      else
	{
	  begin = end = 0;
	  for(i=0; i<fr; i++)
	    {
	      if(line[i] == '"')
		{
		  if(begin)
		    {
		      end = i;
		      break;
		    }
		  else
		    {
		      begin = i;
		    }
		}
	    }

	  for(i=0; i<80; i++)name[i]=0;

	  endline = 0;
	  for(i=begin+1; i<=end-1; i++)name[endline++] = line[i];

	  for(i=0; i<80; i++)type   [i] = 0;
	  for(i=0; i<80; i++)value  [i] = 0;
	  for(i=0; i<80; i++)value1 [i] = 0;
	  for(i=0; i<80; i++)value2 [i] = 0;
	  for(i=0; i<80; i++)value3 [i] = 0;

//	  fr = sscanf(&line[end+1],"%s%80c",&type,&value);
	  fr = sscanf(&line[end+1],"%s%80c",type,value);
	  if(fr == -1) continue;

	  begin = 0;
	  for(i=0; i<80; i++)
	    {
	      if(value[i] != ' ')
		{
		  begin = i;
		  break;
		}
	    }
	  for(i=begin; i<80; i++) value[i-begin] = value[i];
	  for(i=80-begin; i<80; i++) value[i] = 0;

	  if     (!strcasecmp(type,"Standard_Boolean"))
	    fuzzydefinition->Parameter(new Dynamic_BooleanParameter(name,value));

	  else if(!strcasecmp(type,"Standard_Integer"))
	    fuzzydefinition->Parameter(new Dynamic_IntegerParameter(name,atoi(value)));

	  else if(!strcasecmp(type,"Standard_Real"))
	    fuzzydefinition->Parameter(new Dynamic_RealParameter(name,atof(value)));

	  else if(!strcasecmp(type,"Standard_CString"))
	    fuzzydefinition->Parameter(new Dynamic_StringParameter(name,value));

	  else
	    {
	      parameter = Switch(name,type,value);
	      if(!parameter.IsNull())fuzzydefinition->Parameter(parameter);
	    }

	}
    }
  file.close();
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyDefinitionsDictionary::Definition
    (const Standard_CString atype,
     Handle(Dynamic_FuzzyClass)& adefinition) const
{
  Handle(Dynamic_FuzzyClass) definition;

  for(Standard_Integer index=1; index<=thesequenceoffuzzydefinitions->Length(); index++)
    {
      definition = thesequenceoffuzzydefinitions->Value(index);
      if(definition->Type() == atype)
	{
	  adefinition = definition;
	  return Standard_True;
	}
    }
  return Standard_False;
}

//=======================================================================
//function : Switch
//purpose  : 
//=======================================================================

Handle(Dynamic_Parameter) Dynamic_FuzzyDefinitionsDictionary::Switch(
  const Standard_CString aname,
  const Standard_CString atype,
  const Standard_CString avalue) const
{
  Handle(Dynamic_ObjectParameter) objectparameter;
  cout<<"Parameter "<<aname<<" of type "<<atype<<" with "<<avalue<<" does not exist."<<endl;
  return objectparameter;
}

//=======================================================================
//function : UpToDate
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyDefinitionsDictionary::UpToDate() const
{
  struct stat buf;

  TCollection_AsciiString string = thefilename->String();
  if(!stat(string.ToCString(),&buf))
    {
	if(thetime == buf.st_ctime) return Standard_True;
    }

  return Standard_False;
}

//=======================================================================
//function : NumberOfDefinitions
//purpose  : 
//=======================================================================

Standard_Integer Dynamic_FuzzyDefinitionsDictionary::NumberOfDefinitions() const
{
  return thesequenceoffuzzydefinitions->Length();
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Handle(Dynamic_FuzzyClass) Dynamic_FuzzyDefinitionsDictionary::Definition
      (const Standard_Integer anindex) const
{
  return thesequenceoffuzzydefinitions->Value(anindex);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_FuzzyDefinitionsDictionary::Dump(Standard_OStream& astream) const
{
  Standard_Integer index;
  astream<<" DICTIONARY : /n";
  for(index=1;index<=thesequenceoffuzzydefinitions->Length();index++)
    thesequenceoffuzzydefinitions->Value(index)->Dump(astream);
}
