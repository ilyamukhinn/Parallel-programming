#include "pthread.h"
#include "stdlib.h"
#include <ostream>
#include <iostream>
#include <string>
#include <vector>

#define N 10 //amount of groups

std::vector<int> Groups; //Group array, group numbers will be stored here
std::vector<int> Playground; //An array in which the numbers of the groups that are located on the site will be stored
std::vector<int> Kitchen; //An array in which the numbers of the groups that are in the kitchen will be stored

pthread_mutex_t mutex;
int count_cond = 0; //Value for working with a conditional variable
pthread_cond_t wait; //Conditional variable

void* fbring_into (void* arg); //Function for thread 1 - places the group(s) on the playground or in the kitchen
void* fget_out (void* arg); //Function for thread 2 - outputs the group(s) from the playground or kitchen

int main(int argc, char const *argv[]) 
{
	srand(time(0));
	
	Groups.resize(N);
  
	for(int i = 0; i < N; i++) 
	{
		Groups[i] = i + 1;
	}

	//No more than 2 groups can be on the playground and kitchen at the same time
	Playground.resize(2);
	Kitchen.resize(2);

	for(int i = 0; i < 2; i++) 
	{
		Playground[i] = 0;
		Kitchen[i] = 0;
	}
  
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&wait, NULL);

	while(true)
	{ 
		pthread_t bring_into; 
		pthread_t get_out;
		
		pthread_create(&bring_into, NULL, fbring_into, NULL);
		pthread_create(&get_out, NULL, fget_out, NULL);

		pthread_join(bring_into, NULL);
		pthread_join(get_out, NULL);

		std::cout << "Now there are such groups in their rooms:" << std::endl;

		for(int i = 0; i < N; i++) 
		{
			if(Groups[i] != 0) 
			{
				std::cout << "Group #" << Groups[i] << std::endl;
			}
		}

		std::cout << std::endl << "On the playground:" << std::endl;

		int Not_Found = 0;

		for(int i = 0; i < 2; i++) 
		{
			if(Playground[i] != 0) 
			{
				std::cout << "Group #" << Playground[i] << std::endl;
			}
			else 
			{
				Not_Found++;
			}
		}

		if(Not_Found == 2) 
		{
			std::cout << "There are no groups on the playground" << std::endl;
		}

		std::cout << std::endl << "In the kitchen:" << std::endl;

		Not_Found = 0;

		for(int i = 0; i < 2; i++) 
		{
			if(Kitchen[i] != 0) 
			{
				std::cout << "Group #" << Kitchen[i] << std::endl;
			}
			else 
			{
				Not_Found++;
			}
		}

		if(Not_Found == 2) 
		{
			std::cout << "There are no groups in the kitchen" << std::endl;
		}
		
		std::cout << "Press enter to continue ...";
		std::cin.get();
		system( "clear" );
	}
  
	return 0;
}

void* fbring_into(void* arg) 
{ 
	//Close one mutex and check,
	//if the value of the conditional variable = 1, then the thread falls asleep, and the mutex
	//opens so that another thread can continue working
    pthread_mutex_lock(&mutex);
    while(count_cond == 1) 
	{
        pthread_cond_wait(&wait, &mutex);
    }

    for(int i = 0; i < N; i++) 
	{
        for(int j = 0; j < 2; j++) 
		{
            if((rand() % 3) 
                && (Groups[i] != 0) 
                && (Playground[j] == 0)) 
			{
                    Playground[j] = Groups[i];
                    Groups[i] = 0;
            }

            if((rand() % 3) 
                && (Groups[i] != 0) 
                && (Kitchen[j] == 0)) 
			{
                    Kitchen[j] = Groups[i];
                    Groups[i] = 0;
            }
        }
    }
	
	//Change the value for the conditional variable,
	//open the mutex and wakes up all threads
    count_cond = 1;
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&wait);

    return NULL;
}

void* fget_out(void* arg)
{ 
	//Close one mutex and check,
	//if the value of the conditional variable = 0, then the thread falls asleep, and the mutex
	//opens so that another thread can continue working
    pthread_mutex_lock(&mutex);
    while(count_cond == 0) 
	{
        pthread_cond_wait(&wait, &mutex);
    }

    for(int i = 0; i < 2; i++) 
	{
        if((rand() % 3) && (Playground[i] != 0)) 
		{
            Groups[Playground[i] - 1] = Playground[i];
            Playground[i] = 0;
        }

        if((rand() % 3) && (Kitchen[i] != 0)) 
		{
            Groups[Kitchen[i] - 1] = Kitchen[i];
            Kitchen[i] = 0;
        }
    }

	//Change the value for the conditional variable,
	//open the mutex and wakes up all threads
    count_cond = 0;
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&wait);

    return NULL;
}
