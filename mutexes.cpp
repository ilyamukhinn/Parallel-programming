#include "stdlib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "pthread.h"

int N = 0;
std::vector<std::pair<int, bool>> Numbers;

void* fthread(void* arg);
pthread_mutex_t mutex; //A mutex to protect a write to a shared queue

//A shared queue into which threads will write that they have completed their work
struct result 
{
  char str[256];
  result* next;
};

result* head;
result* newrez;
result* rez;

int main(int argc, char const *argv[]) 
{
    srand(time(0));

    head = new result;
    rez = head;
  
    pthread_mutex_init(&mutex, NULL); //mutex's initialization
    
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

        if(i % Digits_amount == 0) 
		{
            is_Special = true;
        }

        Numbers[i - 1] = std::make_pair(i, is_Special);
    }

    pthread_mutex_lock(&mutex); //Closing the mutex for writing to the shared queue
  
    newrez = new result;
    sprintf(newrez->str, "Thread #%d did its work\n", thread_num);
    newrez->next = NULL;
    rez->next = newrez;
    rez = newrez;
    
    pthread_mutex_unlock(&mutex); //Open the mutex, now other threads can continue their writing to the shared queue
    
    return NULL;
}
