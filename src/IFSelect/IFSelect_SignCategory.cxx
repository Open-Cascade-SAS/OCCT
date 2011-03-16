#include <IFSelect_SignCategory.ixx>
#include <Interface_Category.hxx>


static Standard_CString nulsign = "";


    IFSelect_SignCategory::IFSelect_SignCategory ()
    : IFSelect_Signature ("Category")
{
  Interface_Category::Init();  // si pas deja fait
  Standard_Integer i, nb = Interface_Category::NbCategories();
  for (i = 1; i <= nb; i ++)  AddCase (Interface_Category::Name(i));
}


    Standard_CString IFSelect_SignCategory::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  if (ent.IsNull() || model.IsNull()) return nulsign;
  return Interface_Category::Name(model->CategoryNumber (model->Number(ent)));
}
