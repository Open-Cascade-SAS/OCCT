#include <StepBasic_SiUnit.ixx>


StepBasic_SiUnit::StepBasic_SiUnit ()  {}

void StepBasic_SiUnit::Init(
	const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  StepBasic_NamedUnit::Init(aDimensions);
}

void StepBasic_SiUnit::Init(const Standard_Boolean hasAprefix,
                            const StepBasic_SiPrefix aPrefix,
                            const StepBasic_SiUnitName aName)
{
  // --- classe own fields ---
  hasPrefix = hasAprefix;
  prefix = aPrefix;
  name = aName;
  // --- classe inherited fields ---
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  StepBasic_NamedUnit::Init(aDimensions);
}


void StepBasic_SiUnit::SetPrefix(const StepBasic_SiPrefix aPrefix)
{
  prefix = aPrefix;
  hasPrefix = Standard_True;
}


void StepBasic_SiUnit::UnSetPrefix()
{
  hasPrefix = Standard_False;
}


StepBasic_SiPrefix StepBasic_SiUnit::Prefix() const
{
  return prefix;
}


Standard_Boolean StepBasic_SiUnit::HasPrefix() const
{
  return hasPrefix;
}


void StepBasic_SiUnit::SetName(const StepBasic_SiUnitName aName)
{
  name = aName;
}


StepBasic_SiUnitName StepBasic_SiUnit::Name() const
{
  return name;
}


void StepBasic_SiUnit::SetDimensions(const Handle(StepBasic_DimensionalExponents)& aDimensions)
{
  // WARNING : the field is redefined.
  // field set up forbidden.
#ifdef DEB
  cout << "Field is redefined, SetUp Forbidden" << endl;
#endif
}


Handle(StepBasic_DimensionalExponents) StepBasic_SiUnit::Dimensions() const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote

  // attention : dimensional exponent retourne incorrect (pointeur NULL).
  // on devrait, en fonction du nom de l unite SI, construire un dimensional
  // exponents coherent. (du style .METRE. => (1,0,0,0,0,0) ... )

  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  return aDimensions;

}
