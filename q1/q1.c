#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <time.h> 

struct thread_struct{
    int* arr;
    int start_index;
    int end_index;
};

void* thread_sort(void* inp){
    struct thread_struct* input = (struct thread_struct*) inp;
    int start_index = input->start_index;
    int end_index = input->end_index;
    if(((end_index) - (start_index))<4){
        // printf("startindex is %d and endindex is %d\n",start_index,end_index);
        int size = (end_index)-(start_index);
        // printf("size is %d\n",size);
        
        for(int i=0;i<size+1;i++){
            int min = input->arr[i+(start_index)];
            int cur_index = i+(start_index);
            for(int j=i;j<size+1;j++){
                if(min>input->arr[j+(start_index)]){
                    min = input->arr[j+(start_index)];
                    cur_index = j + (start_index);
                }
            }
            int temp = min;
            input->arr[cur_index]=input->arr[i+start_index];
            input->arr[i+start_index] = temp;
        }
        return 0;
    }
    else{
        int mid = ((start_index)+end_index)/2;

        pthread_t tid1,tid2;
        struct thread_struct* thread1 = (struct thread_struct*)malloc(sizeof(struct thread_struct));
        struct thread_struct* thread2 = (struct thread_struct*)malloc(sizeof(struct thread_struct));
        thread1->arr = input->arr;
        thread1->start_index = start_index;
        thread1->end_index = mid;

        thread2->arr = input->arr;
        thread2->start_index = mid+1;
        thread2->end_index = end_index;

        pthread_create(&tid1, NULL, thread_sort, (void*)(thread1));
        pthread_create(&tid2, NULL, thread_sort, (void*)(thread2));
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        // mergesort_nor(input->arr, (start_index), mid);
        // mergesort_nor(input->arr, (mid) + 1 , end_index);
        int init_arry[mid - (start_index - 1)];
        int sec_arry[end_index - (mid)];
        for(int i=start_index; i<= mid;i++){
            // printf("i:%d ",i);
            init_arry[i-(start_index)] = input->arr[i];
        }
        // printf("\n");
        // for(int i=start_index; i<= mid;i++){
        //     printf("%d ",init_arry[i-start_index]);
        // }
        // printf("\n");
        for(int i= mid + 1;i<=end_index ;i++){
            // printf("i:%d ",i);
            sec_arry[i- (mid + 1)] = input->arr[i];
        }
        // printf("\n");
        // for(int i= mid + 1;i<=end_index ;i++){
        //     printf("%d ",sec_arry[i- (mid + 1)]);
        // }
        // printf("\n");
        int init_index = 0;
        int sec_index = 0;
        int total = start_index;
        while(init_index<=(mid-(start_index)) && sec_index<=(end_index - (mid + 1)) ){
            if(init_arry[init_index] < sec_arry[sec_index]){
                input->arr[total] = init_arry[init_index];
                init_index++;
            }
            else{
                input->arr[total] = sec_arry[sec_index];
                sec_index++;
            }
            total++;
        }
        if(init_index>(mid-(start_index))){
            while(sec_index<=(end_index - (mid + 1))){
                input->arr[total] = sec_arry[sec_index];
                sec_index++;
                total++;
            }
        }
        else{
            while(init_index<=(mid-(start_index))){
                input->arr[total] = init_arry[init_index];
                init_index++;
                total++;
            }
        }
        return 0;
    }
}


int mergesort_nor(int* arr, int start_index, int end_index){
    if((end_index - start_index)<4){
        int size = end_index-start_index;
        for(int i=0;i<size+1;i++){
            int min = arr[i+start_index];
            int cur_index = i+start_index;
            for(int j=i;j<size+1;j++){
                if(min>arr[j+start_index]){
                    min = arr[j+start_index];
                    cur_index = j + start_index;
                }
            }
            int temp = min;
            arr[cur_index]=arr[i+start_index];
            arr[i+start_index] = temp;
        }
        return 0;
    }
    else{
        // printf("made\n");
        int mid = (start_index+end_index)/2;
        mergesort_nor(arr, start_index, mid);
        mergesort_nor(arr, (mid) + 1 , end_index);
        int init_arry[mid - (start_index - 1)];
        int sec_arry[end_index - (mid)];
        for(int i=start_index; i<= mid;i++){
            // printf("i:%d ",i);
            init_arry[i-start_index] = arr[i];
        }
        // printf("\n");
        // for(int i=start_index; i<= mid;i++){
        //     printf("%d ",init_arry[i-start_index]);
        // }
        // printf("\n");
        for(int i= mid + 1;i<=end_index ;i++){
            // printf("i:%d ",i);
            sec_arry[i- (mid + 1)] = arr[i];
        }
        // printf("\n");
        // for(int i= mid + 1;i<=end_index ;i++){
        //     printf("%d ",sec_arry[i- (mid + 1)]);
        // }
        // printf("\n");
        int init_index = 0;
        int sec_index = 0;
        int total = start_index;
        while(init_index<=(mid-start_index) && sec_index<=(end_index - (mid + 1)) ){
            if(init_arry[init_index] < sec_arry[sec_index]){
                arr[total] = init_arry[init_index];
                init_index++;
            }
            else{
                arr[total] = sec_arry[sec_index];
                sec_index++;
            }
            total++;
        }
        if(init_index>(mid-start_index)){
            while(sec_index<=(end_index - (mid + 1))){
                arr[total] = sec_arry[sec_index];
                sec_index++;
                total++;
            }
        }
        else{
            while(init_index<=(mid-start_index)){
                arr[total] = init_arry[init_index];
                init_index++;
                total++;
            }
        }
        return 0;
    }

}

int process_merge_sort(int* arr, int start_index, int end_index, int* fd, int first){
    if((end_index-start_index)<4){
        int size = end_index-start_index;
        for(int i=0;i<size+1;i++){
            int min = arr[i+start_index];
            int cur_index = i+start_index;
            for(int j=i;j<size+1;j++){
                if(min>arr[j+start_index]){
                    min = arr[j+start_index];
                    cur_index = j + start_index;
                }
            }
            int temp = min;
            arr[cur_index]=arr[i+start_index];
            arr[i+start_index] = temp;
        }
        if(first){
            write(fd[1], arr+start_index, ((size+1)*sizeof(int)));
        }
        // printf("in the bottom with statr_index = %d\n",start_index);
        // for(int i=0;i<size+1;i++){
            // printf("%d ",arr[i+start_index]);
        // }
        // printf("\n");
        return 0;
    }
    else{
        // printf("here\n");
        int mid = (start_index+end_index)/2;

        int fd1[2],fd2[2];
        pipe(fd1);
        pipe(fd2);

        // first fork
        int forkreturn1 = fork();
        // if(forkreturn1<0){
        //     printf("error in forking exitting entire thing\n");
        // }
        if(forkreturn1==0){
            close(fd1[0]);
            close(fd2[0]);
            close(fd2[1]);
            process_merge_sort(arr, start_index, mid, fd1,1 );
            close(fd1[1]);
            // printf("exitting with start: %d and end: %d\n",start_index,mid);
            exit(0);
        }
        
        int forkreturn2=fork();
        // if(forkreturn2<0){
        //     printf("error in forking\n");
        // }
        if(forkreturn2==0){
            close(fd2[0]);
            close(fd1[0]);
            close(fd1[1]);
            process_merge_sort(arr, (mid) + 1 , end_index, fd2,1);
            close(fd2[1]);
            // printf("exitting with start: %d and end: %d\n",mid+1,end_index);
            exit(0);
        }

        //wait for both to end
        int status;
        waitpid(-1, &status, 0);
        waitpid(-1, &status, 0);
        // printf("continuing with start: %d and end: %d\n",start_index,end_index);
        

        int init_arry[mid - (start_index - 1)];
        int sec_arry[end_index - (mid)];
        


        // printf("reached\n");
        read(fd1[0], init_arry, sizeof(init_arry));
        read(fd2[0], sec_arry, sizeof(sec_arry));
        close(fd1[1]);
        close(fd2[1]);
        close(fd1[0]);
        close(fd2[0]);
        
        // printf("first\n");
        // for(int i=start_index; i<= mid;i++){
        //     // printf("i: %d ",i-start_index);
        //     printf("%d ",init_arry[i-start_index]);
        // }
        // printf("\n");
        // printf("second\n");
        // for(int i= mid + 1;i<=end_index ;i++){
        //     // printf("i: %d ",i-(mid+1));
        //     printf("%d ",sec_arry[i- (mid + 1)]);
        // }
        // printf("\n");
        
        int init_index = 0;
        int sec_index = 0;
        int total = start_index;
        while(init_index<=(mid-start_index) && sec_index<=(end_index - (mid + 1)) ){
            // printf("1\n");
            if(init_arry[init_index] < sec_arry[sec_index]){
                arr[total] = init_arry[init_index];
                init_index++;
            }
            else{
                arr[total] = sec_arry[sec_index];
                sec_index++;
            }
            total++;
        }
        if(init_index>(mid-start_index)){
            while(sec_index<=(end_index - (mid + 1))){
                arr[total] = sec_arry[sec_index];
                sec_index++;
                total++;
            }
        }
        else{
            while(init_index<=(mid-start_index)){
                arr[total] = init_arry[init_index];
                init_index++;
                total++;
            }
        }
        if(first){
            write(fd[1],arr+start_index,(((end_index+1)-start_index)*sizeof(int)));
        }
        return 0;
    }
}



int main(){
    int n;
    struct timespec ts;
    scanf("%d",&n);
    int arr[n];
    for(int i=0;i<n;i++){
        scanf("%d",&arr[i]);
    }
    int normal_arr[n],fork_array[n],thread_array[n];
    int fd[2];
    for(int i=0;i<n;i++){
        normal_arr[i]=arr[i];
        fork_array[i]=arr[i];
        thread_array[i] = arr[i];
    }
    // clock_t t; 
    // t = clock();
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
    process_merge_sort(fork_array,0,n-1,fd,0);
    // t=clock() - t;
    printf("result of the forked merge sort is \n");
    for(int i=0;i<n;i++){
        printf("%d ",fork_array[i]);
    }
    printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);



    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    mergesort_nor(normal_arr,0,n-1);
    printf("result of the normal merge sort is \n");
    for(int i=0;i<n;i++){
        printf("%d ",normal_arr[i]);
    }
    printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);



    struct thread_struct* thread_start =  (struct thread_struct*)malloc(sizeof(struct thread_struct)); 
    thread_start->start_index=0;
    thread_start->end_index = n-1;
    thread_start->arr = &thread_array[0];
    pthread_t tid;
    // t = clock();
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    pthread_create(&tid, NULL, thread_sort, (void*)(thread_start));
    pthread_join(tid, NULL);
    // t=clock() - t;
    printf("result of the thread merge sort is \n");
    for(int i=0;i<n;i++){
        printf("%d ",thread_array[i]);
    }
    printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    // printf("the time taken by it was : %f seconds\n",((double)t)/CLOCKS_PER_SEC);
}
