#include <StepBasic_ProductRelatedProductCategory.ixx>


StepBasic_ProductRelatedProductCategory::StepBasic_ProductRelatedProductCategory ()  {}

void StepBasic_ProductRelatedProductCategory::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAdescription,
	const Handle(TCollection_HAsciiString)& aDescription)
{

	StepBasic_ProductCategory::Init(aName, hasAdescription, aDescription);
}

void StepBasic_ProductRelatedProductCategory::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAdescription,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_HArray1OfProduct)& aProducts)
{
	// --- classe own fields ---
	products = aProducts;
	// --- classe inherited fields ---
	StepBasic_ProductCategory::Init(aName, hasAdescription, aDescription);
}


void StepBasic_ProductRelatedProductCategory::SetProducts(const Handle(StepBasic_HArray1OfProduct)& aProducts)
{
	products = aProducts;
}

Handle(StepBasic_HArray1OfProduct) StepBasic_ProductRelatedProductCategory::Products() const
{
	return products;
}

Handle(StepBasic_Product) StepBasic_ProductRelatedProductCategory::ProductsValue(const Standard_Integer num) const
{
	return products->Value(num);
}

Standard_Integer StepBasic_ProductRelatedProductCategory::NbProducts () const
{
	if (products.IsNull()) return 0;
	return products->Length();
}
