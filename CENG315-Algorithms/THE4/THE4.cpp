#include "the4.h"


int dp_sln(char**& arr1, char**& arr2, int nrow, int ncol1, int ncol2, int**& mem)
{ //dynamic programming

    //your code here
    mem[0][0] = 0;
    for(int i = 1; i<=ncol1; i++)
    {
        //insertion cost for each column seperately calculated
        int insertioncost = 0;
        for(int row = 0; row<nrow; row++)
        {
            if(arr1[row][i-1] != '-')
            {
                insertioncost++; //any character but '
                
            }
        }
        mem[i][0] = mem[i-1][0]+insertioncost;
    }
    for(int j = 1; j<=ncol2; j++)
    {
       
       int insertioncost = 0;
       for(int row = 0; row<nrow; row++)
       {
           if(arr2[row][j-1] != '-')
           {
               insertioncost++;
           }
       } 
       mem[0][j] = mem[0][j-1]+insertioncost;
    }
    //done with 0th row and column of the mem array
    //insertioncost = deletioncost
    
    int result = 0;
    for(int m1 = 1; m1<=ncol1; m1++) //we will only consider last colmn
    {
        for(int m2 = 1; m2<=ncol2; m2++)
        {
            int replacementcost = 0;
            int insertioncost = 0;
            int reorderingcost = 0;
            int basiccase = 0;
            int basicvertical = 0;
            int reconstruct = 0;
            
            
            if(m2 == 1)
            {
            reconstruct = mem[m1][0]+mem[0][m2]; //hepsini sil hepsini bastan yarat
            int isReorder = 1;
            int check = 0;
            char checkarray[nrow];
            int checkindex = 0;
            int checkindex2 = 0;
            int lastelmt = 0;
            basiccase = mem[m1-1][m2] + (mem[m1][0] - mem[m1-1][0]);
            for(checkindex = 0; checkindex<nrow; checkindex++) //we created an array from arr1's related column.
            {
                if(arr1[checkindex][m1-1] != '-')
                {
                    checkarray[lastelmt] = arr1[checkindex][m1-1];
                    lastelmt++;
                }
            }
            for(checkindex2 = 0; checkindex2<nrow; checkindex2++)
            {
                if(arr2[checkindex2][m2-1] != '-')
                {
                    for(int row = 0; row<=lastelmt; row++)
                    {
                        if(arr2[checkindex2][m2-1] == checkarray[row]) // in case elemnt exists in arr1's related colmn:
                        {
                            checkarray[row] = '$';
                            check = 1;
                        }
                    }
                    if(check == 0)
                    {
                        isReorder = 0;
                    }
                }
            }
            for(int row = 0; row<=lastelmt; row++)
            {
                if(checkarray[row] != '$')
                {
                    isReorder = 0;
                }
            }
                if(isReorder) //calculated reordering cost...in case its possible
                {
                    for(int row2 = 0; row2<nrow; row2++)
                    {
                        if(arr1[row2][m1-1] != arr2[row2][m2-1])
                        {
                            reorderingcost++;
                        }
                    }
                    reorderingcost += mem[m1][m2-1]; //with past costs...
                }
                /////REPLACEMENT////
                for(int row = 0; row<nrow; row++) //calculated cost of replacement
                {
                    
                    if(arr1[row][m1-1] != arr2[row][m2-1]) //cost is only when they are inequal
                    {
                        //two choices - vs a letter OR a letter vs a letter
                        if(arr1[row][m1-1] == '-' || arr2[row][m2-1] == '-') //if one of them = '-'
                        {
                            replacementcost += 2;
                        }
                        else //none of them = '-' both letters.
                        {
                            replacementcost++;
                        }
                    }//calculated the replacement for last colmn. now add prev calculations...
                }
                if(m1-1>=0)
                {
                    for(int y = m1-2; y>=0; y--)
                    {
                        for(int row = 0; row<nrow; row++)
                        {
                            if(arr1[row][y] != '-')
                            {
                                replacementcost++;
                            }
                        }
                    }
                    
                }
                
                
                //end of for loop for replacement
                //replacementcost += mem[m1][m2-1]; //added past costs...
                
                mem[m1][m2] = (replacementcost<reconstruct) ? replacementcost : reconstruct;
                mem[m1][m2] = (basiccase<mem[m1][m2]) ? basiccase : mem[m1][m2];
                
                if(isReorder)
                {
                    mem[m1][m2] = (reorderingcost < mem[m1][m2]) ? reorderingcost : mem[m1][m2];
                }
            }
            
            else  ///m2 is NOT EQUAL to 1...
            {
            ///ADDING LAST LINE ONTO EXISTING ARRAY///
            basiccase = mem[m1][m2-1]+(mem[0][m2]-mem[0][m2-1]); //if only adding the last colmn
            basicvertical = mem[m1-1][m2]+(mem[m1][0]-mem[m1-1][0]); 
            ///DELETING ALL AND CONSTRUCTING ALL AGAIN///
            reconstruct = mem[m1][0]+mem[0][m2]; //hepsini sil hepsini bastan yarat
            ////REORDERING///
            int isReorder = 1;
            int check = 0;
            char checkarray[nrow];
            int checkindex = 0;
            int lastelmt = 0;
            int checkindex2 = 0;
            for(checkindex = 0; checkindex<nrow; checkindex++) //we created an array from arr1's related column.
            {
                if(arr1[checkindex][m1-1] != '-')
                {
                    checkarray[lastelmt] = arr1[checkindex][m1-1];
                    lastelmt++;
                }
            }
            //lastelmt--; //we increased for another elmt but that doesn't exist. so it points somewhere garbage.
            for(checkindex2 = 0; checkindex2<nrow; checkindex2++)
            {
                if(arr2[checkindex2][m2-1] != '-')
                {
                    check = 0;
                    for(int row = 0; row<lastelmt; row++)
                    {
                        if(arr2[checkindex2][m2-1] == checkarray[row]) // in case elemnt exists in arr1's related colmn:
                        {
                            checkarray[row] = '$';
                            check = 1;
                            break;
                        }

                    }
                    if(check == 0)
                    {
                        isReorder = 0;
                    }
                }
            }
            for(int row = 0; row<lastelmt; row++)
            {
                if(checkarray[row] != '$')
                {
                    isReorder = 0;
                }
            }
            
            if(isReorder) //calculated reordering cost...in case its possible
            {
                for(int row2 = 0; row2<nrow; row2++)
                {
                    if(arr1[row2][m1-1] != arr2[row2][m2-1])
                    {
                        reorderingcost++;
                    }
                }
                reorderingcost += mem[m1-1][m2-1]; //with past costs...
            }
            /////REPLACEMENT////
            for(int row = 0; row<nrow; row++) //calculated cost of replacement
            {
                if(arr1[row][m1-1] != arr2[row][m2-1]) //cost is only when they are inequal
                {
                    //two choices - vs a letter OR a letter vs a letter
                    if(arr1[row][m1-1] == '-' || arr2[row][m2-1] == '-') //if one of them = '-'
                    {
                        replacementcost += 2;
                    }
                    else //none of them = '-' both letters.
                    {
                        replacementcost++;
                    }
                }//calculated the replacement for last colmn. now add prev calculations...
            }//end of for loop for replacement
            replacementcost += mem[m1-1][m2-1]; //added past costs...
            mem[m1][m2] = (basiccase<reconstruct) ? basiccase : reconstruct;
            mem[m1][m2] = (basicvertical < mem[m1][m2]) ? basicvertical : mem[m1][m2];
            mem[m1][m2] = (replacementcost<mem[m1][m2]) ? replacementcost : mem[m1][m2];
            if(isReorder)
            {
                mem[m1][m2] = (reorderingcost<mem[m1][m2]) ? reorderingcost : mem[m1][m2];
            }
        }
        }
    }
    return mem[ncol1][ncol2]; // nrow = both of their rows ncol2=col count of the second one
}
