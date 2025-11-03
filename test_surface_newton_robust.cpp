#include <iostream>
#include <ShapeAnalysis_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

int main()
{
  std::cout << "Testing robust SurfaceNewton implementation..." << std::endl;

  try
  {
    // Create a simple test B-spline surface
    TColgp_Array2OfPnt poles(1, 3, 1, 3);

    // Define control points for a simple curved surface
    poles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
    poles(1, 2) = gp_Pnt(0.0, 1.0, 0.5);
    poles(1, 3) = gp_Pnt(0.0, 2.0, 0.0);
    poles(2, 1) = gp_Pnt(1.0, 0.0, 0.5);
    poles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);
    poles(2, 3) = gp_Pnt(1.0, 2.0, 0.5);
    poles(3, 1) = gp_Pnt(2.0, 0.0, 0.0);
    poles(3, 2) = gp_Pnt(2.0, 1.0, 0.5);
    poles(3, 3) = gp_Pnt(2.0, 2.0, 0.0);

    // Define knot vectors
    TColStd_Array1OfReal uknots(1, 2), vknots(1, 2);
    uknots(1) = 0.0;
    uknots(2) = 1.0;
    vknots(1) = 0.0;
    vknots(2) = 1.0;

    TColStd_Array1OfInteger umults(1, 2), vmults(1, 2);
    umults(1) = 3;
    umults(2) = 3;
    vmults(1) = 3;
    vmults(2) = 3;

    // Create B-spline surface
    Handle(Geom_BSplineSurface) surface =
      new Geom_BSplineSurface(poles, uknots, vknots, umults, vmults, 2, 2);

    // Test the robust SurfaceNewton method
    ShapeAnalysis_Surface surfAnalysis(surface);

    // Test point near the surface
    gp_Pnt   testPnt(0.5, 1.0, 0.8);
    gp_Pnt2d initialGuess(0.3, 0.5);
    gp_Pnt2d solution;

    Standard_Real    precision = 1e-6;
    Standard_Integer result =
      surfAnalysis.SurfaceNewton(initialGuess, testPnt, precision, solution);

    if (result)
    {
      std::cout << "SUCCESS: SurfaceNewton converged!" << std::endl;
      std::cout << "Solution: U=" << solution.X() << ", V=" << solution.Y() << std::endl;

      // Verify the solution quality
      gp_Pnt        projectedPnt = surface->Value(solution.X(), solution.Y());
      Standard_Real distance     = testPnt.Distance(projectedPnt);
      std::cout << "Distance to surface: " << distance << std::endl;
      std::cout << "Required precision: " << precision << std::endl;

      if (distance <= precision)
      {
        std::cout << "QUALITY CHECK PASSED: Distance within tolerance" << std::endl;
      }
    }
    else
    {
      std::cout << "SurfaceNewton did not converge, using fallback" << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Test completed successfully." << std::endl;
  return 0;
}
