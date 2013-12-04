// Copyright (c) 1995-1999 Matra Datavision
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


Aspect_Driver::Aspect_Driver () {

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

void Aspect_Driver::SetFontMap (const Handle(Aspect_FontMap)& aFontMap) 
 {
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

