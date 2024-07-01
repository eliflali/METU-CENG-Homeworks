#include "the8.h"

//DO NOT ADD OTHER LIBRARIES

std::vector<std::vector<int>> transition(const std::string& sequence, const std::string& pattern, const std::pair<std::string,std::string> mapping)
{
    
    int states = pattern.length() + 1 ; //m+1
    int patternl = states-1;
    int alphabet;
    int firstindex = -1;
    int secondindex = -1;
    char patind[patternl+1];
    for(int i = 0; i<patternl; i++)
    {
        patind[i+1] = pattern[i];
    }
    std::string first = mapping.first;
    std::string second = mapping.second;
    
    firstindex = int(first[0])-65;
    
    secondindex = int(second[0])-65;
    
    std::vector<std::vector<int>> sigma;
    sigma.resize(states, std::vector<int>(26));
    alphabet = 26;
    for(int i = 0; i<alphabet; i++)
    {
        sigma[0][i] = 0;  //ilk sutun tamamen 0 oldu.
    }
    sigma[0][int(patind[1])-65] = 1; // yalnizca pattern'in ilk uyesi 1 oldu.
    if(int(patind[1])-65 == firstindex)
    {
        sigma[0][secondindex] = 1;
    }
    if(int(patind[1])-65 == secondindex)
    {
        sigma[0][firstindex] = 1;
    }
    int q = 0;
    for(int i = 1; i<patternl; i++)
    {
        for(int j = 0; j<alphabet; j++)
        {
            sigma[i][j] = sigma[q][j];
        }
        sigma[i][int(patind[i+1])-65] = i+1;  //burada i+1 sorundu, indexi 1'den baslattim.
        if(int(patind[i+1])-65 == firstindex)
        {
            sigma[i][secondindex] = i+1;
        }
        if(int(patind[i+1])-65 == secondindex)
        {
            sigma[i][firstindex] = i+1;
        }
        q = sigma[q][int(patind[i+1])-65];
    }    
    //std::cout<<"THIS IS Q"<<q<<std::endl;
    for(int j = 0; j<alphabet; j++)
    {
        sigma[patternl][j] = sigma[q][j]; //bu final state'in nereye gidecegini belirliyor. dolayisiyla final state pattern.length() oldu. (0'dan pattern.length()'e kadar index var.)
    }
    return sigma;
    
}
void matchPattern(const std::string& sequence, const std::pair<std::string,std::string> mapping, const std::string& pattern)
{


    /*****************
     *
     * 
     * YOU SHOULD PRINT THE STARTING INDEX OF EACH MATCH 
     * IN THE SAME LINE, SEPARATED BY A SINGLE SPACE
     * 
     * IF THERE IS NO MATCH, YOU SHOULD PRINT "NONE"
     *
     * *************/
     int q = 0;  //ilk state 0.
     int n = sequence.length();
     int A = pattern.length();
     std::vector<int> solution;
     std::vector<int> bakiyorum;
     std::vector<std::vector<int>> sigma = transition(sequence, pattern, mapping);
     for(int i = 0; i<n; i++)
     {
         q = sigma[q][int(sequence[i])-65];
         if(q == A)
         {
             solution.push_back(i+1-A);
         }
     }
     if(solution.empty())
     {
         std::cout<<"NONE";
     }
     
     else
     {
         int x = solution.size();
         for(int i = 0; i<x-1; i++)
        {
             std::cout<<solution[i]<<" ";
        }
        std::cout<<solution[x-1];
     }
}
