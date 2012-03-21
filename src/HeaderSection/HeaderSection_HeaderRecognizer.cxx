// Created on: 1994-06-27
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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
