// File:	MDataStd_CommentStorageDriver.cxx
// Created:	Thu Jan 15 11:21:53 1998
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <MDataStd_CommentStorageDriver.ixx>

#include <PDataStd_Comment.hxx>
#include <TDataStd_Comment.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>


MDataStd_CommentStorageDriver::MDataStd_CommentStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_CommentStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_CommentStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Comment);}

Handle(PDF_Attribute) MDataStd_CommentStorageDriver::NewEmpty() const
{ return new PDataStd_Comment; }

//void MDataStd_CommentStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataStd_CommentStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Comment) S = Handle(TDataStd_Comment)::DownCast (Source);
  Handle(PDataStd_Comment) T = Handle(PDataStd_Comment)::DownCast (Target);
  Handle(PCollection_HExtendedString) Comment = new PCollection_HExtendedString (S->Get());
  T->Set (Comment);
}

