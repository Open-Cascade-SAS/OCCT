#include <STEPEdit.ixx>
//#include <StepData_FileProtocol.hxx>
#include <StepAP214.hxx>
//#include <HeaderSection.hxx>

#include <APIHeaderSection_MakeHeader.hxx>

#include <StepSelect_StepType.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>


Handle(Interface_Protocol)  STEPEdit::Protocol ()
{
/*
  static Handle(StepData_FileProtocol) proto;
  if (!proto.IsNull()) return proto;
  proto =  new StepData_FileProtocol;
  proto->Add (StepAP214::Protocol());
  proto->Add (HeaderSection::Protocol());
  return proto;
*/
  return StepAP214::Protocol();
}

Handle(StepData_StepModel)  STEPEdit::NewModel ()
{
  APIHeaderSection_MakeHeader head;
  return head.NewModel(STEPEdit::Protocol());
}

Handle(IFSelect_Signature)  STEPEdit::SignType ()
{
  static Handle(StepSelect_StepType) sty;
  if (!sty.IsNull()) return sty;
  sty = new StepSelect_StepType;
  sty->SetProtocol (STEPEdit::Protocol());
  return sty;
}

Handle(IFSelect_SelectSignature)  STEPEdit::NewSelectSDR ()
{
  Handle(IFSelect_SelectSignature) sel = new IFSelect_SelectSignature
    (STEPEdit::SignType(),"SHAPE_DEFINITION_REPRESENTATION");
  sel->SetInput (new IFSelect_SelectModelRoots);
  return sel;
}

Handle(IFSelect_SelectSignature)  STEPEdit::NewSelectPlacedItem ()
{
  Handle(IFSelect_SelectSignature) sel = new IFSelect_SelectSignature
    (STEPEdit::SignType(),"MAPPED_ITEM|CONTEXT_DEPENDENT_SHAPE_REPRESENTATION",Standard_False);
  sel->SetInput (new IFSelect_SelectModelEntities);
  return sel;
}

Handle(IFSelect_SelectSignature)  STEPEdit::NewSelectShapeRepr ()
{
  Handle(IFSelect_SelectSignature) sel = new IFSelect_SelectSignature
    (STEPEdit::SignType(),"SHAPE_REPRESENTATION",Standard_False);
// REPRESENTATION_RELATIONSHIP passe par CONTEXT_DEPENDENT_SHAPE_REPRESENTATION
  sel->SetInput (new IFSelect_SelectModelEntities);
  return sel;
}
