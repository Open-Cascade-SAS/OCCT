// File:	HeaderSection_HeaderRecognizer.cxx
// Created:	Mon Jun 27 17:43:28 1994
// Author:	Frederic MAUPAS
//		<fma@nonox>

#include <HeaderSection_HeaderRecognizer.ixx>

#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <HeaderSection_FileDescription.hxx>



static TCollection_AsciiString reco_FileName    ("FILE_NAME");
static TCollection_AsciiString reco_FileSchema ("FILE_SCHEMA");
static TCollection_AsciiString reco_FileDescription ("FILE_DESCRIPTION");



HeaderSection_HeaderRecognizer::HeaderSection_HeaderRecognizer ()
{ }

void HeaderSection_HeaderRecognizer::Eval
  (const TCollection_AsciiString& key)
{
  if (key.IsEqual(reco_FileName)) { 
    SetOK(new HeaderSection_FileName);
    return;
  }
  if (key.IsEqual(reco_FileSchema)) { 
    SetOK(new HeaderSection_FileSchema);
    return;
  }
  if (key.IsEqual(reco_FileDescription)) { 
    SetOK(new HeaderSection_FileDescription);
    return;
  } 
}
