// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Oct  6 1997	Creation



#ifndef DDF_AttributeBrowser_HeaderFile
#define DDF_AttributeBrowser_HeaderFile

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
