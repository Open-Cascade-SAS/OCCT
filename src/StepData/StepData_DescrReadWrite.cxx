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

#include <StepData_DescrReadWrite.ixx>
#include <StepData_EDescr.hxx>
#include <StepData_ECDescr.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_Simple.hxx>
#include <StepData_Plex.hxx>
#include <StepData_FieldListN.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <Interface_Macros.hxx>

static TCollection_AsciiString nocp("?");


    StepData_DescrReadWrite::StepData_DescrReadWrite
  (const Handle(StepData_Protocol)& proto)
    : theproto (proto)    {  }

    Standard_Integer  StepData_DescrReadWrite::CaseStep
  (const TCollection_AsciiString& atype) const
{
  Handle(StepData_ESDescr) descr = theproto->ESDescr (atype.ToCString(),Standard_False);
  if (descr.IsNull()) return 0;
  return theproto->DescrNumber (descr);
}

    Standard_Integer  StepData_DescrReadWrite::CaseStep
  (const TColStd_SequenceOfAsciiString& types) const
{
  Handle(StepData_ECDescr) descr = theproto->ECDescr (types,Standard_False);
  if (descr.IsNull()) return 0;
  return theproto->DescrNumber (descr);
}


    Standard_Boolean  StepData_DescrReadWrite::IsComplex
  (const Standard_Integer CN) const
{
  Handle(StepData_EDescr)  descr = theproto->Descr (CN);
  if (descr.IsNull()) return Standard_False;
  return descr->IsComplex();
}

    const TCollection_AsciiString& StepData_DescrReadWrite::StepType
  (const Standard_Integer CN) const
{
  Handle(StepData_ESDescr)  descr = Handle(StepData_ESDescr)::DownCast
    (theproto->Descr (CN));
  if (descr.IsNull()) return nocp;
  return descr->StepType();
}

    Standard_Boolean StepData_DescrReadWrite::ComplexType
  (const Standard_Integer CN, TColStd_SequenceOfAsciiString& types) const
{
  Handle(StepData_ECDescr)  descr = Handle(StepData_ECDescr)::DownCast
    (theproto->Descr (CN));
  if (descr.IsNull()) return Standard_False;
  Handle(TColStd_HSequenceOfAsciiString) list = descr->TypeList();
  if (list.IsNull()) return Standard_False;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++)  types.Append (list->Value(i));
  return Standard_True;
}


void StepData_DescrReadWrite::ReadStep(const Standard_Integer CN,
                                       const Handle(StepData_StepReaderData)& data,
                                       const Standard_Integer num,
                                       Handle(Interface_Check)& ach,
                                       const Handle(Standard_Transient)&ent) const
{
  if (CN == 0) return;
  DeclareAndCast(StepData_Simple,sent,ent);
  if (!sent.IsNull()) {
    Handle(StepData_ESDescr) sdescr = sent->ESDescr();
    StepData_FieldListN& fl = sent->CFields();
    data->ReadList (num,ach,sdescr,fl);
    return;
  }

  DeclareAndCast(StepData_Plex,cent,ent);
  Standard_Integer i, nb=0;
  if (!cent.IsNull()) nb = cent->NbMembers();
  Standard_Integer n0 = num;
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Simple) si = cent->Member(i);
    Handle(StepData_ESDescr) sdescr = si->ESDescr();
    StepData_FieldListN& fl = si->CFields();
    data->ReadList (n0,ach,sdescr,fl);
    if (i < nb) n0 = data->NextForComplex(n0);
  }
}


    void StepData_DescrReadWrite::WriteStep
  (const Standard_Integer CN,
   StepData_StepWriter& SW,
   const Handle(Standard_Transient)&ent) const
{
  if (CN == 0) return;

  DeclareAndCast(StepData_Simple,sent,ent);
  if (!sent.IsNull()) {
    Handle(StepData_ESDescr) sdescr = sent->ESDescr();
    const StepData_FieldListN& fl = sent->Fields();
    SW.SendList (fl,sdescr);
    return;
  }

  DeclareAndCast(StepData_Plex,cent,ent);
  Standard_Integer i, nb=0;
  if (!cent.IsNull()) nb = cent->NbMembers();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Simple) si = cent->Member(i);
    Handle(StepData_ESDescr) sdescr = si->ESDescr();
      const StepData_FieldListN& fl = si->Fields();
      SW.StartEntity (sdescr->TypeName());
      SW.SendList (fl,sdescr);
  }
}
