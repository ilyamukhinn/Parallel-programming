#include "stdlib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "pthread.h"
#include "semaphore.h"

int N = 0;
std::vector<std::pair<int, bool>> Numbers;

void* fthread(void* arg);
pthread_mutex_t mutex;

struct result 
{
  char str[256];
  result* next;
};

result* head;
result* newrez;
result* rez;

sem_t sem; //Semaphore for limiting the number of threads running at one time
int K = 2; //Number of threads running at one time

int main(int argc, char const *argv[]) 
{
    srand(time(0));

    head = new result;
    rez = head;
  
    pthread_mutex_init(&mutex, NULL);
    sem_init (&sem, 0, K); //semaphore's initialization
    
    std::cout << "Enter the number N: ";
    std::cin >> N;

    Numbers.resize(pow(10, N) - 1);

    pthread_t p[4];
    int* arg = new int[4];

    for (int i = 0; i < 4; i++) 
	{
        arg[i] = i;
        pthread_create(&p[i], NULL, fthread, (void*)&arg[i]);
    }

    for (int i = 0; i < 4; i++) 
	{
        pthread_join (p[i], NULL);
    }

    std::cout << std::endl;

    rez = head->next;
  
    while(rez) 
	{
        fprintf(stdout, "%s", rez->str);
        rez = rez->next;
    }

    std::cout << std::endl << "At one time, only " << K << " thread(s) was/were working" << std::endl;

    std::cout << std::endl << "All special " << N << "-digit numbers:" << std::endl;

    for(int i = 0; i < Numbers.size(); i++) 
	{
        if(Numbers[i].second) 
		{
            std::cout << Numbers[i].first << std::endl;
        }
    }

    return 0;
}

void* fthread(void* arg) 
{
    sem_wait (&sem); //Subtracting one from the semaphore value, if it reaches zero, then
					 //all other threads will not be able to continue their work until one of those
					 // that are already working increases this value by one
    int thread_num = *(int*)arg;
    int begin = 0, end = 0;

    switch (thread_num) 
	{
    case 0:
        begin = pow(10, N - 1);
        end = pow(10, N) / 4;
        break;
    case 1:
        begin = pow(10, N) / 4;
        end = pow(10, N) / 2;
        break;
    case 2:
        begin = pow(10, N) / 2;
        end = pow(10, N) - pow(10, N) / 4;
        break;
    case 3:
        begin = pow(10, N) - pow(10, N) / 4;
        end = pow(10, N);
        break;
    default:
        break;
    }

    for(int i = begin; i < end; i++) 
	{
        std::vector<int> i_Divs;

        for(int j = 1; j < i + 1; j++) 
		{
            if(i % j == 0) 
			{
                i_Divs.push_back(j);
            }
        }

        int Digits_amount = i_Divs.size();
  
        for(int j = 0; j < i_Divs.size(); j++) 
		{
            while ((i_Divs[j] /= 10) != 0) 
			{
                Digits_amount++;
            }
        }

        bool is_Special = false;

        if(i % Digits_amount == 0) {
            is_Special = true;
        }

        Numbers[i - 1] = make_pair(i, is_Special);
    }

    pthread_mutex_lock(&mutex);
  
    newrez = new result;
    sprintf(newrez->str, "Thread #%d did its work\n", thread_num);
    newrez->next = NULL;
    rez->next = newrez;
    rez = newrez;
    
    pthread_mutex_unlock(&mutex);
    sem_post (&sem); //Increasing the semaphore value by one

    return NULL;
}
