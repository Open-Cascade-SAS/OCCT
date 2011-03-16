//
//-- File:        MFT.cxx
//-- Created:     Wed Mar 12 17:49:10 1997
//-- Author:      Gerard GRAS 
//--              <gg@photox> 
//-- Update:
//--	reason :
//--	action :
//---Copyright:    Matra Datavision 1993
//

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
