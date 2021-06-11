# Hex Refinement

Implement **local Hex mesh refinement** algorithm described in

*[CONFORMAL REFINEMENT OF ALL-HEXAHEDRAL ELEMENT  MESHES BASED ON MULTIPLE TWIST PLANE INSERTION](https://imr.sandia.gov/papers/imr13/harris.pdf)*  by Harris et al.

#### Libraries

- VTK

#### I/O

- **INPUT**: <kbd>.vtk</kbd> unstructured hex mesh file & <kbd>.txt</kbd> contains indexes of selected vertexes

- **OUTPUT**: <kbd>.vtk</kbd> refined unstructured hex mesh file

the default input file is <kbd>./data/cad.vtk</kbd>, default outputs is <kbd>output.vtk</kbd>, default selected vertexes are described in  <kbd>./data/cad_refine.vtk</kbd>

using command line to choose input and output files, a example command is like follow:

```shell
./HexRefinement.exe -input "../data/rod.vtk" -output "refined_rod.vtk" -refine "../data/rod_refine.txt"
```

#### How to select vertexes

Here is one method to get indexes of selected vertexes

1. Open the target mesh using **ParaView**
2. Click the button <kbd>interactive Select Points On</kbd> on the top of the display window ![button](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/img/button.png?raw=true)
3. You will see Selection Display Inspector on the right of the display window. Click <kbd>Point Labels</kbd> and tick the ID option :white_check_mark:

![ID](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/img/ID.png?raw=true)

4. Then you will see the index when your cursor is on the vertex. Write indexes into a txt file then run the program.

<img src="https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/img/points.png?raw=true" alt="points" style="zoom: 67%;" />

#### Results

**Vertex Refinement**

![](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/results/vertex.png?raw=true)

**Edge Refinement**

![](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/results/edge.png?raw=true)

**Face Refinement**

![](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/results/face.png?raw=true)

**Remove Concavity**

Notice that even the selected vertexes set is concave, the algorithm would automatically remove the concavity and choose a standard configuration.

![](https://github.com/TaKeTube/Geometry/blob/main/HexRefinement/results/concavity.png?raw=true)
