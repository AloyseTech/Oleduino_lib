const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Oleduino.h>

Oleduino console;

uint16_t line_color = 0;

float xx,xy,xz;
float yx,yy,yz;
float zx,zy,zz;

float fact;

int Xan,Yan;

int Xoff;
int Yoff;
int Zoff;

struct Point3d
{
  int x;
  int y;
  int z;
};

struct Point2d
{
  int x;
  int y;
};



int LinestoRender; // lines to render.
int OldLinestoRender; // lines to render just in case it changes. this makes sure the old lines all get erased.



struct Line3d
{
  Point3d p0;
  Point3d p1;
};



struct Line2d
{
  Point2d p0;
  Point2d p1;
};


Line3d Lines[20];
Line2d Render[20];
Line2d ORender[20];






/***********************************************************************************************************************************/
// Sets the global vars for the 3d transform. Any points sent through "process" will be transformed using these figures.
// only needs to be called if Xan or Yan are changed.
void SetVars(void)
{
  float Xan2,Yan2,Zan2;
  float s1,s2,s3,c1,c2,c3;
 
  Xan2 = Xan / fact; // convert degrees to radians.
  Yan2 = Yan / fact;
 
  // Zan is assumed to be zero
   
  s1 = sin(Yan2);
  s2 = sin(Xan2);
 
  c1 = cos(Yan2);
  c2 = cos(Xan2);

  xx = c1;
  xy = 0;
  xz = -s1;

  yx = (s1 * s2);
  yy = c2;
  yz = (c1 * s2);

  zx = (s1 * c2);
  zy = -s2;
  zz = (c1 * c2);
}


/***********************************************************************************************************************************/
// processes x1,y1,z1 and returns rx1,ry1 transformed by the variables set in SetVars()
// fairly heavy on floating point here.
// uses a bunch of global vars. Could be rewritten with a struct but not worth the effort.
void ProcessLine(struct Line2d *ret,struct Line3d vec)
{
  float zvt1;
  int xv1,yv1,zv1;

  float zvt2;
  int xv2,yv2,zv2;
 
  int rx1,ry1;
  int rx2,ry2;
 
  int x1;
  int y1;
  int z1;

  int x2;
  int y2;
  int z2;
 
  int Ok;
 
  x1=vec.p0.x;
  y1=vec.p0.y;
  z1=vec.p0.z;

  x2=vec.p1.x;
  y2=vec.p1.y;
  z2=vec.p1.z;

  Ok=0; // defaults to not OK

  xv1 = (x1 * xx) + (y1 * xy) + (z1 * xz);
  yv1 = (x1 * yx) + (y1 * yy) + (z1 * yz);
  zv1 = (x1 * zx) + (y1 * zy) + (z1 * zz);

  zvt1 = zv1 - Zoff;


  if( zvt1 < -5){
    rx1 = 256 * (xv1 / zvt1) + Xoff;
    ry1 = 256 * (yv1 / zvt1) + Yoff;
    Ok=1; // ok we are alright for point 1.
  }
 
 
  xv2 = (x2 * xx) + (y2 * xy) + (z2 * xz);
  yv2 = (x2 * yx) + (y2 * yy) + (z2 * yz);
  zv2 = (x2 * zx) + (y2 * zy) + (z2 * zz);

  zvt2 = zv2 - Zoff;


  if( zvt2 < -5){
    rx2 = 256 * (xv2 / zvt2) + Xoff;
    ry2 = 256 * (yv2 / zvt2) + Yoff;
  } else
  {
    Ok=0;
  }
 
 
 
 
  if(Ok==1){
   
  ret->p0.x=rx1;
  ret->p0.y=ry1;

  ret->p1.x=rx2;
  ret->p1.y=ry2;
  }
 // The ifs here are checks for out of bounds. needs a bit more code here to "safe" lines that will be way out of whack, so they dont get drawn and cause screen garbage.
 
}



/***********************************************************************************************************************************/
void setup() {
 
 
 
  console.joystick.read();
  console.imu.begin();
  console.display.begin();

  fact = 180 / 3.14159259; // conversion from degrees to radians.

  Xoff = 63; // positions the center of the 3d conversion space into the center of the OLED screen. This is usally screen_x_size / 2.
  Yoff = 63; // screen_y_size /2
  Zoff = 500;


// line segments to draw a cube. basically p0 to p1. p1 to p2. p2 to p3 so on.

// Front Face.

  Lines[0].p0.x=-50;
  Lines[0].p0.y=-50;
  Lines[0].p0.z=50;
  Lines[0].p1.x=50;
  Lines[0].p1.y=-50;
  Lines[0].p1.z=50;



  Lines[1].p0.x=50;
  Lines[1].p0.y=-50;
  Lines[1].p0.z=50;
  Lines[1].p1.x=50;
  Lines[1].p1.y=50;
  Lines[1].p1.z=50;




  Lines[2].p0.x=50;
  Lines[2].p0.y=50;
  Lines[2].p0.z=50;
  Lines[2].p1.x=-50;
  Lines[2].p1.y=50;
  Lines[2].p1.z=50;



  Lines[3].p0.x=-50;
  Lines[3].p0.y=50;
  Lines[3].p0.z=50;
  Lines[3].p1.x=-50;
  Lines[3].p1.y=-50;
  Lines[3].p1.z=50;






//back face.

  Lines[4].p0.x=-50;
  Lines[4].p0.y=-50;
  Lines[4].p0.z=-50;
  Lines[4].p1.x=50;
  Lines[4].p1.y=-50;
  Lines[4].p1.z=-50;



  Lines[5].p0.x=50;
  Lines[5].p0.y=-50;
  Lines[5].p0.z=-50;
  Lines[5].p1.x=50;
  Lines[5].p1.y=50;
  Lines[5].p1.z=-50;




  Lines[6].p0.x=50;
  Lines[6].p0.y=50;
  Lines[6].p0.z=-50;
  Lines[6].p1.x=-50;
  Lines[6].p1.y=50;
  Lines[6].p1.z=-50;



  Lines[7].p0.x=-50;
  Lines[7].p0.y=50;
  Lines[7].p0.z=-50;
  Lines[7].p1.x=-50;
  Lines[7].p1.y=-50;
  Lines[7].p1.z=-50;




// now the 4 edge lines.

  Lines[8].p0.x=-50;
  Lines[8].p0.y=-50;
  Lines[8].p0.z=50;
  Lines[8].p1.x=-50;
  Lines[8].p1.y=-50;
  Lines[8].p1.z=-50;


  Lines[9].p0.x=50;
  Lines[9].p0.y=-50;
  Lines[9].p0.z=50;
  Lines[9].p1.x=50;
  Lines[9].p1.y=-50;
  Lines[9].p1.z=-50;




  Lines[10].p0.x=-50;
  Lines[10].p0.y=50;
  Lines[10].p0.z=50;
  Lines[10].p1.x=-50;
  Lines[10].p1.y=50;
  Lines[10].p1.z=-50;


  Lines[11].p0.x=50;
  Lines[11].p0.y=50;
  Lines[11].p0.z=50;
  Lines[11].p1.x=50;
  Lines[11].p1.y=50;
  Lines[11].p1.z=-50;



  LinestoRender=12;
  OldLinestoRender=LinestoRender;


 
}
/***********************************************************************************************************************************/
void RenderImage( void)
{
  // renders all the lines after erasing the old ones.

// in here is the only code actually interfacing with the OLED. so if you use a different lib, this is where to change it.



 for (int i=0; i<OldLinestoRender; i++ )
  {
    console.display.drawLine(ORender[i].p0.x,ORender[i].p0.y,ORender[i].p1.x,ORender[i].p1.y, 0); // erase the old lines.
  }

   
  for (int i=0; i<LinestoRender; i++ )
  {
    console.display.drawLine(Render[i].p0.x,Render[i].p0.y,Render[i].p1.x,Render[i].p1.y, line_color);
  }
  OldLinestoRender=LinestoRender;
 
}


/***********************************************************************************************************************************/

void loop() {
 
  line_color=console.display.Color565(0,255,0); // colours of all lines drawn will be green until changed.
 
  Xan++;
  Yan++;


  Yan=Yan % 360;
  Xan=Xan % 360; // prevents overflow.
 


  SetVars(); //sets up the global vars to do the conversion.

  for(int i=0; i<LinestoRender ; i++)
  {
    ORender[i]=Render[i]; // stores the old line segment so we can delete it later.
    ProcessLine(&Render[i],Lines[i]); // converts the 3d line segments to 2d.
  } 
 
  RenderImage(); // go draw it!
 
}
/***********************************************************************************************************************************/
