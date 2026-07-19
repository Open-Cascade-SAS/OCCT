# TKHelix Toolkit

## Overview

The TKHelix toolkit provides comprehensive functionality for creating, approximating, and manipulating helix curves in OCCT. This toolkit enables the construction of complex helix geometries commonly used in mechanical engineering, manufacturing, and architectural applications.

## Architecture

The TKHelix toolkit is organized into two packages:

### HelixGeom Package
Provides geometric helix curve functionality:
- **HelixGeom_HelixCurve**: Analytical helix curve adaptor with parametric representation
- **HelixGeom_BuilderHelix**: Single helix curve approximation algorithms  
- **HelixGeom_BuilderHelixCoil**: Multi-coil helix approximation with advanced features
- **HelixGeom_BuilderApproxCurve**: Base class for approximation algorithms
- **HelixGeom_BuilderHelixGen**: Base class with helix-specific parameter management
- **HelixGeom_Tools**: Static utility functions for curve approximation

### HelixBRep Package
Provides topological helix construction:
- **HelixBRep_BuilderHelix**: Topological helix wire construction from geometric curves

## Key Features

### Helix Types Supported
- **Cylindrical Helixes**: Constant radius helixes for springs, threads
- **Tapered Helixes**: Variable radius helixes with taper angles
- **Spiral Helixes**: Multi-diameter configurations for complex shapes
- **Multi-part Helixes**: Segmented helixes with different parameters per section

### Advanced Capabilities
- High-quality B-spline approximation with specified tolerance
- Configurable continuity requirements (C0, C1, C2)
- Both clockwise and counter-clockwise orientations
- Smooth transitions between helix segments
- Comprehensive error handling and status reporting

### DRAW Command Interface
Interactive commands for helix creation and testing:
- `setaxis`: Define helix axis orientation
- `helix`: Create single-diameter helix
- `comphelix`: Create multi-diameter helix
- `spiral`: Create spiral helix with varying diameter
- `helix2`, `comphelix2`, `spiral2`: Alternative interfaces using number of turns

## Mathematical Foundation

Helix curves are represented using parametric equations in cylindrical coordinates:

```
x(t) = r(t) * cos(t)
y(t) = r(t) * sin(t) [* direction_factor]
z(t) = pitch * t / (2π)
```

Where:
- `r(t) = r_start + taper_factor * t` (radius as function of parameter)
- `t` is the angular parameter in radians
- `pitch` is the vertical distance per full turn (2π radians)
- `direction_factor` is 1 for clockwise, -1 for counter-clockwise

## Usage Examples

### Basic Cylindrical Helix
```cpp
HelixGeom_BuilderHelixCoil builder;
builder.SetCurveParameters(0.0, 4*M_PI, 10.0, 5.0, 0.0, Standard_True);
builder.SetTolerance(1e-6);
builder.Perform();
```

### Tapered Helix
```cpp
HelixGeom_BuilderHelixCoil builder;
builder.SetCurveParameters(0.0, 6*M_PI, 15.0, 8.0, 0.1, Standard_True);
builder.SetApproxParameters(GeomAbs_C2, 8, 100);
builder.Perform();
```

### Topological Wire Construction
```cpp
HelixBRep_BuilderHelix builder;
TColStd_Array1OfReal heights(1, 1);
heights(1) = 50.0;
TColStd_Array1OfReal pitches(1, 1);
pitches(1) = 10.0;
TColStd_Array1OfBoolean isPitches(1, 1);
isPitches(1) = Standard_True;

gp_Ax3 axis(gp_Pnt(0,0,0), gp_Dir(0,0,1), gp_Dir(1,0,0));
builder.SetParameters(axis, 25.0, heights, pitches, isPitches);
builder.Perform();
TopoDS_Shape helixWire = builder.Shape();
```

## Dependencies

The TKHelix toolkit depends on the following OCCT toolkits:
- **TKernel**: Basic OCCT functionality
- **TKMath**: Mathematical utilities
- **TKGeomBase**: Basic geometric types
- **TKG2d**: 2D geometry
- **TKG3d**: 3D geometry and curves
- **TKGeomAlgo**: Geometric algorithms
- **TKBRep**: Boundary representation
- **TKTopAlgo**: Topological algorithms

## Testing

The toolkit includes comprehensive testing:
- **44 unit tests** covering all classes and methods
- **82 TCL test cases** for integration validation
- **Error condition testing** for robustness
- **Performance and tolerance validation**

Test files are located in:
- `src/ModelingAlgorithms/TKHelix/GTests/` - Unit tests
- `tests/helix/` - TCL integration tests

## Build Integration

The toolkit is fully integrated into the OCCT build system:
- CMake configuration files for automatic building
- Proper toolkit dependencies declared
- Integration with CTest framework for automated testing

## Error Codes

Common error status codes returned by algorithms:
- **0**: Success
- **2**: Geometric algorithm failure
- **10**: Invalid start point (on axis)
- **11**: Invalid pitch value
- **12**: Invalid height value  
- **13**: Invalid taper angle value

## Future Enhancements

Potential areas for future development:
- Support for elliptical helix bases
- Variable pitch helixes
- Advanced smoothing algorithms for multi-part helixes
- Integration with surface modeling for helix-based features