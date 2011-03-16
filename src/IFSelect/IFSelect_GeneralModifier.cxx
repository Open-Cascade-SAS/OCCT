#include <IFSelect_GeneralModifier.ixx>



    IFSelect_GeneralModifier::IFSelect_GeneralModifier
  (const Standard_Boolean maychangegraph)    {  thechgr = maychangegraph;  }

    Standard_Boolean  IFSelect_GeneralModifier::MayChangeGraph () const
      {  return thechgr;  }

    void  IFSelect_GeneralModifier::SetDispatch
  (const Handle(IFSelect_Dispatch)& disp)
      {  thedisp = disp;  }

    Handle(IFSelect_Dispatch)  IFSelect_GeneralModifier::Dispatch () const
      {  return thedisp;  }

    Standard_Boolean  IFSelect_GeneralModifier::Applies
  (const Handle(IFSelect_Dispatch)& disp) const
{
  if (thedisp.IsNull()) return Standard_True;
  if (thedisp != disp) return Standard_False;
  return Standard_True;
}


    void  IFSelect_GeneralModifier::SetSelection
  (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    void  IFSelect_GeneralModifier::ResetSelection ()
      {  thesel.Nullify();  }

    Standard_Boolean  IFSelect_GeneralModifier::HasSelection () const 
      {  return !thesel.IsNull();  }

    Handle(IFSelect_Selection)  IFSelect_GeneralModifier::Selection () const 
      {  return thesel;  }
