// File:	DDF_AttributeBrowser.hxx
//      	------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
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
