OpenGL tutorial UI for progressive mesh reduction with edge collapse, demo based on code by Stan Melax, published in 1998. 

## Edge collapse algorithm for mesh reduction ##

 **1.** Select edge *uv* to collapse according to a criterion of lowest cost of loss of topology. 
 
 Melax chooses to assign lowest cost of loss of topology to the edge uv that is  most highly coplanar. 
 
Cost of collapsing an edge = length of edge * curvature term determined by dot products of face normals
 
 To determine this "curvature term", we seek the triangle adjacent to vertex u that's furthest away from the other triangles that are along uv. 

 **2.** Remove triangles on the selected edge uv (u is going to be removed)
 
 **3.** Update the remaining triangles that used vertex u, to use vertex v instead.
 
 **4.** Remove vertex u. 


![edge collapse.JPG](https://bitbucket.org/repo/krqkKE/images/3133645255-edge%20collapse.JPG)

![polygon reduction.JPG](https://bitbucket.org/repo/krqkKE/images/1356747877-polygon%20reduction.JPG)

Lightning, and controls done with GLUI. 

[Melax's method](http://dev.gameres.com/Program/Visual/3D/PolygonReduction.pdf) finds and collapses the edge that when eliminated affects the model's topology the least.


[Demo #1](https://www.youtube.com/watch?v=Hjkfr_ujwdo)

[Demo #2](https://www.youtube.com/watch?v=mmU1BLuisp8)


A tutorial video showing how to install GLUI in Visual Studio 2010 can be seen [here](https://www.youtube.com/watch?v=nmMY-5-GqJE).

Code tested in Visual Studio 2010 Professional SP 1, Win 7. 