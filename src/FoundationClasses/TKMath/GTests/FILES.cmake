# Test source files for TKMath
set(OCCT_TKMath_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKMath_GTests_FILES
  Bnd_B2_Test.cxx
  Bnd_B3_Test.cxx
  Bnd_BoundSortBox_Test.cxx
  Bnd_Box_Test.cxx
  Bnd_OBB_Test.cxx
  BSplCLib_Cache_Test.cxx
  BSplCLib_UnitWeights_Test.cxx
  BSplSLib_Cache_Test.cxx
  BVH_BinnedBuilder_Test.cxx
  BVH_Box_Test.cxx
  BVH_BuildQueue_Test.cxx
  BVH_LinearBuilder_Test.cxx
  BVH_QuickSorter_Test.cxx
  BVH_RadixSorter_Test.cxx
  BVH_Ray_Test.cxx
  BVH_SpatialMedianBuilder_Test.cxx
  BVH_SweepPlaneBuilder_Test.cxx
  BVH_Tools_Test.cxx
  BVH_Traverse_Test.cxx
  BVH_Triangulation_Test.cxx
  BVH_Tree_Test.cxx
  # Convert tests
  Convert_CircleToBSplineCurve_Test.cxx
  Convert_CompBezierCurvesToBSplineCurve_Test.cxx
  Convert_CompPolynomialToPoles_Test.cxx
  Convert_ConeToBSplineSurface_Test.cxx
  Convert_CylinderToBSplineSurface_Test.cxx
  Convert_EllipseToBSplineCurve_Test.cxx
  Convert_GridPolynomialToPoles_Test.cxx
  Convert_HyperbolaToBSplineCurve_Test.cxx
  Convert_ParabolaToBSplineCurve_Test.cxx
  Convert_SphereToBSplineSurface_Test.cxx
  Convert_TorusToBSplineSurface_Test.cxx
  CSLib_Test.cxx
  ElCLib_Test.cxx
  gp_Ax3_Test.cxx
  gp_Mat_Test.cxx
  gp_Trsf_Test.cxx
  math_BFGS_Test.cxx
  math_BissecNewton_Test.cxx
  math_BracketMinimum_Test.cxx
  math_BracketedRoot_Test.cxx
  math_Crout_Test.cxx
  math_BrentMinimum_Test.cxx
  math_ComputeKronrodPointsAndWeights_Test.cxx
  math_DirectPolynomialRoots_Test.cxx
  math_DoubleTab_Test.cxx
  math_EigenValuesSearcher_Test.cxx
  math_FRPR_Test.cxx
  math_FunctionAllRoots_Test.cxx
  math_FunctionRoot_Test.cxx
  math_FunctionRoots_Test.cxx
  math_FunctionSetRoot_Test.cxx
  math_Gauss_Test.cxx
  math_GaussLeastSquare_Test.cxx
  math_GlobOptMin_Test.cxx
  math_Householder_Test.cxx
  math_Integration_Test.cxx
  math_Jacobi_Test.cxx
  math_Matrix_Test.cxx
  math_NewtonFunctionRoot_Test.cxx
  math_NewtonFunctionSetRoot_Test.cxx
  math_NewtonMinimum_Test.cxx
  math_Powell_Test.cxx
  math_PSO_Test.cxx
  math_SVD_Test.cxx
  math_TrigonometricFunctionRoots_Test.cxx
  math_Uzawa_Test.cxx
  math_Vector_Test.cxx
  # MathUtils tests
  MathUtils_Functor_Test.cxx
  # MathPoly tests
  MathPoly_Test.cxx
  MathPoly_Comparison_Test.cxx
  # MathLin tests
  MathLin_Test.cxx
  MathLin_Comparison_Test.cxx
  # MathOpt tests
  MathOpt_1D_Test.cxx
  MathOpt_NDim_Test.cxx
  MathOpt_FRPR_Test.cxx
  MathOpt_Newton_Test.cxx
  MathOpt_PSO_Test.cxx
  MathOpt_GlobOpt_Test.cxx
  MathOpt_1D_Comparison_Test.cxx
  MathOpt_NDim_Comparison_Test.cxx
  # MathRoot tests
  MathRoot_Test.cxx
  MathRoot_Multiple_Test.cxx
  MathRoot_Comparison_Test.cxx
  MathRoot_Trig_Test.cxx
  # MathInteg tests
  MathInteg_Test.cxx
  MathInteg_New_Test.cxx
  MathInteg_Comparison_Test.cxx
  # MathSys tests
  MathSys_LM_Test.cxx
  MathSys_Comparison_Test.cxx
  PLib_Test.cxx
  PLib_JacobiPolynomial_Test.cxx
  PLib_HermitJacobi_Test.cxx
)
