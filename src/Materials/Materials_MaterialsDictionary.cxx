// Created on: 1993-01-18
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Materials_MaterialsDictionary.ixx>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if defined (HAVE_SYS_STAT_H) || defined (WNT)
# include <sys/stat.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#include <Standard_Stream.hxx>

#include <Materials.hxx>
#include <Materials_MaterialsSequence.hxx>
#include <Materials_Material.hxx>
#include <Materials_Color.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>

#ifdef WNT
#define stat _stat
#endif
//#define strcasecmp _stricoll
#include <stdio.h>
//#endif

//=======================================================================
//function : Materials_MaterialsDictionary
//purpose  : 
//=======================================================================

Materials_MaterialsDictionary::Materials_MaterialsDictionary()
{
  Standard_Integer i,fr,begin,end,lengthname;
  //char* filename;
  
  char line[256];
  char name[81];
  char type[81];
  char value1[81],value2[81],value3[81];
  Handle(Materials_MaterialsSequence) materialssequence;
  Handle(Materials_Material) material;
  Handle(Materials_Color) pcolor;
  
  struct stat buf;

  Standard_CString filename = Materials::MaterialsFile();

  ifstream file(filename);
  if(!file)
    {
      cout<<"unable to open "<<filename<<" for input"<<endl;
      return;
    }
  
  thefilename = new TCollection_HAsciiString(filename);

  if(!stat(filename, &buf)) thetime = buf.st_ctime;

  thematerialssequence = new Materials_MaterialsSequence();
  
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
          material = new Materials_Material(line);
          thematerialssequence->Append(material);
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

      memset(name, 0, sizeof(name));

	  lengthname = 0;
	  for(i=begin+1; i<=end-1; i++)name[lengthname++] = line[i];

      memset(type,  0,sizeof(type));
      memset(value1,0,sizeof(value1));
      memset(value2,0,sizeof(value2));
      memset(value3,0,sizeof(value3));

          fr = sscanf(&line[end+1],"%80s%80s%80s%80s",type,value1,value2,value3);

          if(fr == -1) continue;

	  if     (!strcasecmp(type,"Materials_Color"))
            {
              Quantity_Color color
		(Atof(value1),Atof(value2),Atof(value3),Quantity_TOC_RGB);
              pcolor = new Materials_Color(color);
              material->Parameter(name,pcolor);
            }
	  else if(!strcasecmp(type,"Standard_Real"))
            {
              material->Parameter(name,Atof(value1));
            }
          else if(!strcasecmp(type,"Standard_CString"))
            {
              material->Parameter(name,value1);
            }
        }
    }
  file.close();
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================

Handle(Materials_Material) Materials_MaterialsDictionary::Material
       (const Standard_CString amaterial) const
{
  Handle(Materials_Material) material;

  for(Standard_Integer index=1;index<=thematerialssequence->Length();index++)
    {
      material = thematerialssequence->Value(index);
      if(material->Name() == amaterial) return material;
    }
  Standard_NoSuchObject::Raise("Material not in the dictionary");
// Pour compil sur NT ....
  return material;
}

Standard_Boolean  Materials_MaterialsDictionary::ExistMaterial(const Standard_CString amaterial) const
{
  Handle(Materials_Material) material;

  for(Standard_Integer index=1;index<=thematerialssequence->Length();index++)
    {
      material = thematerialssequence->Value(index);
      if(material->Name() == amaterial) return Standard_True;
    }
  return Standard_False;
}

//=======================================================================
//function : NumberOfMaterials
//purpose  : 
//=======================================================================

Standard_Integer Materials_MaterialsDictionary::NumberOfMaterials() const
{
  return thematerialssequence->Length();
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================

Handle(Materials_Material) Materials_MaterialsDictionary::Material
       (const Standard_Integer anindex) const
{
  return thematerialssequence->Value(anindex);
}

//=======================================================================
//function : UpToDate
//purpose  : 
//=======================================================================

Standard_Boolean Materials_MaterialsDictionary::UpToDate() const
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
//function : Dump
//purpose  : 
//=======================================================================

void Materials_MaterialsDictionary::Dump(Standard_OStream& astream) const
{
  Standard_Integer index;
  Handle(Materials_Material) material;

  for(index=1;index<=thematerialssequence->Length();index++)
    {
      material = thematerialssequence->Value(index);
      material->Dump(astream);
    }
}

