# Hex Refinement

Implement local Hex mesh refinement algorithm described in

*[CONFORMAL REFINEMENT OF ALL-HEXAHEDRAL ELEMENT  MESHES BASED ON MULTIPLE TWIST PLANE INSERTION](https://imr.sandia.gov/papers/imr13/harris.pdf)*  by Harris et al.

#### Libraries

- VTK

#### I/O

- **INPUT**: <kbd>.vtk</kbd> unstructured hex mesh file & <kbd>.txt</kbd> contains indexes of selected vertexes

- **OUTPUT**: <kbd>.vtk</kbd> refined unstructured hex mesh file

the default input file is <kbd>./data/rod.vtk</kbd>, default outputs is <kbd>output.vtk</kbd> 

using command line to choose input and output files, a example command is like follow:

```shell
./HexRefinement.exe -input "../data/rod.vtk" -output "refined_rod.vtk" -refine "../data/rod_refine.txt"
```

#### How to select vertexes

Here is one method to get indexes of selected vertexes

1. Open the target mesh using **ParaView**
2. Click the button <kbd>interactive Select Points On</kbd> on the top of the display window ![button](.\img\button.png)
3. You will see Selection Display Inspector on the right of the display window. Click <kbd>Point Labels</kbd> and tick the ID option :white_check_mark:

![ID](.\img\ID.png)

4. Then you will see the index when your cursor is on the vertex. Write indexes into a txt file then run the program.

<img src=".\img\points.png" alt="points" style="zoom: 67%;" />

#### Results

**Vertex Refinement**

**Edge Refinement**

**Face Refinement**

**Remove Concavity**

