#include "stdlib.h"
#include <iostream>
#include <string>
#include <regex>
#include "pthread.h"
#include <mpich/mpi.h>

int main(int argc, char const *argv[]) 
{
    srand(time(0));

    int rank, size;

    MPI_Init (0, 0);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    
	//For this program, the number of processes should be equal to 3 (one is the main one, the others generate str1 and str2 and count the number of occurrences in str)
    if (size != 3) 
	{
        if (rank == 0) printf("\nThe number of processes \nhas to be 3\n\nProgram exit\n");
        MPI_Finalize();
        return 0;
    }

    std::string str = "";
    int iteration = 0; 
    
	//The 0th process generates a str and sends this string to other processes
    if(rank == 0) 
	{
        for(int i = 0; i < 30; i++) 
		{
            str += char('a' + rand() % ('z' - 'a'));
        }

        for(int i = 1; i < size; i++) 
		{
            for(int j = 0; j < 30; j++) 
			{
                MPI_Send(&str[j], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        }
    }

	//Non-0th processes receive the string str
    if(rank != 0) 
	{
        MPI_Status status;

        for(int i = 0; i < 30; i++) 
		{
            char ch;
            MPI_Recv(&ch, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            str += ch;
        }
    }
	
    while(true) 
	{
        //The 0th process counts the number of iterations of the program
		if(rank == 0) 
		{
            iteration++;
        }

		//Non-0th threads generate str1 and str2 respectively
		//and count the number of their occurrences in str, and then pass
		//counts to the 0th process
        if(rank != 0) 
		{
            srand(time(0) + rank);

            std::string local_str(str);
            std::string proccess_str = "";

            for(int i = 0; i < 2; i++) 
			{
                proccess_str += char('a' + rand() % ('z' - 'a'));
            }

            int local_k = (str.size() - (std::regex_replace(local_str, (std::regex)proccess_str, "")).size()) / proccess_str.size();

            for(int i = 0; i < 2; i++) 
			{
                MPI_Send(&proccess_str[i], 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }

            MPI_Send(&local_k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

		//the 0th process outputs the result of the processes, and if the condition is met, then
		//informs other processes to exit the program cycle
        if(rank == 0) 
		{
            MPI_Status status;

            std::string str1 = "", str2 = "";
            int k1 = 0, k2 = 0;
            
            for(int i = 0; i < 2; i++) 
			{
                char ch;
                MPI_Recv(&ch, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
                str1 += ch;
            }

            MPI_Recv(&k1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

            for(int i = 0; i < 2; i++) 
			{
                char ch;
                MPI_Recv(&ch, 1, MPI_CHAR, 2, 0, MPI_COMM_WORLD, &status);
                str2 += ch;
            }

            MPI_Recv(&k2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

            std::cout << "Str: " << str << std::endl;
            std::cout << "Str1: " << str1 << std::endl;
            std::cout << "Str2: " << str2 << std::endl;
            std::cout << "The number of occurrences of str1 in str: " << k1 << std::endl;
            std::cout << "The number of occurrences of str2 in str: " << k2 << std::endl;        

            bool is_Final = false;

            if(k1 != k2) 
			{
                std::cout << "k1 is not equal to the k2" << std::endl;
                std::cout << "Number of iterations completed: " << iteration << std::endl;
                
                is_Final = true;
            }

            std::cout << std::endl << "Press enter to continue ...";
            std::cin.get();
            system( "clear" );

			//Transmitting information to other threads about whether it is necessary to exit the program cycle
			//and finish your work
            for(int i = 1; i < size; i++) 
			{
                MPI_Send(&is_Final, 1, MPI_CXX_BOOL, i, 0, MPI_COMM_WORLD);
            }

            if(is_Final) 
			{
                break;
            }
        }

        if(rank != 0) 
		{
            MPI_Status status;

            bool is_Final = false;
            MPI_Recv(&is_Final, 1, MPI_CXX_BOOL, 0, 0, MPI_COMM_WORLD, &status);

            if(is_Final) 
			{
                break;
            }
        }
    }

    MPI_Finalize();
    return 0;    
}
