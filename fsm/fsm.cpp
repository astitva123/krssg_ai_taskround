#include<iostream>
#include <string.h>
using namespace std;
//using namespace cv;

int curr_floor,keypad[200]={0},in=0,out=0,max1=1,min1=4,total,output[200]={0},count=0,in_size,out_size,test_out,test_in=0,top,bottom;
int flag=0; 		//flag is a variable which will keep count whether the previous state was UP or DOWN 0 means down 1 means up

class Elevator
{
public:
	enum state
	{
		IDLE,UP,DOWN	
	};
	state heh;

	struct button
	{
		int floor=0;
		int fl;
	};

	struct button outside[200];

	void start(Elevator *test);
	
	void transition_to(int state,Elevator *test)
	{
		test->heh=static_cast<Elevator::state>(state);
	}

	int idle_to_up(Elevator *test)
	{
		if ((curr_floor<top&&flag)||curr_floor==bottom)
		{
			transition_to(1,test);		
			return 1;				 
		}

		return 0;

	}

	int idle_to_down(Elevator *test)
	{
		if ((curr_floor>bottom&&!flag)||curr_floor==top)
		{
			transition_to(2,test);
			return 1;
		}
		return 0;
	}

	void up_to_idle(Elevator *test,int a,int b);

	void down_to_idle(Elevator *test,int a,int b);

}test;

int search_array_out(Elevator *test);//confirm whether this is the way to call object of structure which is in a class
int search_array_in();
void remove_in();
void remove_out(Elevator *test); //same here as stated above

void Elevator::up_to_idle(Elevator *test,int a,int b)
	{
		flag=1;

		if (b)
		{
			if (a)
			{
				transition_to(0,test);
				remove_in();
				remove_out(test);

				output[count]=curr_floor;
				count++;			//transition to idle and remove from the input and put in output	
			}
			else
			{
				transition_to(0,test);
				remove_out(test);

				output[count]=curr_floor;
				count++;				//transition to idle and remove from the input and put in output
			}			
		}
		else
		{

			if (a)
			{
				transition_to(0,test);
				remove_in();

				output[count]=curr_floor;
				count++;		//transition to idle and remove from input
			}

		}	
	}

void Elevator::down_to_idle(Elevator *test,int a,int b)
	{
		flag=0;

		if (b)
		{
			if (a)
			{
				transition_to(0,test);
				remove_in();
				remove_out(test);

				output[count]=curr_floor;
				count++;			//transition to idle and remove from the input and put in output	
			}
			else
			{
				transition_to(0,test);
				remove_out(test);

				output[count]=curr_floor;
				count++;				//transition to idle and remove from the input and put in output
			}			
		}
		else
		{
			if (a)
			{
				transition_to(0,test);
				remove_in();

				output[count]=curr_floor;
				count++;		//transition to idle and remove from input
			}

		}
	}

void Elevator::start(Elevator *test)
{
	if (test->heh==0)
	{
		idle_to_up(test);
		idle_to_down(test);
	}
	else
	{
		cout<<endl<<"current floor is: "<<curr_floor<<endl;
		test_in=search_array_in();
		test_out=search_array_out(test);

		if (test_out||test_in)
		{
			if (test->heh==1)
			{
				up_to_idle(test,test_in,test_out);
			}
			else
			{
				down_to_idle(test,test_in,test_out);
			}
			
		}
		else
		{
			if (test->heh==1)
			{
				curr_floor++;
			}
			else
			{
				curr_floor--;
			}
		}
	}
	
}


int main()
{
	int initial;
	char convert[5];
	test.heh=Elevator::IDLE;

	cout<<"\n"<<" enter how many buttons are pressed inside: ";
	cin>>in;
	in_size=in;
	cout<<"\n enter how many buttons are pressed outside: ";
	cin>>out;
	out_size=out;
	if(in) cout<<"\nenter the buttons pressed inside the lift: ";
	total=in+out;

	for (int i = 0; i < in; ++i)
	{
		cin>>keypad[i];
		if (keypad[i]>max1)
		{
			max1=keypad[i];
		}

		if (keypad[i]<min1)
		{
			min1=keypad[i];
		}
	}
	
	cout<<"\n";
	for (int i = 0; i < out; ++i)
	{
		cout<<"enter the floor pressed outside: ";
		cin>>test.outside[i].floor;
		cout<<"enter state for floor "<< test.outside[i].floor<<" (1 for up, 2 for down):";
		cin>>test.outside[i].fl;

		/*if(cout<<!strcmp("up",convert)) {test.outside[i].fl=1;}
		else  { if(cout<<!strcmp("down",convert)) {test.outside[i].fl=2;}
		else {test.outside[i].fl=0;}}*/


		//cout<<test.outside[i].fl<<"   ";

		if (max1<test.outside[i].floor)
		{
			max1=test.outside[i].floor;
		}

		if (min1>test.outside[i].floor)
		{
			min1=test.outside[i].floor;
		}
	}

	cout<<"\nenter the initial floor: ";
	cin>>initial;

	curr_floor=initial;
	top=max1;
	bottom=min1;

	if(max1>=curr_floor&&min1<=curr_floor)
	{
		if (max1-curr_floor<curr_floor-min1)
		{
			test.heh=Elevator::UP;
			//cout<<"goin up first.";
			flag=1;
		}
		else
		{
			test.heh=Elevator::DOWN;
			//cout<<"goin down first";
			flag=0;
		}
	}
	else
	{
		if (curr_floor>=max1)
		{
			test.heh=Elevator::DOWN;
			flag=0;
		}
		else
		{
			test.heh=Elevator::UP;
			flag=1;
		}
	}
	int z=1;
	while(in +out)
	{
		/*cout<<"\ncurrent floor: "<<curr_floor;
		cout<<"\nfloors lesft inside: "<<in<<"   \nfloors outside: "<<out;*/
		//cout<<"\n"/*<<test.heh*/;
		//if (curr_floor<0)
		//{
			getchar();
			cout<<curr_floor<<" "<<test.heh;
		//}
		test.start(&test);
		cout<<endl<<in+out<<"left"<<endl;
		//getchar();
	}

	//cout<<"\nthe rpogramme came out of the loop succesfully!!!!!!\n";

	for (int i = 0; i < count; ++i)
	{
		cout<<">> floor"<<output[i];
	}

	cout<<"\n";
		
	return 0;
}

int search_array_in()
{
	/*if(curr_floor==4) {
		cout<<"\nlets see \n";
		for (int i = 0; i < in_size; ++i)
		{
			cout<<keypad[i];

		}
	}*/
	for (int i = 0; i < in_size; ++i)
	{
		if (curr_floor==keypad[i])
		{
			return 1;
		}
	}

	return 0;
}

int search_array_out(Elevator *test)
{
	if(curr_floor!=max1&&curr_floor!=min1)
	{
		for (int i = 0; i < out_size; ++i)
		{
			if (curr_floor==test->outside[i].floor&&test->heh==test->outside[i].fl)
			{
				return 1;
			}
		}
	}

	else
	{
		if(curr_floor==max1) max1=-20;
		if(curr_floor==min1) min1=-20;

		return 1;
		/*
		for (int i = 0; i < out_size; ++i)
		{
			if (curr_floor==test->outside[i].floor&&test->heh!=test->outside[i].fl)
			{
				return 1;
			}
		}*/

	}

	return 0;
}

void remove_in()  // the elemnt to be removed is made zero
{
	for (int i = 0; i < in_size; ++i)
	{
		if (curr_floor==keypad[i])
		{
			keypad[i]=0;
		}
	}
	in--;
}

void remove_out(Elevator *test)  // the elemnt to be removed is made zero
{
	for (int i = 0; i < out_size; ++i)
	{
		if (curr_floor==test->outside[i].floor)
		{
			test->outside[i].floor=0;
		}
	}
	out--;
}