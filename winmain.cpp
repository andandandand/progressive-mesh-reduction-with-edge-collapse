/*	Progressive Mesh Reduction via Edge Collapse
	
	Antonio Rueda Toicen, 2012
	antonio.rueda.toicen@gmail.com
	demo based on paper and code by Stan Melax

	http://dev.gameres.com/Program/Visual/3D/PolygonReduction.pdf
*/

#include <string>
#include <GL/glui.h>

#include <vector>   // std vector
#include "Vector.h" // 3D point with overloaded cross and dot products 
#include <iostream>


/*fps variables*/
int frame=0,time,timebase=0;
double fps;

using namespace std;
extern void       InitModel(string fileName);
extern void       RenderModel();
extern int		  render_num;
extern int		  initial_num;

float xy_aspect;
int last_x, last_y;
float rotationX = 0.0f, rotationY=0.0f;

extern int render_num;

/*live variables passed into GLUI*/
int  current_off;
int wireframe=1;
int   light0_enabled = 1;
int   light1_enabled = 1;
float light0_intensity = 1.0;
float light1_intensity = .4;
int   main_window;
float scale = 1.0;



float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

/** Pointers to the windows and some of the controls we'll create **/
GLUI *glui, *glui2;
GLUI_Spinner    *light0_spinner, *light1_spinner;
GLUI_RadioGroup *radio;
GLUI_Panel      *obj_panel;


/********** User IDs for callbacks ********/
#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  260
#define ENABLE_ID            300
#define DISABLE_ID           301
#define SHOW_ID              302
#define HIDE_ID              303


/********** Miscellaneous global variables **********/

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };


/******************


GLUI CONTROL CALLBACK


*******************/

void control_cb (int control)
{

	if (control == LIGHT0_ENABLED_ID){
	
		if(light0_enabled){
			glEnable(GL_LIGHT0);
			light0_spinner->enable();
		}
	
		else {
			glDisable(GL_LIGHT0);
			light0_spinner->disable();
		
		}
	
	}

	else if( control ==LIGHT1_ENABLED_ID ){
		
		if(  light1_enabled ){
			glEnable(GL_LIGHT1);
			light1_spinner->enable();
		}
		else {
			glDisable(GL_LIGHT1);
			light1_spinner->disable();
			
		}
	}

	else if ( control == LIGHT0_INTENSITY_ID ) {
    float v[] = { 
      light0_diffuse[0],  light0_diffuse[1],
      light0_diffuse[2],  light0_diffuse[3] };
    
    v[0] *= light0_intensity;
    v[1] *= light0_intensity;
    v[2] *= light0_intensity;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, v );
  }
  else if ( control == LIGHT1_INTENSITY_ID ) {
    float v[] = { 
      light1_diffuse[0],  light1_diffuse[1],
      light1_diffuse[2],  light1_diffuse[3] };
    
    v[0] *= light1_intensity;
    v[1] *= light1_intensity;
    v[2] *= light1_intensity;

    glLightfv(GL_LIGHT1, GL_DIFFUSE, v );
  }
  else if ( control == ENABLE_ID )
  {
    glui2->enable();
  }
  else if ( control == DISABLE_ID )
  {
    glui2->disable();
  }
  else if ( control == SHOW_ID )
  {
    glui2->show();
  }
  else if ( control == HIDE_ID )
  {
    glui2->hide();
  }



}



void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;
  case '-':
	  if (render_num>0){
		render_num--;
	  }
	  break;
  case '+':
	  if(render_num<initial_num){
		render_num++;
	  }
	  break;
  };
  
  glutPostRedisplay();
}

/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
  myGlutKeyboard( value, 0, 0 );
}

/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  


  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y ) // adaptar para el drag con quaternion 
{
}

/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
  glutPostRedisplay(); 
}


/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  glViewport( tx, ty, tw, th );

  xy_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}

void myGlutDisplay( void )
{
 
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // paint window black
 
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xy_aspect*.04, xy_aspect*.04, -.04, .04, .1, 15.0 );

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  glMultMatrixf( lights_rotation );
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -2.6f );
  glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] ); 
  glMultMatrixf( view_rotate );

  glScalef( scale, scale, scale );
			
  RenderModel();
	
  glEnable( GL_LIGHTING );


  char window_title[60]; 
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	
	if ((time - timebase) > 1000) {
		fps = (frame*1000.0/(time-timebase));
		// printf("fps: %4.2f \n", fps);
	  
		sprintf(window_title,  "Progressive Mesh Reduction - FPS: %f", fps);

		glutSetWindowTitle(window_title);
		 
		 
		timebase = time;		
		frame = 0;
	}

  glutSwapBuffers(); 
}


int main(int argc, char* argv[]){

	int selection;	
	for (;;) {
	  cout<<"Please indicate the .off's model index to load"<<endl;
  
	  cout<<"1- rabbit"<<endl;
	  cout<<"2- cube"<<endl;
	  cout<<"3- eagle"<<endl;
	  cout<<"4- jacky"<<endl; 
	  cout<<"5- monster"<<endl; 
	  cout<<"6- nemesis"<<endl;
	  cout<<"7- parakeet"<<endl;
	  cout<<"8- sphere"<<endl;
	  cout<<"9- tiger"<<endl;
	  cout<<"10- turtle"<<endl;
	  cout<<"11- woman"<<endl;
  
	  cin>>selection;
  
	  if(selection>0 && selection<12)
		break;

	}
  
	string selection_array[] = {"", "rabbit.off", 
									"cube.off",
									"eagle.off", 
									"jacky.off",
									"monster.off",
									"nemesis.off",
									"parakeet.off", 
									"sphere.off",
									"tiger.off",
									"turtle.off",
									"woman.off"
								};

	 InitModel(selection_array[selection]);
	 
	

	 glutInit(&argc, argv);
     glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
     glutInitWindowPosition( 150, 150 );
     glutInitWindowSize( 900, 700 );

	 main_window = glutCreateWindow( "HW 1 CG III - Antonio Rueda" );
     glutDisplayFunc( myGlutDisplay );

	 GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
     GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
     GLUI_Master.set_glutSpecialFunc( NULL );
     GLUI_Master.set_glutMouseFunc( myGlutMouse );
     
	 glutMotionFunc( myGlutMotion );


	 /****************************************/
     /*       Set up OpenGL lights           */
    /****************************************/

	 glEnable(GL_LIGHTING);
	 glEnable( GL_NORMALIZE );

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*          Enable z-buferring          */
  /****************************************/

  glEnable(GL_DEPTH_TEST);
  
   /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );

  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_RIGHT );

  obj_panel = new GLUI_Rollout(glui, "Properties", true );

  /***** Control for object params *****/

  new GLUI_Checkbox( obj_panel, "Wireframe", &wireframe, 1, control_cb );
  GLUI_Spinner *spinner = 
    new GLUI_Spinner( obj_panel, "Vertices:", &render_num);
     spinner->set_int_limits(0, initial_num );
  spinner->set_alignment( GLUI_ALIGN_RIGHT );

  GLUI_Spinner *scale_spinner = 
    new GLUI_Spinner( obj_panel, "Scale:", &scale);
  scale_spinner->set_float_limits( .2f, 4.0 );
  scale_spinner->set_alignment( GLUI_ALIGN_RIGHT );


  /******** Add some controls for lights ********/

  GLUI_Rollout *roll_lights = new GLUI_Rollout(glui, "Lights", true );

  GLUI_Panel *light0 = new GLUI_Panel( roll_lights, "Light 1" );
  GLUI_Panel *light1 = new GLUI_Panel( roll_lights, "Light 2" );

  new GLUI_Checkbox( light0, "Enabled", &light0_enabled,
                     LIGHT0_ENABLED_ID, control_cb );
  light0_spinner = 
    new GLUI_Spinner( light0, "Intensity:", 
                      &light0_intensity, LIGHT0_INTENSITY_ID,
                      control_cb );
  light0_spinner->set_float_limits( 0.0, 1.0 );
  GLUI_Scrollbar *sb;
  sb = new GLUI_Scrollbar( light0, "Red",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[0],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light0, "Green",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[1],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light0, "Blue",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[2],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  new GLUI_Checkbox( light1, "Enabled", &light1_enabled,
                     LIGHT1_ENABLED_ID, control_cb );
  light1_spinner = 
    new GLUI_Spinner( light1, "Intensity:",
                      &light1_intensity, LIGHT1_INTENSITY_ID,
                      control_cb );
  light1_spinner->set_float_limits( 0.0, 1.0 );
  sb = new GLUI_Scrollbar( light1, "Red",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[0],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light1, "Green",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[1],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light1, "Blue",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[2],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);


  new GLUI_StaticText( glui, "" );


  //new GLUI_Button( glui, "Show Move Controls", SHOW_ID, control_cb );

  new GLUI_StaticText( glui, "" ); // blank UI row 

  /****** A 'quit' button *****/
  new GLUI_Button( glui, "            Quit            ", 0,(GLUI_Update_CB)exit );


  /**** Link windows to GLUI, and register idle callback ******/
  
  glui->set_main_gfx_window( main_window );


  /*** Create the bottom subwindow ***/
  glui2 = GLUI_Master.create_glui_subwindow( main_window, 
                                             GLUI_SUBWINDOW_BOTTOM );
  glui2->set_main_gfx_window( main_window );


  GLUI_Rotation *view_rot = new GLUI_Rotation(glui2, "Object", view_rotate );
  view_rot->set_spin( 1.0 );
  new GLUI_Column( glui2, false );
 
  new GLUI_Column( glui2, false );
  GLUI_Rotation *lights_rot = new GLUI_Rotation(glui2, "Light 1", lights_rotation );
  lights_rot->set_spin( .82 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_xy = 
    new GLUI_Translation(glui2, "Object XY", GLUI_TRANSLATION_XY, obj_pos );
  trans_xy->set_speed( .005 );
  new GLUI_Column( glui2, false );
 
  GLUI_Translation *trans_z = 
    new GLUI_Translation( glui2, "Object Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  trans_z->set_speed( .005 );

#if 0
  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
#endif










  glutMainLoop();

	return 0;
}

