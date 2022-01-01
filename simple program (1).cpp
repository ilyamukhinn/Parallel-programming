#include "stdlib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "pthread.h"

int M = 10; //Number of points considered
std::vector<std::vector<int>> Points(M, std::vector<int>(2)); //Points' array
std::vector<bool> Suitable_points(M, false); //Array of bool values: true - the point fits, false - the point does not fit

void* fthread(void* arg); //function for threads
int func(int x); //function, which calculates f(x)

int main(int argc, char const *argv[]) 
{
    srand(time(0));
    
	//Generate random points
    std::cout << "All points:" << std::endl;

    for(int i = 0; i < M; i++) 
	{
        Points[i][0] = rand() % 11 - 5;
        Points[i][1] = rand() % 11 - 5;
        std::cout << "Point #" << i + 1 << " - x: " << Points[i][0] 
            << ", y: " << Points[i][1] << std::endl;
    }

	//threads' declaration
    pthread_t p[M];
    int* arg = new int[M];

	//threads' creation
    for (int i = 0; i < M; i++) 
	{
        arg[i] = i;
        pthread_create(&p[i], NULL, fthread, (void*)&arg[i]);
    }

    for (int i = 0; i < M; i++) 
	{
        pthread_join (p[i], NULL);
    }

    std::cout << std::endl << "Function f(x) = x * 2 - 1" << std::endl;

    std::cout << std::endl << "Suitable points:" << std::endl;

    if(find(Suitable_points.begin(), Suitable_points.end(), true) 
                == Suitable_points.end()) 
	{
        std::cout << "There are no suitable points" << std::endl;
        return 0;
    }

    for(int i = 0; i < M; i++) 
	{
        if(Suitable_points[i]) 
		{
            std::cout << "Point #" << i + 1 << " - x: " << Points[i][0] 
                << ", y: " << Points[i][1] << std::endl;
        }
    }

    return 0;
}

void* fthread(void* arg) 
{
    int point_num = *(int*)arg;

	//if y = f(x) point fits
    if(Points[point_num][1] == func(Points[point_num][0])) 
	{
        Suitable_points[point_num] = true;
    }

    return NULL;
}

int func(int x) 
{
    return 2 * x - 1;
}
