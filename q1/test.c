#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <sys/wait.h>

int main(){
    int sum=0;
    int arr[100];
    for(int i=0;i<100;i++){
        arr[i]=0;
    }
    for(int i=0;i<100;i++){
        int a;
        scanf("%d",&a);
        printf("%d\n",a);
        a--;
        if(arr[a]==0){
            arr[a]=1;
            sum++;
        }
    }
    for(int i=0;i<100;i++){
        if(arr[i]==0){
            printf("missing : %d\n",i+1);
        }
    }
    if(sum==100){
        printf("yaya\n");
    }
}