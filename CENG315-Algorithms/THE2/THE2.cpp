#include "the2.h"
#include <cmath>

// You may write your own helper functions here


long countingSort(int *arr, int size, int mod, long &numberOfIterations, bool ascending, int divider) // divider[1:maxDigit-1] ve divider = groupNum*x + max%groupnum
{
    int B[size];
    int C[mod];
    for(int i = 0; i<mod; i++)
    {
        C[i] = 0;

    }

    for(int j = 0; j<size; j++)
    {
        C[(arr[j] / divider)%mod]++;
        
        numberOfIterations++;
    }
    for(int i = 1; i<mod; i++)
    {
        C[i] += C[i-1];
        numberOfIterations++;
    }
    
    if(ascending) //artan
    {
        for(int j = size-1 ; j>=0 ; j--)
        {
            B[C[ (arr[j] / divider)%mod ]-1] = arr[j];
        
            C[ (arr[j] / divider) % mod ]--;
            
            numberOfIterations++;
        }
        for(int i = 0; i<size; i++)
        {
            arr[i] = B[i];
            numberOfIterations++;
        }
    }
    else //reversed artan but not stable anymore.
    {
        for(int j = 0 ; j<size ; j++)
        {
            B[C[ (arr[j] / divider)%mod ]-1] = arr[j];
        
            C[ (arr[j] / divider) % mod ]--;
            numberOfIterations++;
        }
        for(int i = 0; i<size; i++)
        {
            arr[size-1-i] = B[i];
            numberOfIterations++;
        }
    }
    
    return numberOfIterations;
    
}




long radixSort(int *arr, int size, int digitcount, long &numberOfIterations, int groupSize, int maxDigitLength, bool ascending)
{
    for(digitcount = groupSize; digitcount<=maxDigitLength; digitcount+=groupSize)
    {
        countingSort(arr, size, pow(10, groupSize), numberOfIterations, ascending, pow(10,digitcount-groupSize));
        
    }
    
    if((maxDigitLength % groupSize) == 0)
    {
        return numberOfIterations;
    }
    else
    {
        countingSort(arr, size, pow(10,maxDigitLength % groupSize) , numberOfIterations, ascending,  pow(10,digitcount-groupSize));
        
        return numberOfIterations;
    }
}

long multiDigitRadixSort(int* arr, bool ascending, int arraySize, int groupSize, int maxDigitLength){
    long numberOfIterations = 0;
    int digitcount = 1;
    
    
    radixSort(arr, arraySize, digitcount, numberOfIterations, groupSize, maxDigitLength, ascending);
    
    return numberOfIterations;
}
