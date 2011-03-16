#include <IFSelect_Act.ixx>
#include <TCollection_AsciiString.hxx>

static TCollection_AsciiString thedefgr, thedefil;



    IFSelect_Act::IFSelect_Act
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
    : thename (name) , thehelp (help) , thefunc (func)    {  }

    IFSelect_ReturnStatus  IFSelect_Act::Do
  (const Standard_Integer, const Handle(IFSelect_SessionPilot)& pilot)
{
  if (!thefunc) return IFSelect_RetVoid;
  return thefunc (pilot);
}

    Standard_CString  IFSelect_Act::Help (const Standard_Integer) const
      {  return thehelp.ToCString();  }


    void  IFSelect_Act::SetGroup
  (const Standard_CString group, const Standard_CString file)
{  thedefgr.Clear();  if (group[0] != '\0') thedefgr.AssignCat(group);
   thedefil.Clear();  if (file [0] != '\0') thedefil.AssignCat(file);  }

    void  IFSelect_Act::AddFunc
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
{
  Handle(IFSelect_Act) act = new IFSelect_Act (name,help,func);
  if (thedefgr.Length() > 0) act->SetForGroup (thedefgr.ToCString());
  act->Add    (1,name);
}

    void  IFSelect_Act::AddFSet
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
{
  Handle(IFSelect_Act) act = new IFSelect_Act (name,help,func);
  if (thedefgr.Length() > 0)
    act->SetForGroup (thedefgr.ToCString(),thedefil.ToCString());
  act->AddSet (1,name);
}
