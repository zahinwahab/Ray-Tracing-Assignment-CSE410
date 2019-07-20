#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>
#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define INF 99999
#define eps 1e-10
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
//for ray tracing assignment
int recursion;
int number_of_pixels;
int total_objects;
int total_light_sources;
ofstream outFile;
struct Point
{
    double x,y,z;
    Point()
    {
        x=0;
        y=0;
        z=0;
    }
    Point(double x1,double y1,double z1)
    {
        x=x1;
        y=y1;
        z=z1;
    }
    void printPoint()
    {
        cout << "X: " << x << " Y: " << y << " Z: " << z<<endl;
    }
    void normalize()
    {
        double d = sqrt(x*x+y*y+z*z);
        x/=d;
        y/=d;
        z/=d;
    }
    double getAbsoluteVal()
    {
        double d = sqrt(x*x+y*y+z*z);
        return d;
    }
};
struct Color
{
    double R,G,B;
    Color()
    {
        R=0;
        G=0;
        B=0;
    }
    Color(double x,double y,double z)
    {
        R=x;
        G=y;
        B=z;
    }
    void printColor()
    {
        cout << "R: " << R << " G: " << G << " B: " << B<<endl;
    }

};
Point subtractPoints(Point a,Point b)
{
    Point result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;

}
double dotProduct(Point p,Point q)
{
    double cp = p.x*q.x + p.y*q.y+p.z*q.z;
    return cp;
}
Point crossProduct(Point p,Point q)
{
    Point cp;
    cp.x = p.y*q.z - p.z*q.y;
    cp.y = p.z*q.x - p.x*q.z;
    cp.z = p.x*q.y - p.y*q.x;
    return cp;
}
Point scalePoint(Point p,double q)
{
    /*************
    ******************/
    Point cp;
    cp.x = p.x*q;
    cp.y = p.y*q;
    cp.z = p.z*q;

    return cp;
}
Point addPoints(Point p,Point q)
{
    /*************
    ******************/
    Point cp;
    cp.x = p.x+q.x;
    cp.y = p.y+q.y;
    cp.z = p.z+q.z;

    return cp;
}
struct Ray
{
    Point p;
    Point v;
    Ray() {}
    Ray(Point p1,Point p2)
    {
        p.x = p1.x;
        p.y = p1.y;
        p.z = p1.z;

        v = subtractPoints(p2,p1);
        v.normalize();

    }
    void printRay()
    {
        cout << "Direction: ";
        v.printPoint();
        cout << "Starting point: ";
        p.printPoint();
    }


};
struct Triangle
{
    Point a,b,c;
    Point Center;
    Color color;
    double ambient,diffuse,specular,reflection,specular_exponent;
    Point normal;
    Triangle()
    {

    }
    Triangle(Point a1,Point b1,Point c1,Color color1)
    {
        a.x = a1.x;
        a.y = a1.y;
        a.z = a1.z;

        b.x = b1.x;
        b.y = b1.y;
        b.z = b1.z;

        c.x = c1.x;
        c.y = c1.y;
        c.z = c1.z;

        color.R = color1.R;
        color.G = color1.G;
        color.B = color1.B;


    }
    /*****Barycentric Point p has to be on the same plane as the triangle****/
    bool isOnTriangle(Point p)
    {
        outFile << "inside isOnTriangle x: " << p.x << "y: " << p.y << "z: " << p.z << endl;
        Point v0 = subtractPoints(b,a);
        Point v1 = subtractPoints(c,a);
        Point v2 = subtractPoints(p,a);
        float d00 = dotProduct(v0, v0);
        float d01 = dotProduct(v0, v1);
        float d11 = dotProduct(v1, v1);
        float d20 = dotProduct(v2, v0);
        float d21 = dotProduct(v2, v1);
        float denom = d00 * d11 - d01 * d01;
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;
        outFile << "u: " << u << "v: " << v << "w: " << w;
        // Check if point is in triangle
        return (u >= 0) && (v >= 0) && (w >=0);


    }
    void setNormal()
    {
        normal = crossProduct(subtractPoints(a,c),subtractPoints(b,c));
        normal.normalize();

    }
    double rayTriangleIntersection(Ray ray)
    {
        Point e1 = subtractPoints(a,b);
        Point e2 = subtractPoints(a,c);
        Point e3 = subtractPoints(a,ray.p);
        Point q = crossProduct(e2,ray.v);
        double det = dotProduct(e1,q);//det is the determinant of  matrix
        outFile << "det: " << det << endl;

        Point n = crossProduct(subtractPoints(a,c),subtractPoints(b,c));
        n.normalize();
        // Backfacing or nearly parallel?
        if ((dotProduct(n,ray.v) >= 0) || (abs(det) <= eps))   /// dotprod <=0 normal ar vector er angle >= 90
        {

            return -INF;
        }

        Point e2_cross_e3 = crossProduct(e2,e3);
        double detNum = dotProduct(e1,e2_cross_e3);
        double t = detNum/det;
        outFile << "t: " << t << endl;
        if(isOnTriangle(Point(ray.p.x+t*ray.v.x,ray.p.y+t*ray.v.y,ray.p.z+t*ray.v.z)))
        {
            setNormal();
            return t;
        }

        else return -INF;
    }
    void draw()
    {
        glColor3f(color.R,color.G,color.B);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x,a.y,a.z);
            glVertex3f(b.x,b.y,b.z);
            glVertex3f(c.x,c.y,c.z);
        }
        glEnd();

    }

};
struct Square
{
    Point a,b,c,d;
    Color color;
    double ambient,diffuse,specular,reflection,specular_exponent;
    Point normal;

    Square() {}
    Square(Point lowestxyz,double length,Color color1)
    {
        /**
        |d c|
        |a b|
        **/
        a = Point(lowestxyz.x,lowestxyz.y,lowestxyz.z);
        b = Point(lowestxyz.x+length,lowestxyz.y,lowestxyz.z);
        c = Point(lowestxyz.x+length,lowestxyz.y+length,lowestxyz.z);
        d = Point(lowestxyz.x,lowestxyz.y+length,lowestxyz.z);

        color.R = color1.R;
        color.G = color1.G;
        color.B = color1.B;

    }
    Square(Point a1,Point b1,Point c1,Point d1,Color color1)
    {
        a.x = a1.x;
        a.y = a1.y;
        a.z = a1.z;

        b.x = b1.x;
        b.y = b1.y;
        b.z = b1.z;

        c.x = c1.x;
        c.y = c1.y;
        c.z = c1.z;

        d.x = d1.x;
        d.y = d1.y;
        d.z = d1.z;

        color.R = color1.R;
        color.G = color1.G;
        color.B = color1.B;


    }
    double raySquareIntersection(Ray ray)
    {
        Triangle t1 = Triangle(a,b,c,color);
        Triangle t2 = Triangle(c,d,a,color);
        double t11 = t1.rayTriangleIntersection(ray);
        double t22 = t2.rayTriangleIntersection(ray);
        if(t11>=0 && t22>=0)
        {
            if(t11<t22)
            {
                t1.setNormal();
                normal.x = t1.normal.x;
                normal.y = t1.normal.y;
                normal.z = t1.normal.z;
                return t11;
            }
            else
            {

                t2.setNormal();
                normal.x = t2.normal.x;
                normal.y = t2.normal.y;
                normal.z = t2.normal.z;

                return t22;
            }
        }
    }
    void drawSquare()
    {
        glColor3f(color.R,color.G,color.B);
        glBegin(GL_QUADS);
        {
            glVertex3f( a.x, a.y,a.z);
            glVertex3f( b.x, b.y,b.z);
            glVertex3f( c.x, c.y,c.z);
            glVertex3f( d.x, d.y,d.z);
        }
        glEnd();
    }

};
struct Sphere
{
    double radius;
    Point center;
    Color color;
    double ambient,diffuse,specular,reflection,specular_exponent;
    Point normal;
    int id;
    string type;
    Sphere() {}
    Sphere(double r,Point cen,Color col)//,double a,double d,double sp,double re,double sh)
    {
        type = "sphere";
        radius = r;
        center.x = cen.x;
        center.y = cen.y;
        center.z = cen.z;
        color.R = col.R;
        color.G = col.G;
        color.B = col.B;
//        ambient = a;
//        diffuse = d;
//        specular = sp;
//        reflection = re;
//        shininess = sh;


    }
    void setNormal(Ray ray,double t)
    {
        normal = subtractPoints(Point(ray.p.x+t*ray.v.x,ray.p.y+t*ray.v.y,ray.p.z+t*ray.v.z),center);//vector joiningthe point at which the ray intersects
        //the sphere and center forms 90 degree to the surface
        normal.normalize();
    }
    double raySphereIntersection(Ray ray)
    {
        // Quadratic: at2 + bt + c = 0
        Point R0 = subtractPoints(ray.p,center);
        double a = 1; //as Rd's abs val is v (remember, ||Rd|| = 1)
        double b = 2*dotProduct(ray.v,R0);
        double c = dotProduct(R0,R0) - radius*radius;
        double det = b*b - 4*a*c;
        if(det<0) return -INF;
        else if(det==0)
        {
            double t = -b/(2*a);
            if(t<0) return -INF;
            else return t;
        }
        else if(det>0)
        {
            double t1=(-b+sqrt(det))/(2*a);
            double t2=(-b-sqrt(det))/(2*a);
            if(t1<0&&t2<0) return -INF;
            else if(t1<0&&t2>0)
            {
                setNormal(ray,t2);
                return t2;
            }
            else if(t1>0&&t2<0)
            {
                setNormal(ray,t1);
                return t1;
            }
            else return min(t1,t2);
        }




    }



    void draw()
    {
        int slices = 50;
        int stacks = 50;
        struct Point points[100][100];
        int i,j;
        double h,r;
        //generate points
        for(i=0; i<=stacks; i++)
        {
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0; j<=slices; j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0; i<stacks; i++)
        {
            //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,0.9);
            glColor3f(color.R,color.G,color.B);
            for(j=0; j<slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);

                }
                glEnd();
            }
        }
    }
    void drawSphere()
    {
        glPushMatrix();
        {
            glTranslatef(center.x,center.y,center.z);

            draw();
        }
        glPopMatrix();
    }
};


struct Pyramid
{
    Square base;
    Triangle sides[4];

    Color color;
    string type;
    int id;
    double ambient,diffuse,specular,reflection,specular_exponent;
    Point normal;
    Pyramid() {}
    Pyramid(Point lowestxyz,double length,double height,Color col)
    {
        type = "pyramid";
        base = Square(lowestxyz,length,col);
        Point peak =Point((base.a.x+base.c.x)/2,(base.a.y+base.c.y)/2,height);
        sides[0] = Triangle(peak,base.a,base.b,col);
        sides[1] = Triangle(peak,base.b,base.c,col);
        sides[2] = Triangle(peak,base.c,base.d,col);
        sides[3] = Triangle(peak,base.d,base.a,col);
        color.R = col.R;
        color.G = col.G;
        color.B = col.B;

    }
    double rayPyramidIntersection(Ray ray)
    {
        double t = INF;
        double t1 = base.raySquareIntersection(ray);

        if(t1>=0)
        {
            normal.x = base.normal.x;
            normal.y = base.normal.y;
            normal.z = base.normal.z;
            t =t1;

        }
        for(int i=0; i<4; i++)
        {
            double t1 = sides[i].rayTriangleIntersection(ray);
            if(t1>=0 && t1<t)
            {
                t = t1;
                sides[i].setNormal();
                normal.x = sides[i].normal.x;
                normal.y = sides[i].normal.y;
                normal.z = sides[i].normal.z;
            }
        }
        if(t>INF-(eps+eps))    return -INF;
        return t;
    }
    void drawPyramid()
    {
        for(int i=0; i<4; i++) sides[i].draw();
        base.drawSquare();
    }

};

struct CheckerBoard
{
    double width;
    double ambient,diffuse,specular,reflection,specular_exponent;
    const static int no_of_tiles = 200;
    double color_arr[no_of_tiles][no_of_tiles];
    Point point_arr[no_of_tiles][no_of_tiles];
    int id;
    string type;
    CheckerBoard() {}
    CheckerBoard(double w)
    {
        type = "checkerboard";
        width = w;

    }
    double whichTile(Point p)
    {
        int i,j;
        int flag = false;
        for( i=0; i<no_of_tiles; i++)
        {
            for( j=0; j<no_of_tiles; j++)
            {
                Point q = point_arr[i][j];
                //q.printPoint();
                if(p.x>=q.x&&p.x<(q.x+width)&&p.y>=q.y&&p.y<(q.y+width))
                {
                    flag = true;

                    break;
                }


            }
            if(flag == true ) break;
        }
        return color_arr[i][j];
    }
    void drawCheckerBoard()
    {
        double c = 1.0;

        for(int i=-3000; i<3000; i+=width)
        {
            for(int j=-3000; j<3000; j+=width)
            {
                glColor3f(c,c,c);

                color_arr[int((i+3000)/width)][int((j+3000)/width)] = c;
                point_arr[int((i+3000)/width)][int((j+3000)/width)] = Point(i,j,0);
                glBegin(GL_QUADS);
                {
                    glVertex3f(i,j,0);
                    glVertex3f(i+width,j,0);
                    glVertex3f(i+width,j+width,0);
                    glVertex3f(i,j+width,0);
                }
                glEnd();
                c = 1.0-c;
            }
            c = 1.0-c;
        }

    }
    Point getNormal()
    {
        return Point(0,0,1);
    }

    double rayCheckerBoardIntersection(Ray ray)
    {
        if(abs(ray.v.z)<eps)  return -INF;
        double t = -ray.p.z/ray.v.z;
        Point p = Point(ray.p.x+t*ray.v.x,ray.p.y+t*ray.v.y,ray.p.z+t*ray.v.z);
        if(abs(p.z)>eps)    return -INF;
        if(p.x<-3000||p.x>3000 || p.y <-3000 || p.y > 3000) return -INF;
        return t;


    }





};

vector<Sphere>sphere_arr;
vector<Pyramid>pyramid_arr;
vector<Point>lightSource_arr;
CheckerBoard checkerBoard;

Point position;
Point upVector;
Point rightVector;
Point lookVector;

void drawAxes()
{
    if(drawaxes==1)
    {
        //glColor3f(1.0, 1.0, 1.0);
        //RGB
        glBegin(GL_LINES);
        {
            //X-AXIS is RED
            glColor3f(1.0, 0, 0);
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);
            //Y-AXIS IS GREEN
            glColor3f(0, 1.0,0);
            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);
            //Z-AXIS IS BLUE
            glColor3f(0, 0, 1.0);
            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}
Color pixelMap[1000][1000];

void getImage()
{
    bitmap_image image(number_of_pixels,number_of_pixels);
    for(int i=0; i<number_of_pixels; i++)
    {
        for(int j=0; j<number_of_pixels; j++)
        {
            //  Color &c = pixelMap[i][j];
            //  image.set_pixel(j,number_of_pixels-i-1,c.R*255,c.G*255,c.B*255);  // Setting the color of the pixels
        }
    }
    image.save_image("out.bmp");
}

Color lightSouce;//(1,1,1);
Color computeColor(Ray source,string type,int id)
{
    // cout << "Source: ";
    //source.printRay();
    double t_sphere = INF,t_pyramid = INF,t_checkerBoard;
    Point sphereNormal, pyramidNormal, checkerBoardNormal;
    int sphereIndex = -1, pyramidIndex = -1;

    for(int i=0; i<sphere_arr.size(); i++)
    {
        double temp = sphere_arr[i].raySphereIntersection(source);
        // cout << "Intersection for sphere: " << temp << endl;
        if(temp!=-INF && temp<t_sphere)
        {
            t_sphere =temp;
            sphereNormal.x = sphere_arr[i].normal.x;
            sphereNormal.y = sphere_arr[i].normal.y;
            sphereNormal.z = sphere_arr[i].normal.z;
            sphereIndex = i;

        }

    }
   // if(t_sphere>=-eps && t_sphere<=eps)sphereIndex= -1;
    //  if(t_sphere>INF-eps)    sphereIndex= -1;
     for(int i=0; i<pyramid_arr.size(); i++)
    {
        //   cout << i;
        double temp = pyramid_arr[i].rayPyramidIntersection(source);
        // cout << "Intersection for pyramid: " << temp << endl;
        if(temp!=-INF && temp>=0 && temp<t_pyramid)
        {
            t_pyramid =temp;
            pyramidNormal.x = pyramid_arr[i].normal.x;
            pyramidNormal.y = pyramid_arr[i].normal.y;
            pyramidNormal.z = pyramid_arr[i].normal.z;
            pyramidIndex = i;
            //    pyramid_arr[i].color.printColor();

        }

    }
    // if(t_pyramid>INF-eps)    pyramidIndex= -1;
  //  if(t_pyramid>=-eps && t_pyramid<=eps)pyramidIndex= -1;
  //  if(t_pyramid<t_sphere ) cout << "pyramid kacche \n";
    t_checkerBoard = checkerBoard.rayCheckerBoardIntersection(source);

    Point finalNormal;
    double final_t;
    //  cout << "Sphere index: " << sphereIndex << "Pyramid index: " << pyramidIndex << endl;

    if(sphereIndex == -1 && pyramidIndex == -1 && (t_checkerBoard == -INF|| t_checkerBoard > INF-eps))
    {
        return Color(0,0,0);
    }
    else if(sphereIndex == -1 && pyramidIndex == -1 && t_checkerBoard>=0)
    {
        Point p = Point(source.p.x+t_checkerBoard*source.v.x,source.p.y+t_checkerBoard*source.v.y,source.p.z+t_checkerBoard*source.v.z);
        double c = checkerBoard.whichTile(p);

        return Color(c,c,c);
    }

    else if(pyramidIndex == -1 || t_sphere < t_pyramid)
    {
        if(t_checkerBoard>=0  && t_checkerBoard<t_sphere)
        {
            Point p = Point(source.p.x+t_checkerBoard*source.v.x,source.p.y+t_checkerBoard*source.v.y,source.p.z+t_checkerBoard*source.v.z);
            double c = checkerBoard.whichTile(p);
            //  if(c==1) cout << "white is returned ";
            return Color(c,c,c);

        }
        else
        {
            return sphere_arr[sphereIndex].color;
        }

    }
    else if(sphereIndex == -1 || t_pyramid < t_sphere)
    {
        if(t_checkerBoard>=0  &&t_checkerBoard<t_pyramid)
        {
            cout << "Mofo\n";
            Point p = Point(source.p.x+t_checkerBoard*source.v.x,source.p.y+t_checkerBoard*source.v.y,source.p.z+t_checkerBoard*source.v.z);
            double c = checkerBoard.whichTile(p);

            return Color(c,c,c);

        }
        else
        {
            // cout << "hits the pyramid\n";
            // cout << pyramidIndex;
            //  pyramid_arr[pyramidIndex].color.printColor();
            return pyramid_arr[pyramidIndex].color;
        }

    }

}

void generateImage()
{
    Ray src;
    src.p.x = position.x;
    src.p.y = position.y;
    src.p.z = position.z;
    //  sourcePower = Color(1,1,1);
    number_of_pixels = 400;
    int   screenHeight = number_of_pixels;
    int   screenWidth = number_of_pixels;
    bitmap_image image(number_of_pixels,number_of_pixels);
    for(int row = 0; row<screenHeight; row++)
    {
        for(int colm = 0; colm<screenWidth; colm++)
        {
            double ht = row-screenHeight/2;
            ht/=screenHeight/2;
            double wd = colm-screenWidth/2;
            wd/=screenWidth/2;
            Point dir = addPoints(scalePoint(upVector,ht), scalePoint(rightVector,wd));
            dir = addPoints(lookVector,dir);
            src.v.x = dir.x;
            src.v.y = dir.y;
            src.v.z = dir.z;
            src.v.normalize();
            Color c = computeColor(src,"eye", 0);
            image.set_pixel(colm,number_of_pixels-1-row,c.R*255,c.G*255,c.B*255);

            //  imageMap[row][colm] = rayCast(src,recursionLevel, ObjectID(EYE,0));

        }
    }
    cout<<"Done generating the image: "<<endl;
//     number_of_pixels = 786;
//     bitmap_image image(number_of_pixels,number_of_pixels);
//
//          //  Color &c = pixelMap[i][j];
//          //  image.set_pixel(j,number_of_pixels-i-1,c.R*255,c.G*255,c.B*255);  // Setting the color of the pixels
//
//
//    Ray source;
//    source.p.x = position.x;
//    source.p.y = position.y;
//    source.p.z = position.z;
//
//    int half = number_of_pixels/2;
//    for(int i=-number_of_pixels/2+1;i<number_of_pixels/2;i++){
//        for(int j=-number_of_pixels/2 + 1;j<number_of_pixels/2;j++){
//            outFile << "i: "<< i << " " << "j: " << j << endl;
//            double ht = i;//-number_of_pixels/2;
//            ht/=number_of_pixels/2;
//            double wd = j;//-number_of_pixels/2;
//            wd/=number_of_pixels/2;
//            Point dir = addPoints(scalePoint(upVector,ht) ,scalePoint(rightVector,wd));
//            dir = addPoints(lookVector,dir);
//            dir.normalize();
//            source.v.x = dir.x;
//            source.v.y = dir.y;
//            source.v.z = dir.z;
//
//          //  cout << "direction: " << source.v.x <<" " <<source.v.y << " "<<source.v.z <<endl;
//
//            Color c = computeColor(source,"eye", 0);
//            //cout << "i: " << i << " j: " << j << endl;
//           // cout << c.R << " "<< c.G << " "<< c.B << endl;
//           image.set_pixel((i+half)/half,((j+half)/half),c.R*255,c.G*255,c.B*255);
//
//        }
//
//    }
//    cout << "Done printing the image\n";
    image.save_image("out.bmp");
//


}
void keyboardListener(unsigned char key, int x,int y)
{
    double change = 0.03;
    switch(key)
    {
//So, u = r X l //rotate l wrt r
//Then we scale l by cosA and scale u by sinA and take their sum
//u.r = r.l = l.u = 0, u =  r X l, l =  u X r, and r =  l X
    case '1':
    {
        //w.r.t up vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        rightVector.x = prevRight.x*cos(change) + prevLook.x*sin(change);
        rightVector.y = prevRight.y*cos(change) + prevLook.y*sin(change);
        rightVector.z = prevRight.z*cos(change) + prevLook.z*sin(change);

        lookVector = crossProduct(upVector,rightVector);
    }
    break;
    case '2':
    {
        //w.r.t up vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        rightVector.x = prevRight.x*cos(-change) + prevLook.x*sin(-change);
        rightVector.y = prevRight.y*cos(-change) + prevLook.y*sin(-change);
        rightVector.z = prevRight.z*cos(-change) + prevLook.z*sin(-change);

        lookVector = crossProduct(upVector,rightVector);
    }
    break;
    case '3':
    {
        //w.r.t right vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        lookVector.x = prevLook.x*cos(change) + prevUp.x*sin(change);
        lookVector.y = prevLook.y*cos(change) + prevUp.y*sin(change);
        lookVector.z = prevLook.z*cos(change) + prevUp.z*sin(change);

        upVector = crossProduct(rightVector,lookVector);
    }
    break;
    case '4':
    {
        //w.r.t right vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        lookVector.x = prevLook.x*cos(-change) + prevUp.x*sin(-change);
        lookVector.y = prevLook.y*cos(-change) + prevUp.y*sin(-change);
        lookVector.z = prevLook.z*cos(-change) + prevUp.z*sin(-change);

        upVector = crossProduct(rightVector,lookVector);
    }
    break;
    case '5':
    {
        //w.r.t look vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        upVector.x = prevUp.x*cos(change) + prevRight.x*sin(change);
        upVector.y = prevUp.y*cos(change) + prevRight.y*sin(change);
        upVector.z = prevUp.z*cos(change) + prevRight.z*sin(change);

        rightVector = crossProduct(lookVector,upVector);
    }
    break;
    case '6':
    {
        //w.r.t look vector
        Point prevLook = lookVector;
        Point prevUp = upVector;
        Point prevRight = rightVector;
        upVector.x = prevUp.x*cos(-change) + prevRight.x*sin(-change);
        upVector.y = prevUp.y*cos(-change) + prevRight.y*sin(-change);
        upVector.z = prevUp.z*cos(-change) + prevRight.z*sin(-change);

        rightVector = crossProduct(lookVector,upVector);
    }
    break;
    case '0':
    {
        generateImage();
        getImage();

    }

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    double change = 2.0;
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        //cameraHeight -= 3.0;
        position.x -= change*lookVector.x;
        position.y -= change*lookVector.y;
        position.z -= change*lookVector.z;
        break;
    case GLUT_KEY_UP:		// up arrow key
        position.x += change*lookVector.x;
        position.y += change*lookVector.y;
        position.z += change*lookVector.z;
        //cameraHeight += 3.0;
        break;

    case GLUT_KEY_RIGHT:
        position.x += change*rightVector.x;
        position.y += change*rightVector.y;
        position.z += change*rightVector.z;
        //cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        position.x -= change*rightVector.x;
        position.y -= change*rightVector.y;
        position.z -= change*rightVector.z;
        //cameraAngle -= 0.03;
        break;

    case GLUT_KEY_PAGE_UP:
        position.x += change*upVector.x;
        position.y += change*upVector.y;
        position.z += change*upVector.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        position.x -= change*upVector.x;
        position.y -= change*upVector.y;
        position.z -= change*upVector.z;
        break;

    case GLUT_KEY_INSERT:
        break;



    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}

void drawObjects()
{
    for(int i=0; i<sphere_arr.size(); i++) sphere_arr[i].drawSphere();
    for(int i=0; i<pyramid_arr.size(); i++) pyramid_arr[i].drawPyramid();
    checkerBoard.drawCheckerBoard();
    for(int i=0; i<lightSource_arr.size(); i++)
    {
        Sphere sp = Sphere(2,Point(lightSource_arr[i].x,lightSource_arr[i].y,lightSource_arr[i].z),Color(1,1,1));

        sp.drawSphere();


    }

}



void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    //gluLookAt(0,0,200,	0,0,0,	0,1,0);
    gluLookAt(position.x, position.y, position.z, position.x + 10*lookVector.x, position.y + 10*lookVector.y,
              position.z + 10*lookVector.z,upVector.x, upVector.y, upVector.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objectsition

    drawAxes();
    drawGrid();
    drawObjects();
//    CheckerBoard checkerBoard = CheckerBoard(50);
//    checkerBoard.drawCheckerBoard();
//    Pyramid pyramid = Pyramid(Point(0,0,0),30,60,Color(1,0,0));
//    pyramid.drawPyramid();




    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    //angle+=0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void readFile()
{
    ifstream inFile;
    inFile.open("description.txt");
    if (!inFile)
    {
        cerr << "Unable to open file description.txt";
        exit(1);
    }
    inFile >> recursion;
    inFile >> number_of_pixels;
    checkerBoard.width = 30;

    inFile >> total_objects;
    string command;
    int sphereCount = 0, pyramidCount = 0;
    for(int i=0; i<total_objects; i++)
    {
        inFile >> command;
        if(command=="sphere")
        {
            Sphere sphere;
            sphere.id = sphereCount;
            sphereCount++;
            //
            //sphere.id = i+1;
            inFile >> sphere.center.x >> sphere.center.y >> sphere.center.z;
            inFile >> sphere.radius;
            inFile >> sphere.color.R >> sphere.color.G >> sphere.color.B;
            //0.4 0.2 0.2 0.2 ambient diffuse specular reflection coefficient 5 specular exponent
            inFile >> sphere.ambient >> sphere.diffuse >> sphere.specular >> sphere.reflection;
            inFile >> sphere.specular_exponent;
            sphere_arr.push_back(sphere);

        }
        else if(command=="pyramid")
        {

//                0.0 0.0 0.0 lowest x y z ordinate
//30.0 60 length of base and height of pyramid

            Point lowestxyz;
            inFile >> lowestxyz.x >> lowestxyz.y >> lowestxyz.z;
            double length,height;
            inFile >> length >> height;
            double R,G,B;
            inFile >> R >> G >> B;
            Color col = Color(R,G,B);

            Pyramid pyramid = Pyramid(lowestxyz, length, height, col);
            // pyramid.id = i+1;
            pyramid.id = pyramidCount;
            pyramidCount++;


            //0.4 0.2 0.2 0.2 ambient diffuse specular reflection coefficient 5 specular exponent
            inFile >> pyramid.ambient >> pyramid.diffuse >> pyramid.specular >> pyramid.reflection;
            inFile >> pyramid.specular_exponent;

            pyramid_arr.push_back(pyramid);

        }



    }
    checkerBoard.id = 0;
    inFile >> total_light_sources;
    for(int i=0; i<total_light_sources; i++)
    {
        Point pointSource;
        inFile >> pointSource.x >> pointSource.y >> pointSource.z;
        lightSource_arr.push_back(pointSource);
    }


}
void test()
{

    outFile.open("debug.txt");
    generateImage();
//    Sphere sphere = Sphere(1,Point(5,0,0),Color(1,0,0));
////    Triangle t;
////    t.a = Point(0,0,0);
////    t.b = Point(0,5,0);
////    t.c = Point(5,0,0);
////
//    Point p = Point(0,0,5);
//    Point q = Point(5,0,1);
//
//    Ray ray = Ray(p,q);
//    outFile << "ray.v: " << ray.v.x << " " <<ray.v.y << " "<< ray.v.z << endl;
//    outFile << "t:" << sphere.raySphereIntersection(ray);
    outFile.close();

}

void init()
{
    readFile();

    //codes for initialization
    drawgrid=0;
    drawaxes=1;
    cameraHeight=150.0;
    cameraAngle=1.0;
    upVector = Point(0,0,1);
    lightSouce = Color(1,1,1);
    rightVector = Point(-1.0/sqrt(2), 1.0/sqrt(2), 0);
    lookVector = Point(-1.0/sqrt(2), -1.0/sqrt(2), 0);

    position = Point(100,100,50);
    //  u = (0, 0, 1), r = (-1/√2, 1/√2, 0),  l = (-1/√2, -1/√2, 0), and pos = (100, 100, 0

    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
    //test();
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}
