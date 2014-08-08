
#include <windows.h>
#include <stdio.h>  
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <GL/gl.h>
#pragma warning(disable : 4244)

#include "vector.h"
#include "font.h"
#include "progmesh.h"

#include <iostream>
#include <fstream>
#include <sstream>

extern int wireframe;


extern float DeltaT;  // change in time since last frame
int   render_num;   // number of vertices to draw with

float lodbase=0.5f; // the fraction of vertices used to morph toward
float morph=1.0f;   // where to render between 2 levels of detail


List<Vector> vert;       // global list of vertices
List<tridata> tri;       // global list of triangles
List<int> collapse_map;  // to which neighbor each vertex collapses
int renderpolycount=0;   // polygons rendered in the current frame
Vector model_position;         // position of bunny
Quaternion model_orientation;  // orientation of bunny

// Note that the use of the Map() function and the collapse_map
// list isn't part of the polygon reduction algorithm.
// We just set up this system here in this module
// so that we could retrieve the model at any desired vertex count.
// Therefore if this part of the program confuses you, then
// dont worry about it.  It might help to look over the progmesh.cpp
// module first.

//       Map()
//
// When the model is rendered using a maximum of mx vertices
// then it is vertices 0 through mx-1 that are used.  
// We are able to do this because the vertex list 
// gets sorted according to the collapse order.
// The Map() routine takes a vertex number 'a' and the
// maximum number of vertices 'mx' and returns the 
// appropriate vertex in the range 0 to mx-1.
// When 'a' is greater than 'mx' the Map() routine
// follows the chain of edge collapses until a vertex
// within the limit is reached.
//   An example to make this clear: assume there is
//   a triangle with vertices 1, 3 and 12.  But when
//   rendering the model we limit ourselves to 10 vertices.
//   In that case we find out how vertex 12 was removed
//   by the polygon reduction algorithm.  i.e. which
//   edge was collapsed.  Lets say that vertex 12 was collapsed
//   to vertex number 7.  This number would have been stored
//   in the collapse_map array (i.e. collapse_map[12]==7).
//   Since vertex 7 is in range (less than max of 10) we
//   will want to render the triangle 1,3,7.  
//   Pretend now that we want to limit ourselves to 5 vertices.
//   and vertex 7 was collapsed to vertex 3 
//   (i.e. collapse_map[7]==3).  Then triangle 1,3,12 would now be
//   triangle 1,3,3.  i.e. this polygon was removed by the
//   progressive mesh polygon reduction algorithm by the time
//   it had gotten down to 5 vertices.
//   No need to draw a one dimensional polygon. :-)
int Map(int a,int mx) {
	if(mx<=0) return 0;
	while(a>=mx) {  
		a=collapse_map[a];
	}
	return a;
}

int mode;
void DrawModelTriangles() {
	assert(collapse_map.num);
	renderpolycount=0;
	int i=0;
	for(i=0;i<tri.num;i++) {
		int p0= Map(tri[i].v[0],render_num);
		int p1= Map(tri[i].v[1],render_num);
		int p2= Map(tri[i].v[2],render_num);
		// note:  serious optimization opportunity here,
		//  by sorting the triangles the following "continue" 
		//  could have been made into a "break" statement.
		if(p0==p1 || p1==p2 || p2==p0) continue;
		renderpolycount++;
		
		// if we are not currenly morphing between 2 levels of detail
		// (i.e. if morph=1.0) then q0,q1, and q2 are not necessary.
	
		int q0= Map(p0,(int)(render_num*lodbase));
		int q1= Map(p1,(int)(render_num*lodbase));
		int q2= Map(p2,(int)(render_num*lodbase));
		Vector v0,v1,v2; 
		v0 = vert[p0]*morph + vert[q0]*(1-morph);
		v1 = vert[p1]*morph + vert[q1]*(1-morph);
		v2 = vert[p2]*morph + vert[q2]*(1-morph);
		
        int mode = GL_POLYGON;
		if (wireframe){mode = GL_LINES;}
		if (!wireframe){mode=GL_POLYGON;}

		glBegin(mode);
			// the purpose of the demo is to show polygons
			// therefore just use 1 face normal (flat shading)
			Vector nrml = (v1-v0) * (v2-v1);  // cross product
			if(0<magnitude(nrml)) {
				glNormal3fv(normalize(nrml));
			}
			glVertex3fv(v0);
			glVertex3fv(v1);
			glVertex3fv(v2);
		glEnd();
	}
}


void PermuteVertices(List<int> &permutation) {
	// rearrange the vertex list 
	List<Vector> temp_list;
	int i;
	assert(permutation.num==vert.num);
	for(i=0;i<vert.num;i++) {
		temp_list.Add(vert[i]);
	}
	for(i=0;i<vert.num;i++) {
		vert[permutation[i]]=temp_list[i];
	}
	// update the changes in the entries in the triangle list
	for(i=0;i<tri.num;i++) {
		for(int j=0;j<3;j++) {
			tri[i].v[j] = permutation[tri[i].v[j]];
		}
	}
}



using namespace std;
int initial_num;
void GetModelData(string offFileName){ // change to get model data y pasar como argumento el string con el filename
	// Copy the geometry from the arrays of data in rabdata.cpp into
	// the vert and tri lists which we send to the reduction routine
	

	
	//string offFileName= "bulldog.off";

	//string offFileName= "jacky.off";

   // string offFileName = "monster.off";

	//string offFileName = "nemesis.off";

	//string offFileName = "parakeet.off";

	//string offFileName = "sphere.off";

	//string offFileName = "tiger.off";

	/*string offFileName = "woman.off";*/

	

	ifstream offFileStream;

    offFileStream.open(offFileName.data());

	assert (offFileStream.is_open());

	string actualLine;

	offFileStream>>actualLine;

    int MODEL_VERTEX_NUM;
	int MODEL_TRIANGLE_NUM;

	offFileStream>>MODEL_VERTEX_NUM; 

	offFileStream>>MODEL_TRIANGLE_NUM; 
    
	int edgeCount;
	offFileStream>>edgeCount;
	
	int i;

	float X,Y,Z;
	for(i=0;i<MODEL_VERTEX_NUM;i++) {
		
		
		offFileStream>>X;
		offFileStream>>Y;
		offFileStream>>Z;

		vert.Add(Vector(X,Y,Z));
	}

	int v0, v1, v2, dummy3;
	for(i=0;i<MODEL_TRIANGLE_NUM;i++) {
		tridata td;

		offFileStream>>dummy3;

		offFileStream>>v0;
		offFileStream>>v1;
		offFileStream>>v2;

		td.v[0]=v0;
		td.v[1]=v1;
		td.v[2]=v2;
		tri.Add(td);
	}
	render_num=vert.num;  // by default lets use all the model to render
	initial_num = vert.num;
}



void InitModel(string offFileName) {
	List<int> permutation;
	GetModelData(offFileName);  
	ProgressiveMesh(vert,tri,collapse_map,permutation);
	PermuteVertices(permutation);
	model_position    = Vector(0,0,-3);
	Quaternion yaw(Vector(0,1,0),-3.14f/4);    // 45 degrees
	Quaternion pitch(Vector(1,0,0),3.14f/12);  // 15 degrees 
	model_orientation = pitch*yaw;
}




void RenderModel() {
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor3f(1,1,1);
	glPushMatrix();
	//glTranslatef(model_position.x,model_position.y,model_position.z);
	// Rotate by quaternion: model_orientation
	Vector axis=model_orientation.axis();
	float angle=model_orientation.angle()*180.0f/3.14f;
	//glRotatef(angle,axis.x,axis.y,axis.z);
	DrawModelTriangles();
	//StatusDraw();
	glPopMatrix();

	char buf[256];
	sprintf(buf,"Polys: %d  Vertices: %d ",renderpolycount,render_num);
	if(morph<1.0) {
		sprintf(buf+strlen(buf),"<-> %d  morph: %4.2f ",
		        (int)(lodbase *render_num),morph); 
	}
	//PostString(buf,0,-2,5);
}

