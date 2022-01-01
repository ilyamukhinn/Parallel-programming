#include "stdlib.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "pthread.h"

pthread_rwlock_t rwlock; //Declaring a mutex to protect the read/write array of points
pthread_mutex_t mutex; //Declaring a mutex to protect writing to the console

std::vector<std::vector<int>> Points; //Points' array

void* fadd(void* arg); //Function for adding new points
void* fsolutions(void* arg); //The function of finding solutions to the equation
void* fdel(void* arg); //Point removal function
void* fprint(void* arg); //The function of displaying points to the console

std::vector<int> Coefficients = {-2, 3, 7, -4, 5, 3}; //Coefficients of the equation

int main(int argc, char const *argv[]) 
{
    srand(time(0));

    pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_t padd;
    pthread_t psolutions;
    pthread_t pdel;
    pthread_t pprint;

    pthread_create(&padd, NULL, fadd, NULL);
    pthread_create(&psolutions, NULL, fsolutions, NULL);
    pthread_create(&pdel, NULL, fdel, NULL);
    pthread_create(&pprint, NULL, fprint, NULL);

    pthread_join(padd, NULL);
    pthread_join(psolutions, NULL);
    pthread_join(pdel, NULL);
    pthread_join(pprint, NULL);

    return 0;
}

void* fadd(void* arg) 
{
    while(1) 
	{
        int X = rand() % 21 - 10; 
        int Y = rand() % 21 - 10; 
        std::vector<int> New_point = {X, Y};

        pthread_rwlock_wrlock(&rwlock); //We close the mutex for writing, i.e. now no thread will be able to continue working

        Points.push_back(New_point);

		std::cout << std::endl << "New point was added: " << std::endl;
        std::cout << "X: " << New_point[0] << std::endl;
        std::cout << "Y: " << New_point[1] << std::endl;
        
        pthread_rwlock_unlock(&rwlock); //Open the mutex
    }
}

void* fsolutions(void* arg) 
{
    while(1) 
	{
        pthread_rwlock_rdlock(&rwlock); //We close the mutex for reading, i.e. now only threads that also use mutexes for reading will be able to continue working

        if(Points.size()) 
		{
            int Res = 0;
            int Res_index = 0;
            int Diff = 0;

			//Calculation of the closest to Y among all points of the solution of the equation
            for(int i = 0; i < Coefficients.size(); i++) 
			{
                Res += Coefficients[i] * pow(Points[0][0], i);
            }

            Diff = abs(Points[0][1] - Res);

            for(int i = 1; i < Points.size(); i++) 
			{
                int Curr_Res = 0;

                for(int j = 0; j < Coefficients.size(); j++) 
				{
                    Curr_Res += Coefficients[j] * pow(Points[i][0], j);
                }

                if(Diff > abs(Points[i][1] - Curr_Res)) 
				{
                    Diff = abs(Points[i][1] - Curr_Res);
                    Res_index = i;
                    Res = Curr_Res;
                }
            }

            pthread_mutex_lock(&mutex); //Closing the mutex to make an entry in the console

            std::cout << std::endl << "The most suitable point for the solution:" << std::endl;
            std::cout << "X: " << Points[Res_index][0] << std::endl;
            std::cout << "Y: " << Points[Res_index][1] << std::endl;
            std::cout << "And the result of the equation with this point = " << Res << std::endl;

            

            pthread_mutex_unlock(&mutex); //Opening the mutex for the console
        }

        pthread_rwlock_unlock(&rwlock);
    }
}

void* fdel(void* arg) 
{
    while(1) 
	{
        pthread_rwlock_wrlock(&rwlock);
        
		//If there is at least 1 point, then the random point is deleted
        if(Points.size()) 
		{
            int Del_index = rand() % Points.size();
        
            std::cout << std::endl << "Point: " << std::endl;
            std::cout << "X: " << Points[Del_index][0] << std::endl;
            std::cout << "Y: " << Points[Del_index][1] << std::endl;
            std::cout << "was deleted" << std::endl;

            Points.erase(Points.begin() + Del_index);
        }

        pthread_rwlock_unlock(&rwlock);
    } 
}

void* fprint(void* arg) 
{
    while(1) 
	{
        pthread_rwlock_rdlock(&rwlock); 
        pthread_mutex_lock(&mutex); 

        std::cout << std::endl << "Printing all points..." << std::endl;

        for(vector<int> point : Points) 
		{
            std::cout << std::endl << "Point: " << std::endl;
            std::cout << "X: " << point[0] << std::endl;
            std::cout << "Y: " << point[1] << std::endl;
        }

        pthread_mutex_unlock(&mutex);
        pthread_rwlock_unlock(&rwlock); 
    }
}