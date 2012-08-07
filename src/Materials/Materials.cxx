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

// ----------------------------------------------------------------
// Historique :
// ----------------------------------------------------------------
// 12/04/98 : CRD : Suppression des cout.
// 22/10/92 : GDE : Creation
// ----------------------------------------------------------------


#include <Materials.hxx>
#include <Materials_MaterialsDictionary.hxx>
#include <Materials_MaterialDefinition.hxx>
#include <Materials_Material.hxx>
#include <Standard_PCharacter.hxx>

static Handle(Materials_MaterialsDictionary) materialsdictionary;
static Handle(Materials_MaterialDefinition) materialdefinition;

static Standard_PCharacter materialfile;
static Standard_PCharacter materialsfile;

void  DictionaryOfDefinitions(Handle(Materials_MaterialDefinition)&);

//=======================================================================
//function : MaterialFile
//purpose  : 
//=======================================================================

void Materials::MaterialFile(const Standard_CString afile)
{
  Standard_Size length = strlen(afile);
  materialfile = new Standard_Character[length+1];
  strcpy(materialfile,afile);
  materialfile[length] = 0;
}

//=======================================================================
//function : MaterialsFile
//purpose  : 
//=======================================================================

void Materials::MaterialsFile(const Standard_CString afile)
{
  Standard_Size length = strlen(afile);
  materialsfile = new Standard_Character[length+1];
  strcpy(materialsfile,afile);
  materialsfile[length] = 0;
}

//=======================================================================
//function : MaterialsFile
//purpose  : 
//=======================================================================

Standard_CString Materials::MaterialsFile()
{
  return materialsfile;
}

//=======================================================================
//function : DictionaryOfMaterials
//purpose  : 
//=======================================================================

Handle(Materials_MaterialsDictionary) Materials::DictionaryOfMaterials()
{
  if(materialsdictionary.IsNull())
    {
      materialsdictionary  = new Materials_MaterialsDictionary();
    }
  else if(!materialsdictionary->UpToDate())
    {
      materialsdictionary  = new Materials_MaterialsDictionary();
    }
  return materialsdictionary;
}


Handle(Materials_Material) Materials::Material(const Standard_CString amaterial)
{
  return (Materials::DictionaryOfMaterials())->Material(amaterial);
}

Standard_Boolean Materials::ExistMaterial(const Standard_CString aName)
{
  return (Materials::DictionaryOfMaterials())->ExistMaterial(aName);
}

void  DictionaryOfDefinitions (Handle(Materials_MaterialDefinition)& adictionary)
{
  if(materialdefinition.IsNull())
    {
      materialdefinition = new Materials_MaterialDefinition();
      materialdefinition->Creates(materialfile);
    }
  else if(!materialdefinition->UpToDate())
    {
      materialdefinition->Creates(materialfile);
    }
  adictionary = materialdefinition;
}

Standard_Integer Materials::NumberOfMaterials()
{
  return (Materials::DictionaryOfMaterials())->NumberOfMaterials();
}

Handle(Materials_Material) Materials::Material(const Standard_Integer anindex)
{
  return (Materials::DictionaryOfMaterials())->Material(anindex);
}

