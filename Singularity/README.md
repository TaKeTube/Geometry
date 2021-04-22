# Singularity

Find Inner Singular Line and Surface Singular Line of a given Hex mesh.

#### Definition 

- An edge's valence is the number of its adjacent hex cell.

- An Inner singular edge is an edge whose valence not equal to 4.

- A surface singular edge is an edge whose valence not equal to 2.

#### Libraries

- OpenVolumeMesh
- VTK (optional, by changing macro <kbd>USING_VTK</kbd> in **vtkIO.cpp** )

#### I/O

- **INPUT**: <kbd>.ovm</kbd> hex mesh file (OpenVolumeMesh standard file)

- **OUTPUT**: <kbd>.vtk</kbd> ploy data with vtkLine cell type **x2**, one is inner singular line, another is surface singular line.

the default input file is <kbd>./data/bunny.ovm</kbd>, default outputs are <kbd>innerSingularity.vtk</kbd> & <kbd>boundarySingularity.vtk</kbd>, or

using command line to choose input and output files, a example command is like follow:

```shell
./Singularity.exe -input "../data/dragon.ovm" -iout "dragonInnerSingular.vtk" -bout "dragonSurfaceSingular.vtk"
```

#### Results

Blue lines are inner singular lines, Green lines are surface singular line.

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/38636.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/CAD5_1.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/bunny.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/double_torus.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/dragon.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/rod.png?raw=true" style="zoom:80%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/Singularity/results/sculpt.png?raw=true" style="zoom:80%;" />