// File:        BinMDataStd_CommentDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <BinMDataStd_CommentDriver.ixx>
#include <TDataStd_Comment.hxx>

//=======================================================================
//function : BinMDataStd_CommentDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_CommentDriver::BinMDataStd_CommentDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_CommentDriver::NewEmpty() const
{
  return (new TDataStd_Comment());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_CommentDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  TCollection_ExtendedString aString;
  Standard_Boolean ok = theSource >> aString;
  if (ok)
    Handle(TDataStd_Comment)::DownCast(theTarget) -> Set (aString);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_CommentDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       BinObjMgt_SRelocationTable&  ) const
{
  TCollection_ExtendedString aName =
    Handle(TDataStd_Comment)::DownCast(theSource) -> Get();
  theTarget << aName;
}
