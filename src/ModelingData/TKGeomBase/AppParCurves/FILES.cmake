# Source files for AppParCurves package
set(OCCT_AppParCurves_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_AppParCurves_FILES
  AppParCurves.cxx
  AppParCurves.hxx

  AppParCurves_BSpFunction.gxx
  AppParCurves_BSpGradient.gxx
  AppParCurves_Constraint.hxx
  AppParCurves_ConstraintCouple.cxx
  AppParCurves_ConstraintCouple.hxx
  AppParCurves_Function.gxx
  AppParCurves_Gradient.gxx

  AppParCurves_LeastSquare.gxx
  AppParCurves_MultiBSpCurve.cxx
  AppParCurves_MultiBSpCurve.hxx
  AppParCurves_MultiCurve.cxx
  AppParCurves_MultiCurve.hxx
  AppParCurves_MultiPoint.cxx
  AppParCurves_MultiPoint.hxx
  AppParCurves_MultiPoint.lxx
  AppParCurves_ResolConstraint.gxx

)
