// File:	DDocStd.cxx
// Created:	Wed Mar  1 14:05:40 2000
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>

#include <DDocStd.ixx>
#include <DDocStd_DrawDocument.hxx>
#include <TDocStd_Document.hxx>
#include <Draw_Interpretor.hxx>
//
#include <TCollection_AsciiString.hxx>
#include <Draw.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <DBRep.hxx>
#include <TDocStd_Document.hxx>
#include <CDF_Session.hxx>
#include <Standard_DomainError.hxx>


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean DDocStd::Find (Handle(TDocStd_Application)& A,
				const Standard_Boolean Complain)
{  
  if (!CDF_Session::Exists()) 
    Standard_DomainError::Raise("DDocStd::Find no applicative session");
  Handle(CDF_Session) S = CDF_Session::CurrentSession();  
  Handle(TDocStd_Application) APP;
  if (!S->HasCurrentApplication())  
    Standard_DomainError::Raise("DDocStd::Find no applicative session");
  APP = Handle(TDocStd_Application)::DownCast(S->CurrentApplication());
  A = APP;
  return Standard_True;
}


//=======================================================================
//function : GetDocument
//purpose  : 
//=======================================================================

Standard_Boolean DDocStd::GetDocument (Standard_CString&         Name,
				       Handle(TDocStd_Document)& DOC,
				       const Standard_Boolean    Complain)
{

  Handle(Draw_Drawable3D) D = Draw::Get(Name,Standard_False);

  Handle(DDocStd_DrawDocument) DD = Handle(DDocStd_DrawDocument)::DownCast (D);
  if (DD.IsNull()) {
    if (Complain) cout << Name << " is not a Document" << endl; 
    return Standard_False;
  }
  Handle(TDocStd_Document) STDDOC =  Handle(TDocStd_Document)::DownCast(DD->GetDocument());
  if (!STDDOC.IsNull()) {
    DOC = STDDOC;
    return Standard_True;
  }
  if (Complain) cout << Name << " is not a CAF Document" << endl; 
  return Standard_False;
}


//=======================================================================
//function : Label
//purpose  : try to retrieve a label 
//=======================================================================

Standard_Boolean DDocStd::Find (const Handle(TDocStd_Document)& D,
				const Standard_CString  Entry,
				TDF_Label& Label,   
				const Standard_Boolean  Complain)
{
  Label.Nullify();
#ifdef DEB
  Standard_Boolean Found;
#endif
  TDF_Tool::Label(D->GetData(),Entry,Label,Standard_False);
  if (Label.IsNull() && Complain) cout<<"No label for entry "<<Entry<<endl;
  return !Label.IsNull();
}

//=======================================================================
//function : Find
//purpose  : Try to retrieve an attribute.
//=======================================================================

Standard_Boolean DDocStd::Find (const Handle(TDocStd_Document)& D,
				const Standard_CString  Entry,
				const Standard_GUID&    ID,
				Handle(TDF_Attribute)&  A,
				const Standard_Boolean  Complain) 
{
#ifdef DEB
  Standard_Boolean found;
#endif
  TDF_Label L;
  if (Find(D,Entry,L,Complain)) {
    if (L.FindAttribute(ID,A)) return Standard_True;
    if (Complain) cout <<"attribute not found for entry : "<< Entry <<endl; 
  }
  return Standard_False;   
}


//=======================================================================
//function : ReturnLabel
//purpose  : 
//=======================================================================
 
Draw_Interpretor& DDocStd::ReturnLabel(Draw_Interpretor& di, const TDF_Label& L)
{
  TCollection_AsciiString S;
  TDF_Tool::Entry(L,S);
  di << S.ToCString();
  return di;
}

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void DDocStd::AllCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;


  // define commands
  DDocStd::ApplicationCommands(theCommands);
  DDocStd::DocumentCommands(theCommands);
  DDocStd::ToolsCommands(theCommands);
  DDocStd::MTMCommands(theCommands);
}
