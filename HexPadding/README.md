# Hex Padding

Implement **Hex mesh padding/shrinking/pillowing** algorithm

Pad marked cells then do smoothing

#### Reference

-  Gianmarco Cherchi, Pierre Alliez, Riccardo Scateni, Max Lyon, David Bommes. **Selective Padding for Polycube-Based Hexahedral Meshing**. Computer Graphics Forum, Wiley, 2019, ff10.1111/cgf.13593ff. ffhal-01970790f
- MITCHELL S. A., TAUTGES T. J.: **Pillowing doublets: refining a mesh to ensure that faces share at most one edge**. In 4th International Meshing Roundtable (1995), Citeseer, pp. 231–240.

#### Libraries

- VTK
- Eigen

#### I/O

- **INPUT**: <kbd>.vtk</kbd> unstructured hex mesh file & <kbd>.txt</kbd> contains indexes of target cells

- **OUTPUT**: <kbd>.vtk</kbd> padded unstructured hex mesh file

the default input file is <kbd>./data/8cube.vtk</kbd>, default outputs is <kbd>output.vtk</kbd>, default marked cells are described in  <kbd>./data/8cube_target.vtk</kbd>, smooth is not done by default. Add <kbd>-r</kbd> flag to do smoothing.

using command line to choose input and output files, a example command is like follow:

```shell
./Padding.exe -r -input "../data/rod.vtk" -output "padded_rod.vtk" -target "../data/rod_target.txt"
```

#### How to mark cells

Here is one method to get indexes of cells

1. Open the target mesh using **ParaView**
2. Click the button <kbd>Select Cells On</kbd> on the top of the display window ![button](https://github.com/TaKeTube/Geometry/blob/main/HexPadding/img/button.png?raw=true)
3. You will see Selection Display Inspector on the right of the display window. Click <kbd>Cell Labels</kbd> and tick the ID option :white_check_mark:

![ID](https://github.com/TaKeTube/Geometry/blob/main/HexPadding/img/ID.png?raw=true)

4. Then you will see the index when your cursor is on the cell. Write cell indexes into a txt file then run the program.

<img src="https://github.com/TaKeTube/Geometry/blob/main/HexPadding/img/cell.png?raw=true" alt="points" style="zoom: 67%;" />

#### Results

<img src="https://github.com/TaKeTube/Geometry/blob/main/HexPadding/img/cube1.png?raw=true" alt="points" style="zoom: 67%;" />

<img src="https://github.com/TaKeTube/Geometry/blob/main/HexPadding/img/cube2.png?raw=true" alt="points" style="zoom: 67%;" />

