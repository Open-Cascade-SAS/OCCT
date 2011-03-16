// File AppParCurves_ConstraintCouple.cxx

#include <AppParCurves_ConstraintCouple.ixx>



AppParCurves_ConstraintCouple::
  AppParCurves_ConstraintCouple() {}


AppParCurves_ConstraintCouple::
  AppParCurves_ConstraintCouple(const Standard_Integer TheIndex,
				const AppParCurves_Constraint Cons)
{
  myIndex = TheIndex;
  myConstraint = Cons;
}


Standard_Integer AppParCurves_ConstraintCouple::Index() const
{
  return myIndex;
}

AppParCurves_Constraint AppParCurves_ConstraintCouple::Constraint() const
{
  return myConstraint;
}


void AppParCurves_ConstraintCouple::SetIndex(const Standard_Integer TheIndex)
{
  myIndex = TheIndex;
}

void AppParCurves_ConstraintCouple::
  SetConstraint(const AppParCurves_Constraint Cons)
{
  myConstraint = Cons;
}

