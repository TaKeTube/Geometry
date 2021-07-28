# Hex Eval

Evaluate the density of a Hex mesh then output the mesh with scalar field indicating density of each element.

#### Libraries

- VTK
- Eigen

#### I/O & Parameter

- **INPUT**: <kbd>.vtk</kbd> unstructured hex mesh file
- **OUTPUT**: <kbd>.vtk</kbd> unstructured hex mesh file
- <kbd>-i arg</kbd> : input, arg: input file name, default: ../data/cad.vtk

- <kbd>-o arg</kbd> : output, arg: output file name, default: output.vtk
- <kbd>-m arg</kbd> : density metric, arg: **len/vol/anisotropic**
- <kbd>-r</kbd>   : output reference field if setted
- <kbd>-d</kbd>   : output the difference between the actual density field and the reference field
- <kbd>-h</kbd>   : help

using command line to choose input and output files, a example command is like follow:

```shell
./HexEval.exe -i "../data/cad.vtk" -o "cad_eval.vtk" -m "len" -r -d
```

#### Density Metric

There are three types of density metric

- len
  - using reciprocal of the average of edges of a hex cell
- vol
  - using reciprocal of the volume of a hex cell
- anisotropic
  - using average of edge metrics described in *Automated refinement of conformal quadrilateral and hexahedral meshes - Tchon KF, Dompierre J, Camarero R*  of a hex cell
  - PS: By using the average of edge metrics, the metric is actually not anisotropic.

#### Density Field

Density field is evaluated for each cells

There are two types of density field

- Normal density field is a scalar function of 3D vector, which should be modified at line 115/116
  - Reference density field are evaluate using average of vertexes of a hex cell

- Anisotropic density field is the M matrix described in *Automated refinement of conformal quadrilateral and hexahedral meshes - Tchon KF, Dompierre J, Camarero R* , which should be modified at 121/122
  - There is no such thing called reference and difference field in this case.


#### Result

Hex density using len metric

<img src=".\img\eval.png" alt="eval" />

Reference field

<img src=".\img\ref.png" alt="ref" />

Difference between the actual density field and the reference field

<img src=".\img\diff.png" alt="diff" />

