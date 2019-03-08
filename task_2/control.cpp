#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>
#include <turtlesim/Pose.h>
#include <math.h>
#include <fstream>

using namespace std;

turtlesim::Pose pos;
float des_x;
float des_y;

typedef struct 
{
	float x,y;
}coordinate;

coordinate refer1,refer2[50];

void position_callback(const turtlesim::Pose::ConstPtr& msg)
{
    pos.x = msg->x;
    pos.y = msg->y;
    pos.theta = msg->theta;   
}

float linear_distance(turtlesim::Pose pos,float x , float y)
{
    return sqrt((pos.x-x)*(pos.x-x)+(pos.y-y)*(pos.y-y));
}

float angle(turtlesim::Pose pos,float x , float y)
{
    return atan2f(y-pos.y , x-pos.x)-pos.theta;
}

int main(int argc, char**argv)
{
	ros::init(argc, argv, "control");

	fstream f("/home/astitva/catkin_ws/src/beginner_tutorials/src/rrt_data.dat",ios::binary|ios::in);

	f.seekg(-1*sizeof(coordinate),ios::end);
	int size=(f.tellg())/sizeof(coordinate);

	ros::NodeHandle n;
	ros::Subscriber pos_sub = n.subscribe("/turtle1/pose", 5 , position_callback);
	ros::Publisher velocity_publisher = n.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 200);
    
    for(int k=1;k<=size;k++)
    {
    	f.seekg(-k*sizeof(coordinate),f.end);
    	f.read(reinterpret_cast<char *>(&refer1),sizeof(coordinate));
    	refer2[k-1]=refer1;
    }
	    ros::Rate loop_rate(10);

	    int count = 0,flag=1;
	    geometry_msgs::Twist msg;
	    
	    while(ros::ok())
	    {
	    	ros::spinOnce();

	    	if(flag==1)
	    	{
	    		

    			des_x =refer2[count].y/60.0;
	    		des_y =10.0-refer2[count].x/60.0;
	    		flag=0;
    		}
	            
	        if(linear_distance(pos,des_x,des_y)>0.1)
	        {

	            if(angle(pos,des_x,des_y)*angle(pos,des_x,des_y)>0.01)
	            {

	            	msg.angular.z = angle(pos,des_x,des_y)*0.4;
	            	msg.linear.x=0;
	        	}
	        	else
	        	{
	        		if(linear_distance(pos,des_x,des_y)>0.1)
	        		{	
	        			msg.angular.z=0;
	            		msg.linear.x = (linear_distance(pos,des_x,des_y))*0.7;
	            	}
	            	else
	            	{
	            		msg.angular.z=0;
	            		msg.linear.x = 0;
	            		flag=1;
	            		count++;
	            	}
	        	}
	        }

	        else
	        {
	        	msg.linear.x=0;
	        	msg.angular.z=0;
	        	flag=1;
	            count++;
	        }
	        
	        velocity_publisher.publish(msg);
	        
	        loop_rate.sleep();
    	}
    	

    return 0;
}