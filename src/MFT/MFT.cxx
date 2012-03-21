// Created on: 1997-03-12
// Created by: Gerard GRAS 
// Copyright (c) 1997-1999 Matra Datavision
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

#include <MFT.hxx>
#include <TCollection_AsciiString.hxx>

// =============================================================================
//        ---Purpose: Converts a TypeOfCommand to a string
// =============================================================================

static TCollection_AsciiString typestring;
Standard_CString MFT::Convert(const MFT_TypeOfCommand aCommandType){
    switch (aCommandType) {
      case MFT_TOC_UNKNOWN:
	typestring = "UNKNOWN";
        break;
      case MFT_TOC_HSTEM:                                                     
	typestring = "HSTEM";
        break;
      case MFT_TOC_VSTEM:
	typestring = "VSTEM";
	break;
      case MFT_TOC_VMOVETO:
	typestring = "VMOVETO";
	break;
      case MFT_TOC_RLINETO:
	typestring = "RLINETO";
	break;
      case MFT_TOC_HLINETO:
	typestring = "HLINETO";
	break;
      case MFT_TOC_VLINETO:
	typestring = "VLINETO";
	break;
      case MFT_TOC_RRCURVETO:
	typestring = "RRCURVETO";
	break;
      case MFT_TOC_CLOSEPATH:
	typestring = "CLOSEPATH";
	break;
      case MFT_TOC_HSBW:
	typestring = "HSBW";
	break;
      case MFT_TOC_ENDCHAR:
	typestring = "ENDCHAR";
	break;
      case MFT_TOC_RMOVETO:
	typestring = "RMOVETO";
	break;
      case MFT_TOC_HMOVETO:
	typestring = "HMOVETO";
	break;
      case MFT_TOC_VHCURVETO:
	typestring = "VHCURVETO";
	break;
      case MFT_TOC_HVCURVETO:
	typestring = "HVCURVETO";
	break;
      case MFT_TOC_DOTSECTION:
	typestring = "DOTSECTION";
	break;
      case MFT_TOC_VSTEM3:
	typestring = "VSTEM3";
	break;
      case MFT_TOC_HSTEM3:
	typestring = "HSTEM3";
	break;
      case MFT_TOC_SEAC:
	typestring = "SEAC";
	break;
      case MFT_TOC_SBW:
	typestring = "SBW";
	break;
      case MFT_TOC_SETCURRENTPOINT:
	typestring = "SETCURRENTPOINT";
	break;
      case MFT_TOC_CHARSTRING:
	typestring = "CHARSTRING";
	break;
      case MFT_TOC_NUMERIC:
	typestring = "NUMERIC";
	break;
      case MFT_TOC_MINMAX:
	typestring = "MINMAX";
	break;
    }

    return typestring.ToCString();
}

static TCollection_AsciiString valuename;
Standard_CString MFT::Convert(const MFT_TypeOfValue aValueType){
    switch (aValueType) {
      case MFT_TOV_UNKNOWN:
	valuename = "UNKNOWN";
        break;
      case MFT_TOV_INTEGER:
	valuename = "INTEGER";
        break;
      case MFT_TOV_FLOAT:
	valuename = "FLOAT";
        break;
      case MFT_TOV_STRING:
	valuename = "STRING";
        break;
    }

    return valuename.ToCString();
}
