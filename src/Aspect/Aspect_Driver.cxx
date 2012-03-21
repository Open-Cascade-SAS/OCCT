// Copyright (c) 1995-1999 Matra Datavision
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

/***********************************************************************

     FUNCTION :
     ----------
        Class Aspect_Driver :

     HISTORY OF MODIFICATIONS   :
     --------------------------------

      14-05-98 : GG ; Disable using MFT when the symbol
			CSF_MDTVFontDirectory is not defined.
		      See dirMFTisDefined changes. 

      30-01-98 : GG ; SPEC_MFT
              Use of polices MFT becomes parameterized.
	      The driver should use method UseMFT() to
	      know if it is necessary or not to use MDTV policies
	      instead of system policies.

		-> Modifications in SetFontMap()
		-> New method UseMFT()

***********************************************************************/

#include <Aspect_Driver.ixx>
#include <OSD_Environment.hxx>

static Standard_Boolean dirMFTisDefined = Standard_False;

Aspect_Driver::Aspect_Driver () {

  OSD_Environment CSF_MDTVFontDirectory ;

  char *fontdir , *casroot ;
  fontdir =  getenv("CSF_MDTVFontDirectory" );
  if ( !fontdir) {
    casroot  = getenv("CASROOT");
    if ( casroot ) {
      TCollection_AsciiString CasRootString (casroot);
      CasRootString += "/src/FontMFT" ; 
      CSF_MDTVFontDirectory = OSD_Environment(CasRootString.ToCString());
    } else {
      cout << " CASROOT or CSF_MDTVFontDirectory  are mandatory to use this fonctionnality " << endl;
      Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory and CASROOT not setted " );
    }
  } else {
    CSF_MDTVFontDirectory = OSD_Environment("CSF_MDTVFontDirectory");
  }
  TCollection_AsciiString dir(CSF_MDTVFontDirectory.Value());

  dirMFTisDefined = (dir.Length() > 0);
  myUseMFT = Standard_True;
  myColorMapIsDefined = Standard_False;   
  myWidthMapIsDefined = Standard_False;   
  myTypeMapIsDefined  = Standard_False;   
  myFontMapIsDefined  = Standard_False;   
}


void Aspect_Driver::SetColorMap (const Handle(Aspect_ColorMap)& aColorMap) 
 {
   myColorMap = aColorMap;
   this->InitializeColorMap(aColorMap);
   myColorMapIsDefined = Standard_True;
}
Handle(Aspect_ColorMap) Aspect_Driver::ColorMap () const {

  Aspect_UndefinedMap_Raise_if(!myColorMapIsDefined,"ColorMap");
  return myColorMap;
}


void Aspect_Driver::SetTypeMap (const Handle(Aspect_TypeMap)& aTypeMap) 
 {
   myTypeMap = aTypeMap;
   this->InitializeTypeMap(aTypeMap);
   myTypeMapIsDefined = Standard_True;
}
Handle(Aspect_TypeMap) Aspect_Driver::TypeMap () const {

  Aspect_UndefinedMap_Raise_if(!myTypeMapIsDefined,"TypeMap");
  return myTypeMap;
}

void Aspect_Driver::SetWidthMap (const Handle(Aspect_WidthMap)& aWidthMap) 
 {
   myWidthMap = aWidthMap;
   this->InitializeWidthMap(aWidthMap);
   myWidthMapIsDefined = Standard_True;
}
Handle(Aspect_WidthMap) Aspect_Driver::WidthMap () const {

  Aspect_UndefinedMap_Raise_if(!myWidthMapIsDefined,"WidthMap");
  return myWidthMap;
}

void Aspect_Driver::SetFontMap (const Handle(Aspect_FontMap)& aFontMap,
				const Standard_Boolean useMFT) 
 {
   myUseMFT = useMFT;
   myFontMap = aFontMap;
   this->InitializeFontMap(aFontMap);
   myFontMapIsDefined = Standard_True;
}
Handle(Aspect_FontMap) Aspect_Driver::FontMap () const {

  Aspect_UndefinedMap_Raise_if(!myFontMapIsDefined,"FontMap");
  return myFontMap;
}

void Aspect_Driver::SetMarkMap (const Handle(Aspect_MarkMap)& aMarkMap) 
 {
   myMarkMap = aMarkMap;
   this->InitializeMarkMap(aMarkMap);
   myMarkMapIsDefined = Standard_True;
}
Handle(Aspect_MarkMap) Aspect_Driver::MarkMap () const {

  Aspect_UndefinedMap_Raise_if(!myMarkMapIsDefined,"MarkMap");
  return myMarkMap;
}

Standard_Boolean Aspect_Driver::UseMFT () const {
  return myUseMFT & dirMFTisDefined;
}
