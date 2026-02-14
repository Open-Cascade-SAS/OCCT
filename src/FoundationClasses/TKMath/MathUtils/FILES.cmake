# Source files for MathUtils package
set(OCCT_MathUtils_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_MathUtils_FILES
  # Types and configuration
  MathUtils_Types.hxx
  MathUtils_Config.hxx
  MathUtils_Domain.hxx
  # Core utilities
  MathUtils_Core.hxx
  MathUtils_Convergence.hxx
  MathUtils_Poly.hxx
  MathUtils_Random.hxx
  MathUtils_Bracket.hxx
  MathUtils_Gauss.hxx
  MathUtils_Deriv.hxx
  MathUtils_LineSearch.hxx
  MathUtils_GaussKronrodWeights.hxx
  MathUtils_GaussKronrodWeights.cxx
  # Functors
  MathUtils_FunctorScalar.hxx
  MathUtils_FunctorVector.hxx
  # Documentation
)
