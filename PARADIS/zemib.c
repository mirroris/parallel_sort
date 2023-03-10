#ifndef ZEMIB_H
#define ZEMIB_H
#include "zemib.h"
#endif

#include<stdio.h>

typedef unsigned int  ui;
typedef long long ll;

extern int size, seed;
ui mod, digmax;
ui bucket[8][MOD]; /*    bucket level */

void msdRadixSort(ui *array, ui argmod){
    mod = argmod;
    ui max = findMax(array);
    digmax = 0;
    ui powMod = mod;
    while(max>mod){
        max/=mod;
        digmax++;
    }
    radixSort(array, digmax, 0, size);
    //display(array);
}

ui findMax(ui *array){
    ui max=0;
    for(int i=0;i<size;i++){
        if(max<array[i]) max = array[i];
    }
    return max;
}

void radixSort(ui *array, int l, int left, int right){
    //display(array, l, left, right);
    ui head[mod], tail[mod], index;
    ui *lbucket = bucket[l];
    /*  make bucket empty */
    for(int i=0;i<mod;i++)lbucket[i] = 0;
    /*  distribute array element to bucket  */
    ui shift = 28 - 4*l; /*  when mod=16, n'th digits of HEX begins at 4*(n-1)+1 bit of BIN : n = 8-l;*/
    for(int i=left;i<right;i++){
        /*  calculate l'th most significant digit to acindex with shift*/
        index = (array[i] << shift) >> 28;    // 28 means 32-4, which is 4 most significant digits of previous acindex   
        lbucket[index]++;
    }

    /*  calculate head, tail of the indexes of the bucket*/
    head[0] = left;
    tail[0] = left+lbucket[0]; 
    ui pocket = tail[0];
    for(int i=1;i<mod;i++){
        head[i] = pocket; 
        tail[i] = pocket + lbucket[i];
        pocket = tail[i];
    }

    /*  swap and permutate */
    for(int i=0;i<mod;i++){
        while(head[i]<tail[i]){
            ui v = array[head[i]];
            /*  calculate l'th most significant digit to acindex with shift*/
            index = (v << shift) >> 28;
            while(head[index]!=head[i]){
                ui tmp = array[head[index]]; 
                array[head[index]++] = v;
                v = tmp;
                index = (v << shift) >> 28;
            }
            array[head[i]++] = v;
        }
    }

    ui prevtail = left, curtail = left;
    if(l--!=0){
        for(int i=0;i<mod;i++) {
            curtail = tail[i];
            if(curtail > (prevtail+1)) radixSort(array, l, prevtail, curtail); //if curtail = prevtail call is not required
            prevtail = curtail;
        }
    }
    return;
}

void display(ui *array){
    printf("array =");
    for(int i=0;i<size;i++)printf("%u ",array[i]);
    printf("\n");
}

void msdParRadixSort(ui *array, ui argmod){
    mod = argmod;
    ui max = findMaxPar(array);
    digmax = 0;
    ui powMod = mod;
    while(max>mod){
        max/=mod;
        digmax++;
    }
    #pragma omp parallel num_threads(8)
    {   
        #pragma omp single 
        {
            paradis(array, digmax, 0, size);
        }
    }
    //display(array);
}

ui findMaxPar(ui *array){
    ui max;
    #pragma omp parallel for reduction(max : max)
    for(int i=0;i<size;i++){
        if(max<array[i]) max = array[i];
    }
    return max;
}

void paradis(ui *array, int l, int left, int right){
    //display(array, l, left, right);
    ui head[mod], tail[mod], index;
    ui lbucket[mod];
    /*  make bucket empty */
    for(int i=0;i<mod;i++)lbucket[i] = 0;
    /*  distribute array element to bucket  */
    ui shift = 28 - 4*l; /*  when mod=16, n'th digits of HEX begins at 4*(n-1)+1 bit of BIN : n = 8-l;*/
    //#pragma omp parallel for reduction(+:lbucket[:mod])
        for(int i=left;i<right;i++){
            /*  calculate l'th most significant digit to acindex with shift*/
            index = (array[i] << shift) >> 28;    // 28 means 32-4, which is 4 most significant digits of previous acindex   
            lbucket[index]++;
        }
    
    /*  calculate head, tail of the indexes of the bucket*/
    head[0] = left;
    tail[0] = left+lbucket[0]; 
    ui pocket = tail[0];
    for(int i=1;i<mod;i++){
        head[i] = pocket; 
        tail[i] = pocket + lbucket[i];
        pocket = tail[i];
    }

    /*  swap and permutate */
    for(int i=0;i<mod;i++){
        while(head[i]<tail[i]){
            ui v = array[head[i]];
            /*  calculate l'th most significant digit to acindex with shift*/
            index = (v << shift) >> 28;
            while(head[index]!=head[i]){
                ui tmp = array[head[index]]; 
                array[head[index]++] = v;
                v = tmp;
                index = (v << shift) >> 28;
            }
            array[head[i]++] = v;
        }
    }
    

    ui prevtail = left, curtail = left;
    if(l--!=0){
                for(int i=0;i<mod;i++) {
                    curtail = tail[i];
                    if(curtail > (prevtail+1)) {
                        #pragma omp task shared(array)
                            paradis(array, l, prevtail, curtail); //if curtail = prevtail call is not required
                    }
                    prevtail = curtail;
                }
            
    }
    return;
}

