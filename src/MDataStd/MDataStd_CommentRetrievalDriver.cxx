// File:	MDataStd_CommentRetrievalDriver.cxx
// Created:	Thu Jan 15 11:24:15 1998
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <MDataStd_CommentRetrievalDriver.ixx>

#include <PDataStd_Comment.hxx>
#include <TDataStd_Comment.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_CommentRetrievalDriver::MDataStd_CommentRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_CommentRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_CommentRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Comment); }

Handle(TDF_Attribute) MDataStd_CommentRetrievalDriver::NewEmpty() const
{ return new TDataStd_Comment (); }

//void MDataStd_CommentRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataStd_CommentRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Comment) S = Handle(PDataStd_Comment)::DownCast (Source);
  Handle(TDataStd_Comment) T = Handle(TDataStd_Comment)::DownCast (Target);
  TCollection_ExtendedString Comment = (S->Get())->Convert ();
  T->Set (Comment);
}

