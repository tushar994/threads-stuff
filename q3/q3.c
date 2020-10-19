#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <time.h> 
#include <string.h>
#include <errno.h>

int k,a,e,c,t1,t2,t;
int errno;
sem_t sem_acoustic;
sem_t sem_electric;
sem_t sem_sing_pls;
sem_t sem_coordin; 


struct player{
    int id;
    char name[100];
    char instrument;
    int arives;
    struct player* next;
    pthread_mutex_t mutex;
    int common_var;
};
struct player* first_player;

struct stage{
    char type;
    struct stage* next;
    char name_of_instru[100];
    char name_of_singer[100];
    int is_singer;
    int is_instru;
    int id;
    pthread_mutex_t mutex;
};
// very first one, go to next to get first actual one
struct stage* first_stage_a;
// very first one, go to the next to get to electric one
struct stage* first_stage_e;

void* get_tshirt(void* inp){
    struct player* input = (struct player*)inp;
    sem_wait(&sem_coordin);
    char string[1000];
    ////printf guy is collecting t-shirt
    sprintf(string,"\033[1;32m %s collecting t-shirt\033[0m\n\n",input->name);
    write(STDOUT_FILENO,string,strlen(string));
    sem_post(&sem_coordin);
    // print that 
}

void* singer_acoustic(void* inp){
    // get the input into something we can use
    struct player* input = (struct player*) inp;
    // do time wait where you wait for the person to get the shite
    char string[100];

    // we wait for accoustic to happen
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(&sem_acoustic, &ts);

    // edit commonvar accoridingly
    pthread_mutex_lock(&(input->mutex));
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            if(input->common_var==0 || input->common_var==-1 || input->common_var==1){
                // decrease to indicate that you failed here too (if applicable)
                if(input->common_var!=1){
                    input->common_var--;
                } 
                // exit normally
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            else if(input->common_var==-2){
                input->common_var=-3;
                sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
                write(STDOUT_FILENO,string,strlen(string)); 
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
    }
    // if someone else does 1 first then you leave
    else if(input->common_var==1){
        sem_post(&sem_acoustic);
        pthread_mutex_unlock(&(input->mutex));
        return NULL;
    }
    // else you are gonna perform in this bitch!!
    else{
        input->common_var=1;
    }
    pthread_mutex_unlock(&(input->mutex));
    
    
    // we are now performing!!!!!!!

    // declare that we are perfroming in a certain acoustic stadium
    struct stage* iter = first_stage_a->next;
    struct stage* performing_at;
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==0){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_singer=1;
            strcpy(iter->name_of_singer, input->name);
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        iter = iter->next;
    }

    // we declare that we are playing, and we play
    int play = rand();
    play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at acoustic %d for %d time \033[0m\n\n",input->name,input->instrument,performing_at->id,play);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    sleep(play);

    // we declare that we have ended our performance
    sprintf(string,"\033[1;36m %s performance at acoustic %d ended\033[0m\n\n",input->name,performing_at->id);
    write(STDOUT_FILENO,string,strlen(string));
    // we are done playing and now we leave it in the structure and we exit the semaphore
    pthread_mutex_lock(&(performing_at->mutex));
    performing_at->is_singer=0;
    performing_at->is_instru=0;
    pthread_mutex_unlock(&(performing_at->mutex));
    sem_post(&sem_acoustic);


    // we call and wait for the thread that lets us get a t-shirt
    pthread_t t1;
    pthread_create(&t1,NULL,get_tshirt, inp);
    pthread_join(t1,NULL);
    return NULL;
}
void* singer_electric(void* inp){
    // get the input into something we can use
    struct player* input = (struct player*) inp;
    // do time wait where you wait for the person to get the shite
    char string[100];

    // we wait for electric to happen
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(&sem_electric, &ts);

    // edit commonvar accoridingly
    pthread_mutex_lock(&(input->mutex));
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            if(input->common_var==0 || input->common_var==-1 || input->common_var==1){
                // decrease to indicate that you failed here too (if applicable)
                if(input->common_var!=1){
                    input->common_var--;
                } 
                // exit normally
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            else if(input->common_var==-2){
                input->common_var=-3;
                sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
                write(STDOUT_FILENO,string,strlen(string)); 
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
    }
    // if someone else does 1 first then you leave
    else if(input->common_var==1){
        sem_post(&sem_electric);
        pthread_mutex_unlock(&(input->mutex));
        return NULL;
    }
    // else you are gonna perform in this bitch!!
    else{
        input->common_var=1;
    }
    pthread_mutex_unlock(&(input->mutex));
    
    
    // we are now performing!!!!!!!

    // declare that we are perfroming in a certain electric stadium
    struct stage* iter = first_stage_e->next;
    struct stage* performing_at;
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==0){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_singer=1;
            strcpy(iter->name_of_singer, input->name);
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        iter = iter->next;
    }

    // we declare that we are playing, and we play
    int play = rand();
    play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at electric %d for %d time \033[0m\n\n",input->name,input->instrument,performing_at->id,play);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    sleep(play);

    // we declare that we have ended our performance
    sprintf(string,"\033[1;36m %s performance at electric %d ended\033[0m\n\n",input->name,performing_at->id);
    write(STDOUT_FILENO,string,strlen(string));
    // we are done playing and now we leave it in the structure and we exit the semaphore
    pthread_mutex_lock(&(performing_at->mutex));
    performing_at->is_singer=0;
    performing_at->is_instru=0;
    pthread_mutex_unlock(&(performing_at->mutex));
    sem_post(&sem_electric);


    // we call and wait for the thread that lets us get a t-shirt
    pthread_t t1;
    pthread_create(&t1,NULL,get_tshirt, inp);
    pthread_join(t1,NULL);
    return NULL;
}
void* singer_sing_pls(void* inp){
    // get the input into something we can use
    struct player* input = (struct player*) inp;
    // do time wait where you wait for the person to get the shite
    char string[100];

    // we wait for free sing pls to happen
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(&sem_sing_pls, &ts);

    // edit commonvar accoridingly
    pthread_mutex_lock(&(input->mutex));
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            if(input->common_var==0 || input->common_var==-1 || input->common_var==1){
                // decrease to indicate that you failed here too (if applicable)
                if(input->common_var!=1){
                    input->common_var--;
                } 
                // exit normally
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            else if(input->common_var==-2){
                input->common_var=-3;
                sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
                write(STDOUT_FILENO,string,strlen(string)); 
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
    }
    // if someone else does 1 first then you leave
    else if(input->common_var==1){
        sem_post(&sem_sing_pls);
        pthread_mutex_unlock(&(input->mutex));
        return NULL;
    }
    // else you are gonna perform in this bitch!!
    else{
        input->common_var=1;
    }
    pthread_mutex_unlock(&(input->mutex));
    
    
    // we are now performing!!!!!!!

    // declare that we are perfroming in a certain electric stadium and after exhausting electric we look at acoustic stadiums
    struct stage* iter = first_stage_e->next;
    struct stage* performing_at;
    char type_of_stadium[100];
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==1){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_singer=1;
            strcpy(iter->name_of_singer, input->name);
            if(iter->type=='e'){
                strcpy(type_of_stadium,"electric");
            }
            else{
                strcpy(type_of_stadium,"acoustic");
            }
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        struct stage* prev = iter;
        iter = iter->next;
        // if we have exhusted electric we lok at acoustic
        if(iter==NULL && prev->type=='e'){
            iter = first_stage_a->next;
        }
    }
    


    // we declare that we are playing, and we play
    // int play = rand();
    // play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at %s %d with %s \033[0m\n\n",input->name,input->instrument,type_of_stadium,performing_at->id,performing_at->name_of_instru);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    // sleep(play);

    // we are done playing and now we leave it in the structure and we exit the semaphore
    // pthread_mutex_lock(&(performing_at->mutex));
    // performing_at->is_singer=0;
    // performing_at->is_instru=0;
    // pthread_mutex_unlock(&(performing_at->mutex));
    // sem_post(&sem_electric);

    // we declare that we have ended our performance
    // sprintf(string,"\033[1;36m %s performance at electric %d ended\033[0m\n\n",input->name,performing_at->id);
    // write(STDOUT_FILENO,string,strlen(string));

    // we call and wait for the thread that lets us get a t-shirt
    // pthread_t t1;
    // pthread_create(&t1,NULL,get_tshirt, inp);
    // pthread_join(t1,NULL);
    return NULL;
}

// this is the main singer function that calls the three threads that checks the stuff
void* singer_exec(void* inp){
    char string[1000];
    struct player* input = (struct player*) inp;
    // set common_var to 0
    input->common_var=0;

    // wait for arival and announce arival
    sleep(input->arives);
    sprintf(string," %s %c arrived \n\n",input->name, input->instrument);
    write(STDOUT_FILENO,string,strlen(string));

    // create threads for the thihngs needed
    pthread_t acoustic_tid, electric_tid, common_tid;
    pthread_create(&common_tid,NULL,singer_sing_pls,inp);
    pthread_create(&acoustic_tid,NULL,singer_acoustic,inp);
    pthread_create(&electric_tid,NULL,singer_electric,inp);
    pthread_join(acoustic_tid,NULL);
    pthread_join(electric_tid,NULL);
    pthread_join(common_tid,NULL);
    
}


void* double_instru_acoustic(void* inp){
    char string[1000];
    struct player* input = (struct player*) inp;


    // we wait for free acoustic to happen
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(&sem_acoustic, &ts);

    // edit commonvar accoridingly
    pthread_mutex_lock(&(input->mutex));
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            if(input->common_var==0 || input->common_var==1){
                // decrease to indicate that you failed here too (if applicable)
                if(input->common_var!=1){
                    input->common_var--;
                } 
                // exit normally
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            else if(input->common_var==-1){
                input->common_var=-2;
                sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
                write(STDOUT_FILENO,string,strlen(string)); 
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
    }
    // if someone else does 1 first then you leave
    else if(input->common_var==1){
        sem_post(&sem_acoustic);
        pthread_mutex_unlock(&(input->mutex));
        return NULL;
    }
    // else you are gonna perform in this bitch!!
    else{
        input->common_var=1;
    }
    pthread_mutex_unlock(&(input->mutex));
    
    
    // we are now performing!!!!!!!

    // assign musician to an acoustic stage
    // printf("%s is getting a stage\n\n",input->name);
    struct stage* iter = first_stage_a->next;
    struct stage* performing_at;
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==0){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_instru=1;
            strcpy(iter->name_of_instru, input->name);
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        iter = iter->next;
    }
    // printf("%s is HAS GOT a stage\n\n",input->name);
    // say there is available a slot for singers with him
    sem_post(&sem_sing_pls);
    // printf("%s has posted to sem sing pls\n\n",input->name);


    //sleep for the duration of performance
    int play = rand();
    play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at acoustic %d for %d time \033[0m\n\n",input->name,input->instrument,performing_at->id,play);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    sleep(play);
    // printf("%s is UPPP\n\n",input->name);

    // check if a singer joined
    if(performing_at->is_singer==1){
        // if he has a singer, then he doesnt do sem_wait(&sem_sing_pls)
        // printf("NO\n");
        sleep(2);
        char singer_name[100];
        // print that both of them performed
        sprintf(string,"\033[1;36m %s performance at acoustic %d ended\033[0m\n\n",input->name,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        sprintf(string,"\033[1;36m %s performance at acoustic %d ended\033[0m\n\n",performing_at->name_of_singer,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        // edit the data structure
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_singer=0;
        performing_at->is_instru=0;
        strcpy(singer_name,performing_at->name_of_singer);
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_post(&sem_acoustic);
        // call thread for both to get tshirt
        // wait for booth threads to end
        pthread_t t1,t2;
        struct player* singer_struct;
        singer_struct = (struct player*)malloc(sizeof(singer_struct));
        strcpy(singer_struct->name,singer_name);
        pthread_create(&t1,NULL,get_tshirt, inp);
        pthread_create(&t2,NULL,get_tshirt,(void*) singer_struct);
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        free(singer_struct);
        return NULL;
    }
    else{
        // printf("bruh\n");
        // prn that we are done
        sprintf(string,"\033[1;36m %s performance at acoustic %d ended\033[0m\n\n",input->name,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        // edit data structure
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_instru=0;
        performing_at->is_singer=0;
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_wait(&sem_sing_pls);
        sem_post(&sem_acoustic);
        // call thread for the guy to get tshirt
        pthread_t t1;
        pthread_create(&t1,NULL,get_tshirt,inp);
        pthread_join(t1,NULL);
        return NULL;
    }

}

void* double_instru_electric(void* inp){
    char string[1000];
    struct player* input = (struct player*) inp;


    // we wait for free acoustic to happen
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(&sem_electric, &ts);

    // edit commonvar accoridingly
    pthread_mutex_lock(&(input->mutex));
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            if(input->common_var==0 || input->common_var==1){
                // decrease to indicate that you failed here too (if applicable)
                if(input->common_var!=1){
                    input->common_var--;
                } 
                // exit normally
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            else if(input->common_var==-1){
                input->common_var=-2;
                sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
                write(STDOUT_FILENO,string,strlen(string)); 
                pthread_mutex_unlock(&(input->mutex));
                return NULL;
            }
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            pthread_mutex_unlock(&(input->mutex));
            return NULL;
        }
    }
    // if someone else does 1 first then you leave
    else if(input->common_var==1){
        sem_post(&sem_electric);
        pthread_mutex_unlock(&(input->mutex));
        return NULL;
    }
    // else you are gonna perform in this bitch!!
    else{
        input->common_var=1;
    }
    pthread_mutex_unlock(&(input->mutex));
    
    
    // we are now performing!!!!!!!

    // assign musician to an acoustic stage
    // printf("%s is getting a stage\n\n",input->name);
    struct stage* iter = first_stage_e->next;
    struct stage* performing_at;
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==0){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_instru=1;
            strcpy(iter->name_of_instru, input->name);
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        iter = iter->next;
    }
    // printf("%s is HAS GOT a stage\n\n",input->name);
    // say there is available a slot for singers with him
    sem_post(&sem_sing_pls);
    // printf("%s has posted to sem sing pls\n\n",input->name);


    //sleep for the duration of performance
    int play = rand();
    play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at electric %d for %d time \033[0m\n\n",input->name,input->instrument,performing_at->id,play);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    sleep(play);
    // printf("%s is UPPP\n\n",input->name);

    // check if a singer joined
    if(performing_at->is_singer==1){
        // if he has a singer, then he doesnt do sem_wait(&sem_sing_pls)
        // printf("NO\n");
        sleep(2);
        char singer_name[100];
        // print that both of them performed
        sprintf(string,"\033[1;36m %s performance at electric %d ended\033[0m\n\n",input->name,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        sprintf(string,"\033[1;36m %s performance at electric %d ended\033[0m\n\n",performing_at->name_of_singer,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_singer=0;
        performing_at->is_instru=0;
        strcpy(singer_name,performing_at->name_of_singer);
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_post(&sem_electric);
        // call thread for both to get tshirt
        // wait for booth threads to end
        pthread_t t1,t2;
        struct player* singer_struct;
        singer_struct = (struct player*)malloc(sizeof(singer_struct));
        strcpy(singer_struct->name,singer_name);
        pthread_create(&t1,NULL,get_tshirt, inp);
        pthread_create(&t2,NULL,get_tshirt,(void*) singer_struct);
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        free(singer_struct);
        return NULL;
    }
    else{
        // printf("bruh\n");
        sprintf(string,"\033[1;36m %s performance at electric %d ended\033[0m\n\n",input->name,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_instru=0;
        performing_at->is_singer=0;
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_wait(&sem_sing_pls);
        sem_post(&sem_electric);
        // call thread for the guy to get tshirt
        pthread_t t1;
        pthread_create(&t1,NULL,get_tshirt,inp);
        pthread_join(t1,NULL);
        return NULL;
    }
}


void* double_instru_exec(void* inp){
    char string[1000];
    struct player* input = (struct player*) inp;
    // set common_var to 0
    input->common_var=0;

    // wait for arival and announce arival
    sleep(input->arives);
    sprintf(string," %s %c arrived \n\n",input->name, input->instrument);
    write(STDOUT_FILENO,string,strlen(string));

    // create threads for the thihngs needed
    pthread_t acoustic_tid, electric_tid;
    pthread_create(&acoustic_tid,NULL,double_instru_acoustic,inp);
    pthread_create(&electric_tid,NULL,double_instru_electric,inp);
    pthread_join(acoustic_tid,NULL);
    pthread_join(electric_tid,NULL);
}




void* single_instru(void* inp){
    struct player* input = (struct player*) inp;

    char string[1000];

    sem_t* appr_sem;
    struct stage* appr_stage;
    char stage_type[100];

    if(input->instrument=='v'){
        strcpy(stage_type,"acoustic");
        appr_stage = first_stage_a;
        appr_sem = &sem_acoustic;
    }
    else if(input->instrument=='b'){
        strcpy(stage_type,"electric");
        appr_stage = first_stage_e;
        appr_sem = &sem_electric;   
    }

    sleep(input->arives);
    sprintf(string," %s %c arrived \n\n",input->name, input->instrument);
    write(STDOUT_FILENO,string,strlen(string));
    //  
    

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    int s = sem_timedwait(appr_sem, &ts);
    // printf("%s got past time\n\n",input->name);
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            sprintf(string,"\033[1;35m %s %c left because of impatience \033[0m\n\n",input->name, input->instrument);
            write(STDOUT_FILENO,string,strlen(string));
            return NULL;
        }
        else{
            perror("something has gone wrong");
            return NULL;
        }
    }

    // assign musician to a stage
    // printf("%s is getting a stage\n\n",input->name);
    struct stage* iter = appr_stage->next;
    struct stage* performing_at;
    while(iter!=NULL){
        // printf("%s is looking\n\n",input->name);
        pthread_mutex_lock(&(iter->mutex));
        if(iter->is_singer==0 && iter->is_instru==0){
            // printf("%s is bring assigned a stage\n\n",input->name);
            iter->is_instru=1;
            strcpy(iter->name_of_instru, input->name);
            pthread_mutex_unlock(&(iter->mutex));
            performing_at = iter;
            break;
        }
        else{
            // printf("%s failed in being assigned\n\n",input->name);
        }
        pthread_mutex_unlock(&(iter->mutex));
        iter = iter->next;
    }
    // printf("%s is HAS GOT a stage\n\n",input->name);
    // say there is available a slot for singers with him
    sem_post(&sem_sing_pls);
    // printf("%s has posted to sem sing pls\n\n",input->name);


    //sleep for the duration of performance
    int play = rand();
    play = play%(t2-t1 +1) + t1;
    // printf("%s has create randm wait\n\n",input->name);
    sprintf(string,"\033[01;33m %s is performing %c at %s %d for %d time \033[0m\n\n",input->name,input->instrument,stage_type,performing_at->id,play);
    write(STDOUT_FILENO,string,strlen(string));
    // printf("%s is sleeping\n\n",input->name);
    sleep(play);
    // printf("%s is UPPP\n\n",input->name);

    // check if a singer joined
    if(performing_at->is_singer==1){
        // if he has a singer, then he doesnt do sem_wait(&sem_sing_pls)
        // printf("NO\n");
        sleep(2);
        char singer_name[100];
        // print that both of them performed
        sprintf(string,"\033[1;36m %s performance at %s %d ended\033[0m\n\n",input->name,stage_type,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        sprintf(string,"\033[1;36m %s performance at %s %d ended\033[0m\n\n",performing_at->name_of_singer,stage_type,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_singer=0;
        performing_at->is_instru=0;
        strcpy(singer_name,performing_at->name_of_singer);
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_post(appr_sem);
        // call thread for both to get tshirt
        // wait for booth threads to end
        pthread_t t1,t2;
        struct player* singer_struct;
        singer_struct = (struct player*)malloc(sizeof(singer_struct));
        strcpy(singer_struct->name,singer_name);
        pthread_create(&t1,NULL,get_tshirt, inp);
        pthread_create(&t2,NULL,get_tshirt,(void*) singer_struct);
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        free(singer_struct);
        return NULL;
    }
    else{
        // printf("bruh\n");
        sprintf(string,"\033[1;36m %s performance at %s %d ended\033[0m\n\n",input->name,stage_type,performing_at->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_lock(&(performing_at->mutex));
        performing_at->is_instru=0;
        performing_at->is_singer=0;
        pthread_mutex_unlock(&(performing_at->mutex));
        sem_wait(&sem_sing_pls);
        sem_post(appr_sem);
        // call thread for the guy to get tshirt
        pthread_t t1;
        pthread_create(&t1,NULL,get_tshirt,inp);
        pthread_join(t1,NULL);
        return NULL;
    }


}





int main(){
    srand(time(NULL));
    scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
    // printf("time is %d\n",t);
    // initialize the semaphores by doing this
    sem_init(&sem_coordin, 0, c);
    sem_init(&sem_acoustic, 0, a);
    sem_init(&sem_sing_pls, 0, 0);
    sem_init(&sem_electric, 0, e);

    // create first stage for acoustic
    first_stage_a = (struct stage*)malloc(sizeof(struct stage));
    first_stage_a->type = '\0';
    first_stage_a->name_of_instru[0] = '\0';
    first_stage_a->name_of_singer[0] = '\0';
    first_stage_a->is_instru = -1;
    first_stage_a->is_singer = -1;
    first_stage_a->id = -1;

    // create first stage for electric
    first_stage_e = (struct stage*)malloc(sizeof(struct stage));
    first_stage_e->type = '\0';
    first_stage_e->name_of_instru[0] = '\0';
    first_stage_e->name_of_singer[0] = '\0';
    first_stage_e->is_instru = -1;
    first_stage_e->is_singer = -1;
    first_stage_e->id = -1;

    // create all real stage for acoustic
    struct stage* iter_stage_a = first_stage_a;
    for(int i=0;i<a;i++){
        struct stage* new = (struct stage*)malloc(sizeof(struct stage));
        new->type = 'a';
        new->name_of_instru[0] = '\0';
        new->name_of_singer[0] = '\0';
        new->is_instru = 0;
        new->is_singer = 0;
        new->id = i+1;
        new->next = NULL;
        iter_stage_a->next = new;
        iter_stage_a=iter_stage_a->next;
    }
    // create all real stage for electric
    struct stage* iter_stage_e = first_stage_e;
    for(int i=0;i<e;i++){
        struct stage* new = (struct stage*)malloc(sizeof(struct stage));
        new->type = 'e';
        new->name_of_instru[0] = '\0';
        new->name_of_singer[0] = '\0';
        new->is_instru = 0;
        new->is_singer = 0;
        new->id = i+a;
        new->next = NULL;
        iter_stage_e->next = new;
        iter_stage_e=iter_stage_e->next;
    }



    // create players
    first_player = (struct player*)malloc((sizeof(struct player)));
    first_player->arives = -1;
    first_player->name[0] = '\0';
    first_player->instrument='\0';
    first_player->next=NULL;
    first_player->id=-1;
    struct player* iter_player = first_player;
    for(int i=0;i<k;i++){
        struct player* new = (struct player*)malloc(sizeof(struct player));
        scanf("%s %c %d",(new->name),&(new->instrument),&(new->arives));
        new->next=NULL;
        new->id = i+1;
        iter_player->next=new;
        iter_player=iter_player->next;
    }

    

    // initialise the mutexs for stages
    iter_stage_a = first_stage_a;
    for(int i=0;i<a;i++){
        pthread_mutex_init(&(iter_stage_a->mutex), NULL);
        iter_stage_a=iter_stage_a->next;
    }
    iter_stage_e = first_stage_e;
    for(int i=0;i<e;i++){
        pthread_mutex_init(&(iter_stage_e->mutex), NULL);
        iter_stage_e=iter_stage_e->next;
    }


    // start the kid threads
    pthread_t player_tid[k];
    iter_player = first_player->next;
    for(int i=0;i<k;i++){
        if(iter_player->instrument=='v'|| iter_player->instrument=='b'){
            pthread_create(&player_tid[i],NULL,single_instru,(void*)iter_player);
        }
        else if(iter_player->instrument=='s'){
            pthread_mutex_init(&(iter_player->mutex), NULL);
            pthread_create(&player_tid[i],NULL,singer_exec,(void*)iter_player);
        }
        else if( iter_player->instrument=='p' || iter_player->instrument=='g'){
            pthread_mutex_init(&(iter_player->mutex), NULL);
            pthread_create(&player_tid[i],NULL,double_instru_exec,(void*)iter_player);
        }

        iter_player=iter_player->next;
    }
    iter_player = first_player->next;
    for(int i=0;i<k;i++){
        if(iter_player->instrument=='v'|| iter_player->instrument=='b' || iter_player->instrument=='s' || iter_player->instrument=='p' || iter_player->instrument=='g'){
            pthread_join(player_tid[i],NULL);
        }
        iter_player=iter_player->next;
    }
    iter_player = first_player->next;
    for(int i=0;i<k;i++){
        if(iter_player->instrument=='s' || iter_player->instrument=='p' || iter_player->instrument=='g'){
            pthread_mutex_destroy(&(iter_player->mutex));
        }
        iter_player=iter_player->next;
    }

    printf("\033[0;31m EXITTED \033[0m\n");

}