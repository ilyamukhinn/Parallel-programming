#include "stdlib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "pthread.h"

int N = 0; //amount of characters in a number
std::vector<std::pair<int, bool>> Numbers; //Numbers with bool-value: true - a special number, otherwise - no

//a special number is a number that is completely divisible by the sum of 
//the number of digits of the divisors of this number

void* fthread(void* arg); //threads' function

int main(int argc, char const *argv[]) 
{
    srand(time(0));
    
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

	//Each thread works from the begin number to the end number,
	//we define the boundaries of the flow
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

	//The cycle of checking whether a number is special
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
	
	return NULL;
}
