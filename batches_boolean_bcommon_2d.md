# OCCT Test Migration Batches

Below are batches of Tcl scripts. Paste one batch at a time to the AI.

--- BATCH 1 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/A8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: A8
- **Data Files**: `case_1_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_shell.brep`), `case_1_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_edge2.brep`)
```tcl
restore [locate_data_file case_1_shell.brep] a
restore [locate_data_file case_1_edge2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/A9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: A9
- **Data Files**: `case_1_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_shell.brep`), `case_1_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_edge4.brep`)
```tcl
restore [locate_data_file case_1_shell.brep] a
restore [locate_data_file case_1_edge4.brep] b

bcommon result b a

checkprops result -l 471.239
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/B1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B1
- **Data Files**: `case_1_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_shell.brep`), `case_1_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_1_wire3.brep`)
```tcl
restore [locate_data_file case_1_shell.brep] a
restore [locate_data_file case_1_wire3.brep] b

bcommon result b a

checkprops result -l 942.478
checksection result
checknbshapes result -vertex 2 -edge 2 -t
```

### Test: boolean/bcommon_2d/B2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B2
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge1.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_edge1.brep] b

bcommon result b a

checkprops result -l 847.007
checksection result
checknbshapes result -vertex 14 -edge 7 -t
```

### Test: boolean/bcommon_2d/B3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B3
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge2.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_edge2.brep] b

bcommon result b a

checkprops result -l 847.007
checksection result
checknbshapes result -vertex 14 -edge 7 -t
```

### Test: boolean/bcommon_2d/B4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B4
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge3.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_edge3.brep] b

bcommon result b a

checkprops result -l 753.315
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/B5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B5
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge4.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_edge4.brep] b

bcommon result b a

checkprops result -l 753.315
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/B6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B6
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire1.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_wire1.brep] b

bcommon result b a

checkprops result -l 874
checksection result
checknbshapes result -vertex 18 -edge 11 -t
```

### Test: boolean/bcommon_2d/B7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B7
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire2.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_wire2.brep] b

bcommon result b a

checkprops result -l 874
checksection result
checknbshapes result -vertex 18 -edge 11 -t
```

### Test: boolean/bcommon_2d/B8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B8
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire3.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_wire3.brep] b

bcommon result b a

checkprops result -l 846.821
checksection result
checknbshapes result -vertex 9 -edge 6 -t
```

--- BATCH 1 END ---

--- BATCH 2 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/B9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: B9
- **Data Files**: `case_2_solid_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_solid_r.brep`), `case_2_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire4.brep`)
```tcl
restore [locate_data_file case_2_solid_r.brep] a
restore [locate_data_file case_2_wire4.brep] b

bcommon result b a

checkprops result -l 768.597
checksection result
checknbshapes result -vertex 9 -edge 6 -t
```

### Test: boolean/bcommon_2d/C1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C1
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge1.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_edge1.brep] b

bcommon result b a

checkprops result -l 847.007
checksection result
checknbshapes result -vertex 14 -edge 7 -t
```

### Test: boolean/bcommon_2d/C2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C2
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge2.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_edge2.brep] b

bcommon result b a

checkprops result -l 847.007
checksection result
checknbshapes result -vertex 14 -edge 7 -t
```

### Test: boolean/bcommon_2d/C3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C3
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge3.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_edge3.brep] b

bcommon result b a

checkprops result -l 753.315
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/C4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C4
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire1.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_wire1.brep] b

bcommon result b a

checkprops result -l 874
checksection result
checknbshapes result -vertex 18 -edge 11 -t
```

### Test: boolean/bcommon_2d/C5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C5
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire2.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_wire2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/C6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C6
- **Data Files**: `case_2_shell_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_shell_r.brep`), `case_2_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire3.brep`)
```tcl
restore [locate_data_file case_2_shell_r.brep] a
restore [locate_data_file case_2_wire3.brep] b

bcommon result b a

checkprops result -l 846.821
checksection result
checknbshapes result -vertex 9 -edge 6 -t
```

### Test: boolean/bcommon_2d/C7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C7
- **Data Files**: `case_2_face_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_face_r.brep`), `case_2_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge1.brep`)
```tcl
restore [locate_data_file case_2_face_r.brep] a
restore [locate_data_file case_2_edge1.brep] b

bcommon result b a

checkprops result -l 847.007
checksection result
checknbshapes result -vertex 14 -edge 7 -t
```

### Test: boolean/bcommon_2d/C8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C8
- **Data Files**: `case_2_face_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_face_r.brep`), `case_2_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_edge3.brep`)
```tcl
restore [locate_data_file case_2_face_r.brep] a
restore [locate_data_file case_2_edge3.brep] b

bcommon result b a

checkprops result -l 753.315
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/C9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: C9
- **Data Files**: `case_2_face_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_face_r.brep`), `case_2_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire1.brep`)
```tcl
restore [locate_data_file case_2_face_r.brep] a
restore [locate_data_file case_2_wire1.brep] b

bcommon result b a

checkprops result -l 874
checksection result
checknbshapes result -vertex 18 -edge 11 -t
```

--- BATCH 2 END ---

--- BATCH 3 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/D1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D1
- **Data Files**: `case_2_face_r.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_face_r.brep`), `case_2_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_2_wire3.brep`)
```tcl
restore [locate_data_file case_2_face_r.brep] a
restore [locate_data_file case_2_wire3.brep] b

bcommon result b a

checkprops result -l 846.821
checksection result
checknbshapes result -vertex 9 -edge 6 -t
```

### Test: boolean/bcommon_2d/D2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D2
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge1.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_edge1.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/D3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D3
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge2.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_edge2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/D4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D4
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `offset_wire_034.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\offset_wire_034.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file offset_wire_034.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/D5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D5
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire2.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_wire2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 14 -edge 13 -t
```

### Test: boolean/bcommon_2d/D6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D6
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire3.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_wire3.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/D7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D7
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire4.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_wire4.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/D8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D8
- **Data Files**: `case_3_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_solid.brep`), `case_3_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire5.brep`)
```tcl
restore [locate_data_file case_3_solid.brep] a
restore [locate_data_file case_3_wire5.brep] b

bcommon result b a

checkprops result -l 292.732
checksection result
checknbshapes result -vertex 4 -edge 4 -t
```

### Test: boolean/bcommon_2d/D9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: D9
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge1.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_edge1.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E1
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge2.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_edge2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

--- BATCH 3 END ---

--- BATCH 4 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/E2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E2
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `offset_wire_034.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\offset_wire_034.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file offset_wire_034.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E3
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire2.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_wire2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 14 -edge 13 -t
```

### Test: boolean/bcommon_2d/E4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E4
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire3.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_wire3.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E5
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire4.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_wire4.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E6
- **Data Files**: `case_3_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_shell.brep`), `case_3_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire5.brep`)
```tcl
restore [locate_data_file case_3_shell.brep] a
restore [locate_data_file case_3_wire5.brep] b

bcommon result b a

checkprops result -l 292.732
checksection result
checknbshapes result -vertex 4 -edge 4 -t
```

### Test: boolean/bcommon_2d/E7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E7
- **Data Files**: `case_3_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_face.brep`), `case_3_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge1.brep`)
```tcl
restore [locate_data_file case_3_face.brep] a
restore [locate_data_file case_3_edge1.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E8
- **Data Files**: `case_3_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_face.brep`), `offset_wire_034.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\offset_wire_034.brep`)
```tcl
restore [locate_data_file case_3_face.brep] a
restore [locate_data_file offset_wire_034.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/E9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: E9
- **Data Files**: `case_3_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_face.brep`), `case_3_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire2.brep`)
```tcl
restore [locate_data_file case_3_face.brep] a
restore [locate_data_file case_3_wire2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 14 -edge 13 -t
```

### Test: boolean/bcommon_2d/F1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F1
- **Data Files**: `case_3_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_face.brep`), `case_3_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire5.brep`)
```tcl
restore [locate_data_file case_3_face.brep] a
restore [locate_data_file case_3_wire5.brep] b

bcommon result b a

checkprops result -l 66.3661
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/F2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F2
- **Data Files**: `case_3_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire.brep`), `case_3_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_edge1.brep`)
```tcl
restore [locate_data_file case_3_wire.brep] a
restore [locate_data_file case_3_edge1.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

--- BATCH 4 END ---

--- BATCH 5 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/F3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F3
- **Data Files**: `case_3_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire.brep`), `offset_wire_034.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\offset_wire_034.brep`)
```tcl
restore [locate_data_file case_3_wire.brep] a
restore [locate_data_file offset_wire_034.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 8 -edge 7 -t
```

### Test: boolean/bcommon_2d/F4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F4
- **Data Files**: `case_3_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire.brep`), `case_3_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire2.brep`)
```tcl
restore [locate_data_file case_3_wire.brep] a
restore [locate_data_file case_3_wire2.brep] b

bcommon result b a

checkprops result -l 345.975
checksection result
checknbshapes result -vertex 14 -edge 13 -t
```

### Test: boolean/bcommon_2d/F5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F5
- **Data Files**: `case_3_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire.brep`), `case_3_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_3_wire5.brep`)
```tcl
restore [locate_data_file case_3_wire.brep] a
restore [locate_data_file case_3_wire5.brep] b

bcommon result b a

checkprops result -l 66.3661
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/F6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F6
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge1.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_edge1.brep] b

bcommon result b a

checkprops result -l 1231.36
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/F7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F7
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge2.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_edge2.brep] b

bcommon result b a

checkprops result -l 1231.36
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/F8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F8
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge3.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_edge3.brep] b

bcommon result b a

checkprops result -l 375.748
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/F9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: F9
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire1.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_wire1.brep] b

bcommon result b a

checkprops result -l 2553.95
checksection result
checknbshapes result -vertex 3 -edge 3 -t
```

### Test: boolean/bcommon_2d/G1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: G1
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire2.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_wire2.brep] b

bcommon result b a

checkprops result -l 2553.95
checksection result
checknbshapes result -vertex 3 -edge 3 -t
```

### Test: boolean/bcommon_2d/G2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: G2
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire3.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_wire3.brep] b

bcommon result b a

checkprops result -l 633.071
checksection result
checknbshapes result -vertex 9 -edge 7 -t
```

### Test: boolean/bcommon_2d/G3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: G3
- **Data Files**: `case_4_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_solid.brep`), `case_4_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire4.brep`)
```tcl
restore [locate_data_file case_4_solid.brep] a
restore [locate_data_file case_4_wire4.brep] b

bcommon result b a

checkprops result -l 633.071
checksection result
checknbshapes result -vertex 9 -edge 7 -t
```

--- BATCH 5 END ---

--- BATCH 6 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/G8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: G8
- **Data Files**: `case_4_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_face.brep`), `case_4_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge1.brep`)
```tcl
restore [locate_data_file case_4_face.brep] a
restore [locate_data_file case_4_edge1.brep] b

bcommon result b a

checkprops result -l 615.235
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/G9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: G9
- **Data Files**: `case_4_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_face.brep`), `case_4_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge3.brep`)
```tcl
restore [locate_data_file case_4_face.brep] a
restore [locate_data_file case_4_edge3.brep] b

bcommon result b a

checkprops result -l 474.547
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

### Test: boolean/bcommon_2d/H1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H1
- **Data Files**: `case_4_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_face.brep`), `case_4_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire1.brep`)
```tcl
restore [locate_data_file case_4_face.brep] a
restore [locate_data_file case_4_wire1.brep] b

bcommon result b a

checkprops result -l 1245.29
checksection result
checknbshapes result -vertex 10 -edge 5 -t
```

### Test: boolean/bcommon_2d/H2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H2
- **Data Files**: `case_4_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_face.brep`), `case_4_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire3.brep`)
```tcl
restore [locate_data_file case_4_face.brep] a
restore [locate_data_file case_4_wire3.brep] b

bcommon result b a

checkprops result -l 1150.78
checksection result
checknbshapes result -vertex 15 -edge 10 -t
```

### Test: boolean/bcommon_2d/H3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H3
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_4_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge1.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_4_edge1.brep] b

bcommon result b a

checkprops result -l 887.185
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/H4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H4
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_5_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_edge2.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_5_edge2.brep] b

bcommon result b a

checkprops result -l 887.185
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/H5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H5
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_4_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge3.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_4_edge3.brep] b

bcommon result b a

checkprops result -l 829.541
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/H6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H6
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_4_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire1.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_4_wire1.brep] b

bcommon result b a

checkprops result -l 1747.73
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

### Test: boolean/bcommon_2d/H7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H7
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_4_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire2.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_4_wire2.brep] b

bcommon result b a

checkprops result -l 1747.73
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

### Test: boolean/bcommon_2d/H8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H8
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_4_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire3.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_4_wire3.brep] b

bcommon result b a

checkprops result -l 1808.02
checksection result
checknbshapes result -vertex 12 -edge 11 -t
```

--- BATCH 6 END ---

--- BATCH 7 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/H9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: H9
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_5_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_wire4.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_5_wire4.brep] b

bcommon result b a

checkprops result -l 1808.02
checksection result
checknbshapes result -vertex 12 -edge 11 -t
```

### Test: boolean/bcommon_2d/I1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I1
- **Data Files**: `case_5_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_solid.brep`), `case_5_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_wire5.brep`)
```tcl
restore [locate_data_file case_5_solid.brep] a
restore [locate_data_file case_5_wire5.brep] b

bcommon result b a

checkprops result -l 3000.52
checksection result
checknbshapes result -vertex 2 -edge 2 -t
```

### Test: boolean/bcommon_2d/I2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I2
- **Data Files**: `case_5_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_shell.brep`), `case_4_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge1.brep`)
```tcl
restore [locate_data_file case_5_shell.brep] a
restore [locate_data_file case_4_edge1.brep] b

bcommon result b a

checkprops result -l 887.185
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/I3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I3
- **Data Files**: `case_5_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_shell.brep`), `case_4_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge3.brep`)
```tcl
restore [locate_data_file case_5_shell.brep] a
restore [locate_data_file case_4_edge3.brep] b

bcommon result b a

checkprops result -l 829.541
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/I4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I4
- **Data Files**: `case_5_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_shell.brep`), `case_4_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire1.brep`)
```tcl
restore [locate_data_file case_5_shell.brep] a
restore [locate_data_file case_4_wire1.brep] b

bcommon result b a

checkprops result -l 1747.73
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

### Test: boolean/bcommon_2d/I5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I5
- **Data Files**: `case_5_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_shell.brep`), `case_4_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire3.brep`)
```tcl
restore [locate_data_file case_5_shell.brep] a
restore [locate_data_file case_4_wire3.brep] b

bcommon result b a

checkprops result -l 1808.02
checksection result
checknbshapes result -vertex 12 -edge 11 -t
```

### Test: boolean/bcommon_2d/I6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I6
- **Data Files**: `case_5_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_shell.brep`), `case_5_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_wire5.brep`)
```tcl
restore [locate_data_file case_5_shell.brep] a
restore [locate_data_file case_5_wire5.brep] b

bcommon result b a

checkprops result -l 3000.52
checksection result
checknbshapes result -vertex 2 -edge 2 -t
```

### Test: boolean/bcommon_2d/I7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I7
- **Data Files**: `case_5_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_face.brep`), `case_4_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge1.brep`)
```tcl
restore [locate_data_file case_5_face.brep] a
restore [locate_data_file case_4_edge1.brep] b

bcommon result b a

checkprops result -l 887.185
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/I8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I8
- **Data Files**: `case_5_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_face.brep`), `case_4_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_edge3.brep`)
```tcl
restore [locate_data_file case_5_face.brep] a
restore [locate_data_file case_4_edge3.brep] b

bcommon result b a

checkprops result -l 829.541
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/I9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: I9
- **Data Files**: `case_5_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_face.brep`), `case_4_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire1.brep`)
```tcl
restore [locate_data_file case_5_face.brep] a
restore [locate_data_file case_4_wire1.brep] b

bcommon result b a

checkprops result -l 1747.73
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

--- BATCH 7 END ---

--- BATCH 8 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/J1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: J1
- **Data Files**: `case_5_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_5_face.brep`), `case_4_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_4_wire3.brep`)
```tcl
restore [locate_data_file case_5_face.brep] a
restore [locate_data_file case_4_wire3.brep] b

bcommon result b a

checkprops result -l 1808.02
checksection result
checknbshapes result -vertex 12 -edge 11 -t
```

### Test: boolean/bcommon_2d/K8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: K8
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge1.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_edge1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/K9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: K9
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge2.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_edge2.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L1
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge3.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_edge3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L2
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge4.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_edge4.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L3
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire1.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_wire1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L4
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire2.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_wire2.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L5
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire3.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_wire3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L6
- **Data Files**: `case_7_solid.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_solid.brep`), `case_7_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire4.brep`)
```tcl
restore [locate_data_file case_7_solid.brep] a
restore [locate_data_file case_7_wire4.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L7
- **Data Files**: `case_7_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_shell.brep`), `case_7_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge1.brep`)
```tcl
restore [locate_data_file case_7_shell.brep] a
restore [locate_data_file case_7_edge1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

--- BATCH 8 END ---

--- BATCH 9 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/L8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L8
- **Data Files**: `case_7_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_shell.brep`), `case_7_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge3.brep`)
```tcl
restore [locate_data_file case_7_shell.brep] a
restore [locate_data_file case_7_edge3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/L9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: L9
- **Data Files**: `case_7_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_shell.brep`), `case_7_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire1.brep`)
```tcl
restore [locate_data_file case_7_shell.brep] a
restore [locate_data_file case_7_wire1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M1
- **Data Files**: `case_7_shell.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_shell.brep`), `case_7_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire3.brep`)
```tcl
restore [locate_data_file case_7_shell.brep] a
restore [locate_data_file case_7_wire3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M2
- **Data Files**: `case_7_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_face.brep`), `case_7_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge1.brep`)
```tcl
restore [locate_data_file case_7_face.brep] a
restore [locate_data_file case_7_edge1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M3
- **Data Files**: `case_7_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_face.brep`), `case_7_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_edge3.brep`)
```tcl
restore [locate_data_file case_7_face.brep] a
restore [locate_data_file case_7_edge3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M4
- **Data Files**: `case_7_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_face.brep`), `case_7_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire1.brep`)
```tcl
restore [locate_data_file case_7_face.brep] a
restore [locate_data_file case_7_wire1.brep] b

bcommon result b a

checkprops result -l 482.392
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M5
- **Data Files**: `case_7_face.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_face.brep`), `case_7_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_7_wire3.brep`)
```tcl
restore [locate_data_file case_7_face.brep] a
restore [locate_data_file case_7_wire3.brep] b

bcommon result b a

checkprops result -l 520.393
checksection result
checknbshapes result -vertex 6 -edge 3 -t
```

### Test: boolean/bcommon_2d/M6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M6
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire1.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire1.brep] b

bcommon result b a

checkprops result -l 181.079
checksection result
checknbshapes result -vertex 3 -edge 2 -t
```

### Test: boolean/bcommon_2d/M7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M7
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire2.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire2.brep] b

bcommon result b a

checkprops result -l 49.966
checksection result
checknbshapes result -vertex 4 -edge 2 -t
```

### Test: boolean/bcommon_2d/M8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M8
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire3.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire3.brep] b

bcommon result b a

checkprops result -l 3.83899
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

--- BATCH 9 END ---

--- BATCH 10 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/M9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: M9
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire4.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire4.brep] b

bcommon result b a

checkprops result -l 36.0874
checksection result
checknbshapes result -vertex 7 -edge 4 -t
```

### Test: boolean/bcommon_2d/N1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N1
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire5.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire5.brep] b

bcommon result b a

checkprops result -l 126.81
checksection result
checknbshapes result -vertex 6 -edge 4 -t
```

### Test: boolean/bcommon_2d/N2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N2
- **Data Files**: `case_8_solid_repaired.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_solid_repaired.brep`), `case_8_wire6.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_8_wire6.brep`)
```tcl
restore [locate_data_file case_8_solid_repaired.brep] a
restore [locate_data_file case_8_wire6.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/N3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N3
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge1.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_edge1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/N4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N4
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge2.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_edge2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/N5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N5
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge3.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_edge3.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/N6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N6
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge4.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_edge4.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/N7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N7
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_edge5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge5.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_edge5.brep] b

bcommon result b a

checkprops result -l 79.9002
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/N8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N8
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire1.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_wire1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/N9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: N9
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire2.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_wire2.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

--- BATCH 10 END ---

--- BATCH 11 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/O1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O1
- **Data Files**: `case_9_wire.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire.brep`), `case_9_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire3.brep`)
```tcl
restore [locate_data_file case_9_wire.brep] a
restore [locate_data_file case_9_wire3.brep] b

bcommon result b a

checkprops result -l 79.9002
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/O2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O2
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge1.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_edge1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/O3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O3
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge2.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_edge2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/O4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O4
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge3.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_edge3.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/O5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O5
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge4.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_edge4.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/O6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O6
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_edge5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge5.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_edge5.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/O7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O7
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire1.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_wire1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/O8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O8
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire2.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_wire2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/O9
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: O9
- **Data Files**: `case_9_wire_a.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire_a.brep`), `case_9_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire3.brep`)
```tcl
restore [locate_data_file case_9_wire_a.brep] a
restore [locate_data_file case_9_wire3.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P1
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P1
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_edge1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge1.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_edge1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

--- BATCH 11 END ---

--- BATCH 12 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: boolean/bcommon_2d/P2
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P2
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_edge2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge2.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_edge2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P3
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P3
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_edge3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge3.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_edge3.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P4
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P4
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_edge4.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge4.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_edge4.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P5
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P5
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_edge5.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge5.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_edge5.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P6
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P6
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_wire1.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire1.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_wire1.brep] b

bcommon result b a

checkprops result -l 143.787
checksection result
checknbshapes result -vertex 2 -edge 1 -t
```

### Test: boolean/bcommon_2d/P7
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P7
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_wire2.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire2.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_wire2.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

### Test: boolean/bcommon_2d/P8
- **Target Suite**: OCCT_Boolean_bcommon_2d
- **Target Case**: P8
- **Data Files**: `case_9_edge.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_edge.brep`), `case_9_wire3.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\case_9_wire3.brep`)
```tcl
restore [locate_data_file case_9_edge.brep] a
restore [locate_data_file case_9_wire3.brep] b

bcommon result b a

checkprops result -l empty
checksection result
checknbshapes result -vertex 0 -edge 0 -t
```

--- BATCH 12 END ---

