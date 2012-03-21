// Created on: 1993-01-18
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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
  
  char line[255];
  char name[80];
  char type[80];
  char value1[80],value2[80],value3[80];
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
      for(i=0; i<255; i++) line[i]=0;
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

	  for(i=0; i<80; i++)name[i]=0;

	  lengthname = 0;
	  for(i=begin+1; i<=end-1; i++)name[lengthname++] = line[i];

	  for(i=0; i<80; i++)type   [i] = 0;
	  for(i=0; i<80; i++)value1 [i] = 0;
	  for(i=0; i<80; i++)value2 [i] = 0;
	  for(i=0; i<80; i++)value3 [i] = 0;

	  //          fr = sscanf(&line[end+1],"%s%s%s%s",&type,&value1,&value2,&value3);
          fr = sscanf(&line[end+1],"%s%s%s%s",type,value1,value2,value3);

          if(fr == -1) continue;

	  if     (!strcasecmp(type,"Materials_Color"))
            {
              Quantity_Color color
		(atof(value1),atof(value2),atof(value3),Quantity_TOC_RGB);
              pcolor = new Materials_Color(color);
              material->Parameter(name,pcolor);
            }
	  else if(!strcasecmp(type,"Standard_Real"))
            {
              material->Parameter(name,atof(value1));
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

