#include <stdio.h>
#include <stdlib.h>
#include "the3.h"
int strcompare(char* str1, char* str2)
{
    char* str1h, *str2h;
    int i;
    str1h = str1;
    str2h = str2;
    for(i = 0;; i++)
    {
        if(str2h[0]=='\0'&&str1h[0]=='\0')
        {
            return 1;
        }
        else if(str2h[0]==str1h[0])
        {
            str1h = str1h+1;
            str2h = str2h+1;
        }
        else
        {
            return 0;
        }
    }
}
Apartment* add_apartment(Apartment* head, int index, char* apartment_name, int max_bandwidth)
{
    Apartment* curr;
    Apartment* prev;
    Apartment* newapar;
    Apartment* lastap;
    int i, j;
    newapar = malloc(sizeof(Apartment));
    newapar->name = apartment_name;
    newapar->max_bandwidth = max_bandwidth;
    newapar->flat_list = NULL;
    newapar->next = NULL;
    curr = head;
    if (head==NULL)
    {
        newapar->next = newapar;
        head = newapar;
        return head;
    }
    for(j = 0; ; j++)
    {
        if(curr->next == head)
        {
            lastap = curr;
            curr = head;
            break;
        }
        else
        {
            curr = curr->next;
        }
    }

    if (index==0)
    {
        newapar->next = head;
        head = newapar;
        lastap->next = head;
        return head;
    }
    else
    {
        for(i = 0; i<index; i++)
        {
            prev = curr;
            curr = curr->next;
        }
        prev->next = newapar;
        newapar->next = curr;
        return head;
    }
}
void add_flat(Apartment* head, char* apartment_name, int index, int flat_id, int initial_bandwidth)
{
    Apartment* temphead;
    Flat* curr;
    Flat* prev;
    Flat* newflat;
    int i, j;
    newflat = malloc(sizeof(Flat));
    temphead = head;
    newflat->initial_bandwidth = initial_bandwidth;
    newflat->is_empty = 0;
    newflat->id = flat_id;

    for(i = 0; ;i++)
    {
        if(strcompare(temphead->name, apartment_name))
        {
            if(temphead->flat_list==NULL)
            {
                temphead->flat_list = newflat;
                if(initial_bandwidth <= temphead->max_bandwidth)
                {
                    temphead->max_bandwidth=initial_bandwidth;
                }
                newflat->next = NULL;
                newflat->prev = NULL;
            }
            curr = temphead->flat_list;
            if (index==0)
            {
                newflat->next = curr;
                curr->prev = newflat;
                temphead->flat_list = newflat;
            }
            else
            {
                for(j = 0;j<index;j++)
                {
                    prev = curr;
                    curr = curr->next;
                }
                prev->next = newflat;
                newflat->prev = prev;
                newflat->next = curr;
                curr->prev = newflat;
            }
            break;
        }
        temphead = temphead->next;
    }

}



Apartment* remove_apartment(Apartment* head, char* apartment_name)
{
    Apartment* curr;
    Apartment* prev;
    Apartment* lastap;
    int i, j;
    lastap = malloc(sizeof(Apartment));
    prev = malloc(sizeof(Apartment));
    curr = malloc(sizeof(Apartment));
    curr = head;

    if(head==NULL)
    {
        return NULL;
    }
    for(j = 0; ; j++)
    {
        if(curr->next == head)
        {
            lastap = curr;
            curr = head;
            break;
        }
        else
        {
            curr = curr->next;
        }
    }
    prev = lastap;
    for(i = 0; ;i++)
    {
        if(strcompare(curr->name,apartment_name))
        {
            if(i==0)
            {
                free(curr->flat_list);
                prev->next = curr->next;
                free(curr);
                return head->next;
            }

            free(curr->flat_list);
            prev->next = curr->next;
            free(curr);
            break;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
    return head;
}

void make_flat_empty(Apartment* head, char* apartment_name, int flat_id)
{
    Apartment* emptyapa;
    Flat* emptyfla;
    int i, j;
    emptyapa = head;
    for(i = 0; ; i++)
    {
        if(strcompare(emptyapa->name,apartment_name))
        {
            emptyfla = emptyapa->flat_list;
            for(j = 0;;j++)
            {
                if(emptyfla->id == flat_id)
                {
                    emptyfla->is_empty = 1;
                    emptyfla->initial_bandwidth = 0;
                    break;
                }
                else
                {
                    emptyfla = emptyfla->next;
                }
            }
            break;
        }
        else
        {
            emptyapa = emptyapa->next;
        }
    }

}

int find_sum_of_max_bandwidths(Apartment* head)
{
    Apartment* temp;
    int i, sum;
    temp = head;
    sum = 0;
    if(head->next == head)
    {
        return 0;
    }
    for(i = 0; ; i++)
    {
        if(temp->next != head)
        {
            sum+=temp->max_bandwidth;
            temp = temp->next;
        }
        else
        {
            sum+=temp->max_bandwidth;
            break;
        }
    }
    return sum;

}


Apartment* merge_two_apartments(Apartment* head, char* apartment_name_1, char* apartment_name_2)
{
    Apartment* temp1;
    Apartment* temp2;
    Apartment* lastap;
    Apartment* curr;
    Apartment* prev;
    Flat* flathead;
    Flat* flathead2;
    int i, j;
    flathead = malloc(sizeof(Flat));
    flathead2 = malloc(sizeof(Flat));
    temp1 = malloc(sizeof(Apartment));
    temp2 = malloc(sizeof(Apartment));
    curr = head;
    temp1=head;
    temp2=head;
    for(j = 0; ; j++)
    {
        if(curr->next == head)
        {
            lastap = curr;
            curr = head;
            break;
        }
        else
        {
            curr = curr->next;
        }
    }
    for(i = 0;;i++)
    {
        if(temp1->name[0]==apartment_name_1[0])
        {
            break;
        }
        else
        {
            temp1=temp1->next;
        }
    }
    for(j = 0;;j++)
    {
        if(temp2->name[0]==apartment_name_2[0])
        {
            if(j == 0)
            {
                lastap->next = head->next;
                head = head->next;
                break;
            }

            prev->next = temp2->next;
            break;
        }
        else
        {
            prev = temp2;
            temp2=temp2->next;
        }
    }
    temp1->max_bandwidth+=temp2->max_bandwidth;
    flathead = temp1->flat_list;
    flathead2 = temp2->flat_list;
    for(i = 0;;i++)
    {
        if(flathead->next==NULL)
        {
            break;
        }
        else
        {
            flathead = flathead->next;
        }
    }

    flathead->next = flathead2;
    flathead2->prev = flathead;
    free(temp2);
    return head;
}
void relocate_flats_to_same_apartment(Apartment* head, char* new_apartment_name, int flat_id_to_shift, int* flat_id_list)
{
    Apartment* temphead;
    Apartment* newap;
    Flat* flattomove;
    Flat* flatpre;
    Flat* temp;
    Flat* flatidcheck;
    int i, j, count, k;
    count = 0;
    temphead = head;
    for(i = 0;; i++)
    {
        if(strcompare(temphead->name, new_apartment_name))
        {
            newap = temphead;
            break;
        }
        else
        {
            temphead = temphead->next;
        }
    }
    flattomove = newap->flat_list;
    for(i = 0 ;; i++)
    {
        if(flattomove->id == flat_id_to_shift)
        {
            if(i == 0)
            {
                flatpre = NULL;
                break;
            }
            flatpre = flattomove->prev;
            break;
        }
        else
        {
            flattomove = flattomove->next;
        }
    }
    temphead = head;
    count = 2;
    for(k = 0;; k++)
    {
        for (i = 0;; i++)
        {
            flatidcheck = temphead->flat_list;
            if (count==1)
            {
                break;
            }
            if(count==0 && temphead == head)
            {
                break;
            }
            for (j = 0; flatidcheck != NULL; j++)
            {
                temp = flatidcheck->next;

                if (flatidcheck->id == flat_id_list[0])
                {
                    if (temphead->flat_list == flatidcheck)
                    {
                        temphead->flat_list = temphead->flat_list->next;
                        if (temphead->flat_list) temphead->flat_list->prev = NULL;
                    }
                    else
                    {
                        flatidcheck->prev->next = flatidcheck->next;
                        if (flatidcheck->next) flatidcheck->next->prev = flatidcheck->prev;
                    }
                    temphead->max_bandwidth -= flatidcheck->initial_bandwidth;
                    newap->max_bandwidth += flatidcheck->initial_bandwidth;
                    if (flatpre == NULL)
                    {
                        newap->flat_list = flatidcheck;
                    }
                    else
                    {
                        flatidcheck->prev = flatpre;
                        flatpre->next = flatidcheck;
                    }
                    flatidcheck->next = flattomove;
                    flattomove->prev = flatidcheck;
                    flatpre = flatidcheck;
                    flatidcheck = temp;
                    temphead = head;
                    count = 1;
                    break;


                }
                else
                {
                    flatidcheck = flatidcheck->next;
                    count = 0;
                }
            }

            temphead = temphead->next;
        }
        if(count==0 && temphead == head)
        {
            break;
        }
        flat_id_list = flat_id_list + 1;
        count = 2;
    }
}
