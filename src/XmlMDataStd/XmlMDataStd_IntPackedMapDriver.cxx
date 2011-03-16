// File:	XmlMDataStd_IntPackedMapDriver.cxx
// Created:	Wed Aug 22 09:48:41 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2007

#include <XmlMDataStd_IntPackedMapDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <XmlMDF_ADriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <XmlMDataStd.hxx>

IMPLEMENT_DOMSTRING (IntPackedMapSize, "mapsize")
IMPLEMENT_DOMSTRING (IsDeltaOn,        "delta")
//=======================================================================
//function : XmlMDataStd_IntPackedMapDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_IntPackedMapDriver::XmlMDataStd_IntPackedMapDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_IntPackedMap)->Name())
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_IntPackedMapDriver::NewEmpty () const
{
  return (new TDataStd_IntPackedMap());
}

//=======================================================================
//function : Paste()
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntPackedMapDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_IntPackedMap) aPackedMap =
    Handle(TDataStd_IntPackedMap)::DownCast(theTarget);
  if(!aPackedMap.IsNull()) {
    Standard_Integer aSize;
    const XmlObjMgt_Element& anElement = theSource;
    XmlObjMgt_DOMString aSizeDStr = anElement.getAttribute(::IntPackedMapSize());
    if(aSizeDStr == NULL)
      aSize = 0;
    else if (!aSizeDStr.GetInteger(aSize)) {
      TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the Map size"
                                 " for IntPackedMap attribute as \"")
        + aSize + "\"";
      WriteMessage (aMessageString);
      return Standard_False;            
    }
    Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
    Standard_Boolean Ok = Standard_True;
    if(aSize) {    
      Standard_CString aValueString =
	Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
//      Handle(TColStd_HPackedMapOfInteger) aHMap = new TColStd_HPackedMapOfInteger ();
      for (Standard_Integer i = 1; i <= aSize; i++) {
	Standard_Integer aValue;
	if (!XmlObjMgt::GetInteger(aValueString, aValue)) {
	  Ok = Standard_False; break;
	}
	if(!aHMap->ChangeMap().Add( aValue )) {
	  Ok = Standard_False; break;
	}
      }
      if(!Ok) {
	TCollection_ExtendedString aMessageString =
	  TCollection_ExtendedString("Cannot retrieve integer member"
				     " for IntPackedMap attribute as \"")
	    + aValueString + "\"";
	WriteMessage (aMessageString);
	return Standard_False;
      }
      if(aPackedMap->ChangeMap(aHMap))
	Ok = Standard_True;
    }
    //
#ifdef DEB
  //cout << "CurDocVersion = " << XmlMDataStd::DocumentVersion() <<endl;
#endif
    if(Ok) {
      Standard_Boolean aDelta(Standard_False);
  
      if(XmlMDataStd::DocumentVersion() > 2) {
	Standard_Integer aDeltaValue;
	if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
	  {
	    TCollection_ExtendedString aMessageString =
	      TCollection_ExtendedString("Cannot retrieve the isDelta value"
					 " for IntPackedMap attribute as \"")
		+ aDeltaValue + "\"";
	    WriteMessage (aMessageString);
	    return Standard_False;
	  } 
	else
	  aDelta = (Standard_Boolean)aDeltaValue;
      }
#ifdef DEB
      else if(XmlMDataStd::DocumentVersion() == -1)
	cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
      aPackedMap->SetDelta(aDelta);
      return Standard_True;
    }  
  }
  WriteMessage("error retrieving Map for type TDataStd_IntPackedMap");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntPackedMapDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntPackedMap) aS = Handle(TDataStd_IntPackedMap)::DownCast(theSource);
  if (aS.IsNull()) {
    WriteMessage ("IntPackedMapDriver:: The source attribute is Null.");
    return;
  }

  Standard_Integer aSize = (aS->IsEmpty()) ? 0 : aS->Extent();
  theTarget.Element().setAttribute(::IntPackedMapSize(), aSize);
  theTarget.Element().setAttribute(::IsDeltaOn(),aS->GetDelta());

  TCollection_AsciiString aValueString;
  if(aSize) {
    TColStd_MapIteratorOfPackedMapOfInteger anIt(aS->GetMap());
    for(;anIt.More();anIt.Next()) {
      aValueString += TCollection_AsciiString(anIt.Key());
      aValueString += ' ';
    }
    // No occurrence of '&', '<' and other irregular XML characters
    XmlObjMgt::SetStringValue (theTarget, aValueString.ToCString(), Standard_True);
  }
}
