// File:        XmlMDataStd_CommentDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataStd_CommentDriver.ixx>
#include <TDataStd_Comment.hxx>
#include <XmlObjMgt.hxx>

//=======================================================================
//function : XmlMDataStd_CommentDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_CommentDriver::XmlMDataStd_CommentDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_CommentDriver::NewEmpty() const
{
  return (new TDataStd_Comment());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_CommentDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString (theSource, aString))
  {
    Handle(TDataStd_Comment)::DownCast(theTarget) -> Set (aString);
    return Standard_True;
  }
  WriteMessage("error retrieving ExtendedString for type TDataStd_Comment");
  return Standard_False;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_CommentDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Comment) aName= Handle(TDataStd_Comment)::DownCast(theSource);
  XmlObjMgt::SetExtendedString (theTarget, aName -> Get());
}
