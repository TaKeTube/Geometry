# Density Field Hex Refinement

A pipeline that refines all-hex mesh according to given density field then evaluates the result.

Incomplete because the padding is not optimized.

### Libraries

- VTK
- Eigen

- HexRefinement: https://github.com/TaKeTube/Geometry/tree/main/HexRefinement
- HexPadding: https://github.com/TaKeTube/Geometry/tree/main/HexPadding
- HexEval: https://github.com/TaKeTube/Geometry/tree/main/HexEval

see README of HexRefinement, HexPadding, HexEval for more information.

### I/O & Parameter

- **INPUT**: <kbd>.vtk</kbd> unstructured hex mesh file
- **OUTPUT**: <kbd>.vtk</kbd> unstructured hex mesh file
- <kbd>-i arg</kbd> : input vtk file, arg: input vtk file name, default: <kbd>../data/cad.vtk</kbd>
- <kbd>-o arg</kbd> : output vtk file, arg: output vtk file name, default: <kbd>output.vtk</kbd>
- <kbd>-d arg</kbd> : density metric, arg: <kbd>len</kbd>/<kbd>vol</kbd>, default: <kbd>len</kbd>
- <kbd>-r</kbd>   : refine method, arg: <kbd>padding</kbd>/<kbd>trivial</kbd>, default: <kbd>padding</kbd>
- <kbd>-t</kbd>   : number of iterations, arg: number of iterations, default: 3
- <kbd>-s</kbd>   : smooth the padded mesh
- <kbd>-m</kbd>   : output mesh with padded element marked using scalar 1
- <kbd>-e</kbd>   : evaluate the results, output field of the result mesh, reference & difference field
- <kbd>-h</kbd>   : help

using command line to choose input and output files, a example command is like follow:

```shell
./HexRefinement.exe -i "../data/rod.vtk" -o "refined_rod.vtk" -s -e
```

### Refine Method

There are two types of refine method.

- Padding refinement
  - see https://github.com/TaKeTube/Geometry/tree/main/HexPadding
- Trivial refinement
  - see https://github.com/TaKeTube/Geometry/tree/main/HexRefinement

### Density Metric

There are two types of density metric (for the sake of convenience, anisotropic metric is removed)

- len
  - using reciprocal of the average of edges of a hex cell
- vol
  - using reciprocal of the volume of a hex cell

see https://github.com/TaKeTube/Geometry/tree/main/HexEval for more information.

### Result

**Padding Method**

Refine according to the field <img src="http://latex.codecogs.com/svg.latex?50(1 + \sin{3y})">, 3 iterations

<img src=".\img\pad.png" alt="pad" />

Result hex density using len metric

<img src=".\img\pad_field.png" alt="pad_field" />

Reference field <img src="http://latex.codecogs.com/svg.latex?50(1 + \sin{3y})">

<img src=".\img\pad_ref.png" alt="pad_ref" />

Difference between the density field of the result mesh and the reference field

<img src=".\img\pad_diff.png" alt="pad_diff" />

Marked padded hex cell

<img src=".\img\pad_mark.png" alt="pad_mark" />

**Trivial Method**

Refine according to the field <img src="http://latex.codecogs.com/svg.latex?50(1 + \sin{3y})">, 1 iteration

<img src=".\img\trivial.png" alt="trivial" />

Result hex density using len metric

<img src=".\img\trivial_field.png" alt="trivial_field" />

Difference between the density field of the result mesh and the reference field

<img src=".\img\trivial_diff.png" alt="trivial_diff" />

### Limitation

The hex mesh must be optimized after padding, otherwise the quality of the mesh would be terrible. According to ***Selective Padding for Polycube-Based Hexahedral Meshing***, the optimizer could be chosen from ***Practical Hex-Mesh Optimization via Edge-Cone Rectification***. http://www.cs.ubc.ca/labs/imager/tr/2015/untangler/

Another point needs to be improved is the target mesh select part. Some isolated cell may be selected thus lead to a bad geometry after padding. Therefore, an optimization of selection is needed in this part.
