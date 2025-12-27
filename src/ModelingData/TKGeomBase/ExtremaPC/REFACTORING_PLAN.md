# ExtremaPC Immutable Configuration Refactoring Plan

## Goal
Make all ExtremaPC classes immutable after construction:
- Domain/UV limits fixed at construction time
- Grids and caches built eagerly in constructor
- Perform() methods only take the query point
- No mutable members, no runtime cache validation

## Benefits
1. **Performance**: Grid built once, no `updateCacheIfNeeded()` on each query
2. **Thread-safety**: Immutable objects are inherently thread-safe
3. **Simplicity**: No lazy initialization, no cache invalidation logic
4. **Predictability**: All heavy computation happens at construction

## API Changes

### Before (Current)
```cpp
// Domain can be passed at Perform time - forces cache rebuild check
ExtremaPC_BSplineCurve curve(bspline);
curve.Perform(point, domain1, tol, mode);  // builds grid for domain1
curve.Perform(point, domain2, tol, mode);  // rebuilds grid for domain2
```

### After (New)
```cpp
// Domain fixed at construction - grid built once
ExtremaPC_BSplineCurve curve(bspline, domain);  // builds grid here
curve.Perform(point, tol, mode);  // only searches, no rebuilding
curve.Perform(point2, tol, mode); // reuses same grid
```

## Changes Per Class

### 1. Analytical Curves (No caching, but consistent API)

**ExtremaPC_Line**
- Constructor: `ExtremaPC_Line(gp_Lin, optional<Domain1D>)`
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses stored domain
- Add: `PerformWithEndpoints(P, tol, mode)` - uses stored domain

**ExtremaPC_Circle**
- Constructor: `ExtremaPC_Circle(gp_Circ, optional<Domain1D>)`
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses stored domain

**ExtremaPC_Ellipse**
- Constructor: `ExtremaPC_Ellipse(gp_Elips, optional<Domain1D>)`
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses stored domain

**ExtremaPC_Parabola**
- Constructor: `ExtremaPC_Parabola(gp_Parab, optional<Domain1D>)`
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses stored domain

**ExtremaPC_Hyperbola**
- Constructor: `ExtremaPC_Hyperbola(gp_Hypr, optional<Domain1D>)`
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses stored domain

### 2. Grid-Based Curves (Major performance benefit)

**ExtremaPC_BezierCurve**
- Constructor: `ExtremaPC_BezierCurve(Handle(Geom_BezierCurve), optional<Domain1D>)`
- Build grid in constructor (not lazy)
- Remove: `mutable` grid members
- Remove: `updateCacheIfNeeded()` method
- Remove: `Perform(P, domain, tol, mode)` overload
- Keep: `Perform(P, tol, mode)` - uses pre-built grid

**ExtremaPC_BSplineCurve**
- Constructor: `ExtremaPC_BSplineCurve(Handle(Geom_BSplineCurve), optional<Domain1D>)`
- Build knot-aware grid in constructor
- Remove: `mutable` grid members
- Remove: `updateCacheIfNeeded()` method
- Remove: `Perform(P, domain, tol, mode)` overload

**ExtremaPC_OffsetCurve**
- Constructor: `ExtremaPC_OffsetCurve(Adaptor3d_Curve, optional<Domain1D>)`
- Build grid in constructor
- Remove: `mutable` members
- Remove: `Perform(P, domain, tol, mode)` overload

**ExtremaPC_OtherCurve**
- Constructor: `ExtremaPC_OtherCurve(Adaptor3d_Curve, optional<Domain1D>)`
- Build grid in constructor
- Remove: `mutable` members
- Remove: `Perform(P, domain, tol, mode)` overload

### 3. Aggregator

**ExtremaPC_Curve**
- Constructor passes domain to inner curve constructor
- Inner curves are constructed with their final domain
- Remove any domain-modifying logic after construction

## Implementation Order

1. Update ExtremaPC_Line (simplest, establishes pattern)
2. Update ExtremaPC_Circle, ExtremaPC_Ellipse
3. Update ExtremaPC_Parabola, ExtremaPC_Hyperbola
4. Update ExtremaPC_BezierCurve (grid caching benefit)
5. Update ExtremaPC_BSplineCurve (grid caching benefit)
6. Update ExtremaPC_OffsetCurve, ExtremaPC_OtherCurve
7. Update ExtremaPC_Curve aggregator
8. Update tests to use new API
9. Build and verify all tests pass

## Member Changes Summary

| Class | Remove mutable | Remove updateCacheIfNeeded | Add myDomain |
|-------|----------------|---------------------------|--------------|
| Line | N/A | N/A | Yes |
| Circle | N/A | N/A | Yes |
| Ellipse | N/A | N/A | Yes |
| Parabola | N/A | N/A | Yes |
| Hyperbola | N/A | N/A | Yes |
| BezierCurve | Yes | Yes | Yes |
| BSplineCurve | Yes | Yes | Yes |
| OffsetCurve | Yes | Yes | Yes |
| OtherCurve | Yes | Yes | Yes |
