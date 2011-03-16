#include <IFSelect_SelectControl.ixx>



    Handle(IFSelect_Selection)  IFSelect_SelectControl::MainInput () const 
      {  return themain;  }

    Handle(IFSelect_Selection)  IFSelect_SelectControl::SecondInput () const
      {  return thesecond;  }

    Standard_Boolean  IFSelect_SelectControl::HasSecondInput () const
      {  return (!thesecond.IsNull());  }

    void  IFSelect_SelectControl::SetMainInput
  (const Handle(IFSelect_Selection)& sel)
      {  themain = sel;  }

    void  IFSelect_SelectControl::SetSecondInput
  (const Handle(IFSelect_Selection)& sel)
      {  thesecond = sel;  }


    void  IFSelect_SelectControl::FillIterator
  (IFSelect_SelectionIterator& iter) const 
{
  iter.AddItem(themain);
  if (!thesecond.IsNull()) iter.AddItem(thesecond);
}
