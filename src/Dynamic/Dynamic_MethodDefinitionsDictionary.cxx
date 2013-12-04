// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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
// CRD : 15/04/97 : Passage WOK++ : Replace TYPE by STANDARD_TYPE
// CRD : 03/07/97 : Porting Windows NT.

#include <Standard_Stream.hxx>

#include <Dynamic_MethodDefinitionsDictionary.ixx>
#include <Dynamic.hxx>
#include <Dynamic_CompiledMethod.hxx>
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
#define stat _stat
//#define strcasecmp _stricoll
#endif

//=======================================================================
//function : Dynamic_MethodDefinitionsDictionary
//purpose  : 
//=======================================================================

Dynamic_MethodDefinitionsDictionary::Dynamic_MethodDefinitionsDictionary()
{
}

//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

void Dynamic_MethodDefinitionsDictionary::Creates(const Standard_CString afilename)
{
  Standard_Boolean group;
  Standard_Integer fr,i,begin,end,endline;
  char line[256];
  char name[81];
  char mode[81];
  char type[81];
  char value[81],value1[81],value2[81],value3[81];
  Handle(Dynamic_CompiledMethod) methoddefinition;
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

  thesequenceofmethoddefinitions = new Dynamic_SequenceOfMethodDefinitions();
  
  for(;;)
    {
      memset(line,0,sizeof(line));

      file.getline(line,255);
      if(!file)break;

      i = 254;
      while( i >= 0 && ( line[i] == ' ' || !line[i]))line[i--] = 0;
      fr = i+1;
      if(fr <= 1)continue;

      if(line[0] != ' ')
	{
	  methoddefinition = new Dynamic_CompiledMethod(line,line);
	  thesequenceofmethoddefinitions->Append(methoddefinition);
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

      memset(name,0,sizeof(name));

	  endline = 0;
	  for(i=begin+1; i<=end-1; i++)name[endline++] = line[i];

      memset(mode,  0x00,sizeof(mode));
      memset(type,  0x00,sizeof(type));
      memset(value, 0x00,sizeof(value));
      memset(value1,0x00,sizeof(value1));
      memset(value2,0x00,sizeof(value2));
      memset(value3,0x00,sizeof(value3));

	  fr = sscanf(&line[end+1],"%80s%80s%80c",mode,type,value);
	  if(fr == -1) continue;

	  group = Standard_False;
	  if(type[0] == '[')
	    {
	      group = Standard_True;
	      for(i=1; i<80; i++)type[i-1] = type[i];
	    }

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
	    methoddefinition->AddVariable(new Dynamic_BooleanParameter(name,value),Dynamic::Mode(mode),group);

	  else if(!strcasecmp(type,"Standard_Integer"))
	    methoddefinition->AddVariable(new Dynamic_IntegerParameter(name,atoi(value)),Dynamic::Mode(mode),group);

	  else if(!strcasecmp(type,"Standard_Real"))
	    methoddefinition->AddVariable(new Dynamic_RealParameter(name,Atof(value)),Dynamic::Mode(mode),group);

	  else if(!strcasecmp(type,"Standard_CString"))
	    methoddefinition->AddVariable(new Dynamic_StringParameter(name,value),Dynamic::Mode(mode),group);

	  else
	    {
	      parameter = Switch(name,type,value);
	      if(!parameter.IsNull())methoddefinition->AddVariable(parameter,Dynamic::Mode(mode),group);
	    }

	}
    }
  file.close();
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_MethodDefinitionsDictionary::Definition(const Handle(Dynamic_Method)& adefinition)
{
  Handle(Dynamic_MethodDefinition) methoddefinition;
  if(adefinition->IsKind(STANDARD_TYPE(Dynamic_MethodDefinition)))
    {
      methoddefinition = *(Handle_Dynamic_MethodDefinition*)&adefinition;
      thesequenceofmethoddefinitions->Append(methoddefinition);
      return Standard_True;
    }
  else
    {
      cout<<"the definition is not a method definition"<<endl;
      return Standard_False;
    }
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_MethodDefinitionsDictionary::Definition
    (const Standard_CString atype,
     Handle(Dynamic_Method)& adefinition) const
{
  Handle(Dynamic_Method) definition;

  for(Standard_Integer index=1; index<=thesequenceofmethoddefinitions->Length(); index++)
    {
      definition = thesequenceofmethoddefinitions->Value(index);
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

Handle(Dynamic_Parameter) Dynamic_MethodDefinitionsDictionary::Switch(
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

Standard_Boolean Dynamic_MethodDefinitionsDictionary::UpToDate() const
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

Standard_Integer Dynamic_MethodDefinitionsDictionary::NumberOfDefinitions() const
{
  return thesequenceofmethoddefinitions->Length();
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_MethodDefinitionsDictionary::Definition
      (const Standard_Integer anindex) const
{
  return thesequenceofmethoddefinitions->Value(anindex);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_MethodDefinitionsDictionary::Dump(Standard_OStream& astream) const
{
  Standard_Integer index;
  astream<<" DICTIONARY : \n\n";
  for(index=1;index<=thesequenceofmethoddefinitions->Length();index++)
    thesequenceofmethoddefinitions->Value(index)->Dump(astream);
}
