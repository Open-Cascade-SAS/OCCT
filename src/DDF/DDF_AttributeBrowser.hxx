// Created by: DAUTRY Philippe
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

//      	------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Oct  6 1997	Creation



#ifndef DDF_AttributeBrowser_HeaderFile
#define DDF_AttributeBrowser_HeaderFile

#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>

class DDF_AttributeBrowser {

  public :

    Standard_EXPORT DDF_AttributeBrowser 
      (Standard_Boolean (*test)(const Handle(TDF_Attribute)&),
       TCollection_AsciiString (*open) (const Handle(TDF_Attribute)&),
       TCollection_AsciiString (*text) (const Handle(TDF_Attribute)&)
       );


  Standard_Boolean Test
    (const Handle(TDF_Attribute)&anAtt) const;
  TCollection_AsciiString Open
    (const Handle(TDF_Attribute)&anAtt) const;
  TCollection_AsciiString Text
    (const Handle(TDF_Attribute)&anAtt) const;
  inline DDF_AttributeBrowser* Next() {return myNext;}

  static DDF_AttributeBrowser* FindBrowser
    (const Handle(TDF_Attribute)&anAtt);

  private :
    
  Standard_Boolean (*myTest)
     (const Handle(TDF_Attribute)&);

     TCollection_AsciiString (*myOpen)
     (const Handle(TDF_Attribute)&);

     TCollection_AsciiString (*myText)
     (const Handle(TDF_Attribute)&);

     DDF_AttributeBrowser* myNext;
    
};

#endif
