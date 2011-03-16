// File:	PDataStd_Comment.cxx
// Created:	Thu Jan 15 11:19:02 1998
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <PDataStd_Comment.ixx>


PDataStd_Comment::PDataStd_Comment() {}

PDataStd_Comment::PDataStd_Comment(const Handle(PCollection_HExtendedString)& V)
: myValue (V) {}

Handle(PCollection_HExtendedString) PDataStd_Comment::Get() const
{ return myValue; }

void PDataStd_Comment::Set(const Handle(PCollection_HExtendedString)& V) 
{ myValue = V; }

