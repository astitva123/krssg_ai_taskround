#include "stdio.h"
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>
//#include "opencv2/contrib/contrib.hpp"

using namespace cv;
using namespace std;

#define area 60

typedef struct 
{
  float x,y;
}reference;

reference refer;

struct star {

    star *parent;
    reference position;
    
};

star start_star;
star end_star;
Mat img;
star* stars[5000];
int totstars = 0;
int reached = 0;
int step_size = 10;
int iter = 0;

void init()
{
    start_star.position.x = 70;
    start_star.position.y = 70;
    start_star.parent = NULL;
    for(int i=start_star.position.x; i < start_star.position.x + 10; i++)
    {
      for(int j=start_star.position.y; j < start_star.position.y + 10; j++)
      {
        img.at<Vec3b>(i, j)[0] = 255;
        img.at<Vec3b>(i, j)[1] = 0;
        img.at<Vec3b>(i, j)[2] = 0;
      }
    }
    stars[totstars++] = &start_star;
    end_star.position.x = 550;
    end_star.position.y = 550;
    for(int i=end_star.position.x - 5; i < end_star.position.x + 5; i++)
    {
      for(int j=end_star.position.y - 5; j < end_star.position.y + 5; j++)
      {
        img.at<Vec3b>(i, j)[0] = 0;
        img.at<Vec3b>(i, j)[1] = 0;
        img.at<Vec3b>(i, j)[2] = 255;
      }
    }
    srand(time(NULL));
}
float star_dist(reference p, reference q)
{
  reference v;
  v.x = p.x - q.x;
  v.y = p.y - q.y;
  return sqrt(pow(v.x, 2) + pow(v.y, 2));
}
int near_star(star rstar)
{
  float min_dist = 999.0, dist= star_dist(start_star.position, rstar.position);
  int lstar = 0, i = 0;
  for(i=0; i<totstars; i++)
  {
    dist = star_dist(stars[i]->position, rstar.position);
    if(dist<min_dist)
    {
      min_dist = dist;
      lstar = i;
    }
  }
  return lstar;
}
reference stepping(reference nstar, reference rstar)
{
  reference interm, step;
  float magn = 0.0, x = 0.0, y = 0.0;
  interm.x = rstar.x - nstar.x;
  interm.y = rstar.y - nstar.y;
  magn = sqrt((interm.x)*(interm.x) + (interm.y)*(interm.y));
  x = (float)(interm.x / magn);
  y = (float)(interm.y / magn);
  step.x = (int)(nstar.x + step_size*x);
  step.y = (int)(nstar.y + step_size*y);
  return step;
}
int is_valid_1(reference p, reference q)
{
  reference large, small;
  int i = 0, j1 = 0, j2 = 0;
  double slope;

  if(q.x<p.x)
  {
    small = q;
    large = p;
  }
  else
  {
    small = p;
    large = q;
  }

  if(large.x - small.x<3)
    return 0;

  slope = ((double)large.y - small.y)/((double)large.x - small.x);

  for(i=small.x+1; i<large.x; i++)
  {
    j1 = (int)((i*slope) - (small.x)*(slope) + small.y);
     for (int w =j1-7 ; w <= j1+7; ++w)
    {
      for (int z = i-7; z <= i+7; ++z)
      {
        if((w<0) || (w>600) || (z<0) || (z>600))
        continue;
      
        if(((int)img.at<Vec3b>(z, w)[0] ==255) && ((int)img.at<Vec3b>(z,w)[1] == 255) && ((int)img.at<Vec3b>(z, w)[2] == 255))
        return 0;   
      }
    }
  }
  return 1;
}

void draw_path()
{
  int count=0;
  star up, down;
  int breaking = 0;
  down = end_star;
  up = *(end_star.parent);
  while(1)
  {
    count++;
    line(img, Point(up.position.y, up.position.x), Point(down.position.y, down.position.x), Scalar(255, 255, 0), 2, 8);
    if(up.parent == NULL){
      cout<<count<<endl;
      break;
    }

    up = *(up.parent);
    down = *(down.parent);
  }
}

int check_node(int i,int j)
{
  for (int k = 0; k < totstars; ++k)
  {
    if (stars[k]->position.x==i&&stars[k]->position.y==j)
    {
      return k;
    }
  }
  return 0;
}

int path_dist(star* stepstar)
{
  float path=0;
  while(stepstar->parent!=NULL)
  {
      path+=star_dist(stepstar->position,stepstar->parent->position);
      stepstar=stepstar->parent;
  }
  return path;
}

void rrt()
{
  int flag1 = 0, index = 0, flag2 = 0,k;

    star* rstar = new star;
    star* stepstar = new star;

    (rstar->position).x = rand() % 600 + 1;
    (rstar->position).y = rand() % 600 + 1;

    index = near_star(*rstar);

    if((star_dist(rstar->position, stars[index]->position)) < step_size)
      return;
    else
      stepstar->position = stepping(stars[index]->position, rstar->position);

    flag1 = is_valid_1(stars[index]->position, stepstar->position);
    
    if(flag1 == 1)
    {
      stars[totstars++] = stepstar;
      stepstar->parent = stars[index];
      
      line(img, Point((stepstar->position).y, (stepstar->position).x), Point(stars[index]->position.y, stars[index]->position.x), Scalar(0, 255, 255), 2, 8);

      for(int i=stepstar->position.x - 2; i < stepstar->position.x + 2; i++)
      {
        for(int j=stepstar->position.y - 2; j < stepstar->position.y + 2; j++)
        {
          if((i<0) || (i>600) || (j<0) || (j>600))
            continue;

          img.at<Vec3b>(i, j)[0] = 0;
          img.at<Vec3b>(i, j)[1] = 0;
          img.at<Vec3b>(i, j)[2] = 255;
        }
      }

      for (int i = stepstar->position.x-area; i < stepstar->position.x+area; ++i)
      {
        for (int j = stepstar->position.y-area; j < stepstar->position.y+area; ++j)
        {
          if (k=check_node(i,j))
            {
              
                if(is_valid_1(stepstar->position,stars[k]->position) )
                {
                  if(path_dist(stepstar)>star_dist(stepstar->position,stars[k]->position)+path_dist(stars[k]))stepstar->parent=stars[k];
                }
              
            } 
        }
      }

      for (int i = stepstar->position.x-area; i < stepstar->position.x+area; ++i)
      {
        for (int j = stepstar->position.y-area; j < stepstar->position.y+area; ++j)
        {
          if (k=check_node(i,j))
            {
              if(is_valid_1(stepstar->position,stars[k]->position))
              {
                if( path_dist(stepstar)+star_dist(stepstar->position,stars[k]->position) < path_dist(stars[k])) stars[k]->parent=stepstar;
              }
            } 
        }
      }

      if((is_valid_1(stepstar->position, end_star.position)) && (star_dist(stepstar->position,end_star.position) < 3*step_size))
      {
        for (int i = stepstar->position.x-200; i < stepstar->position.x+200; ++i)
        {
          for (int j = stepstar->position.y-200; j < stepstar->position.y+200; ++j)
          {
            if (k=check_node(i,j))
            {
              if (path_dist(stars[k])+star_dist(stepstar->position,stars[k]->position) < path_dist(stepstar) && is_valid_1(stepstar->position,stars[k]->position))
              {
                stepstar->parent=stars[k];
              }
            }
          }
        }

        reached = 1;
        stars[totstars++] = &end_star;
        end_star.parent = stepstar;
        
        draw_path();
      }
    }
    iter++;
}

int main()
{
  star temp_star;

  fstream f("/home/astitva/catkin_ws/src/beginner_tutorials/src/rrt_data.dat",ios::out|ios::binary);

  Mat img1 = imread("rrt.png", 0);
  Mat img2(img1.rows,img1.cols,CV_8UC3,Scalar(0,0,0));
  for(int i=0;i<img1.rows;i++)
  {
    for(int j=0;j<img1.cols;j++)
    {
      if(img1.at<uchar>(i,j)>150)
      {  
        img2.at<Vec3b>(i,j)[0]=255;
        img2.at<Vec3b>(i,j)[1]=255;
        img2.at<Vec3b>(i,j)[2]=255;
      } 
    }
  }
  img=img2;
  init();
  namedWindow( "window", WINDOW_AUTOSIZE );
  while((reached == 0))
  {
    rrt();
    imshow("window", img);
    waitKey(1);
  }

temp_star=end_star;

while(temp_star.parent!=NULL)
{
  refer.x=temp_star.position.x;
  refer.y=temp_star.position.y;
  f.write(reinterpret_cast<char*>(&refer),sizeof(reference));
  temp_star=*(temp_star.parent);
}

  f.close();
  
  imshow("window", img);

  waitKey();
  return 0;
}

