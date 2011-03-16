//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SelectedComponent.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SelectedComponent.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_SelectedComponent::IGESSolid_SelectedComponent ()    {  }


    void  IGESSolid_SelectedComponent::Init
  (const Handle(IGESSolid_BooleanTree)& anEntity, const gp_XYZ& SelectPnt)
{
  theEntity      = anEntity;
  theSelectPoint = SelectPnt;
  InitTypeAndForm(182,0);
}

    Handle(IGESSolid_BooleanTree)  IGESSolid_SelectedComponent::Component () const
{
  return theEntity;
}

    gp_Pnt  IGESSolid_SelectedComponent::SelectPoint () const
{
  return gp_Pnt(theSelectPoint);
}

    gp_Pnt  IGESSolid_SelectedComponent::TransformedSelectPoint () const
{
  if (!HasTransf()) return gp_Pnt(theSelectPoint);
  else
    {
      gp_XYZ tmp = theSelectPoint;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}
