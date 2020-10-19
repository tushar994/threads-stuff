#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <time.h> 
#include <string.h>

int number;
int n,m,o;
int total_students;
int waiting_students = 0;
pthread_mutex_t mutex_waiting_student;

int min_int(int a,int b,int c){
    if(a<b){
        if(a<c){
            return a;
        }
        else{
            if(c<b){
                return c;
            }
            else{ return b;}
        }
    }
    else{
        if(b<c){
            return b;
        }
        else{
            return c;
        }
    }
}



struct pharma{
    int id;
    float probability;
    int vac_per_batch[5];
    int vaccines_in_circulation;
    struct pharma* next;
    pthread_mutex_t mutex;
};

struct zone{
    int slots[10];
    float prob;
    int id;
    int number_vaccines_left;
    int slots_left;
    struct zone* next;
    pthread_mutex_t mutex;
};

struct student{
    int id;
    int no_of_tries;
    int anti_bodies;
    struct student* next;
};
struct student* first_student;
struct pharma* first_pharma;
struct zone* first_zone;



void* pharma_func(void* inp){
    // to put condition to exit here
    struct pharma* input = (struct pharma*) inp;
    // int number_of_i=2;
    while(total_students>0){
        
        int number_of_batches = rand();
        number_of_batches = number_of_batches%5 + 1;
        char string[1000];
        sprintf(string,"Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\n\n",input->id,number_of_batches,input->probability);
        write(STDOUT_FILENO, string, strlen(string));
        sleep(1);
        int wait_time = rand();
        wait_time = wait_time%4 + 2;
        if(total_students==0){
            break;
        }
        sleep(wait_time);
        if(total_students==0){
            break;
        }
        int sum =0;
        pthread_mutex_lock(&(input->mutex));
        for(int i=0;i<number_of_batches;i++){
            // to put random number here
            int vac_no = rand();
            vac_no = vac_no%11 + 10;
            input->vac_per_batch[i] = vac_no;
            input->vaccines_in_circulation+=vac_no;
        }
        for(int i=number_of_batches;i<5;i++){
            input->vac_per_batch[i] = 0;
        }
        sprintf(string,"Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %f.Waiting for all the vaccines to be used to resume production\n\n",input->id,number_of_batches,input->probability);
        write(STDOUT_FILENO, string, strlen(string));
        // sleep(1);
        pthread_mutex_unlock(&(input->mutex));


        while(input->vaccines_in_circulation!=0 && total_students>0){}
        if(total_students==0){
            break;
        }
        pthread_mutex_lock(&(input->mutex));
        sprintf(string,"All the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now\n\n",input->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_unlock(&(input->mutex));
        // sleep(1);
    }
    return NULL;
}



void* stud_func(void* inp){
    char string[1000];
    struct student* input = (struct student*)inp;
    // wait for him to join
    int wait_gate = rand();
    wait_gate = wait_gate%6;
    sleep(wait_gate);
    sprintf(string,"Student %d has arrived at the gate\n",input->id);
    write(STDOUT_FILENO,string,strlen(string));

    // now that he has joined, make him try to do this thang thrice

    while(input->no_of_tries<3 && input->anti_bodies==0){
        char turn[4];
        if(input->no_of_tries==0){
            turn[0] = '1';
            turn[1] = 's';
            turn[2] = 't';
            turn[3]='\0';
        }
        else if(input->no_of_tries==1){
            turn[0]='2';
            turn[1]='n';
            turn[2]='d';
            turn[3]='\0';
        }
        else{
            turn[0]='3';
            turn[1]='r';
            turn[2]='d';
            turn[3]='\0';
        }
        sprintf(string,"Student %d has arrived for his %s round of vaccination \n\n",input->id,turn);
        write(STDOUT_FILENO,string,strlen(string));
        sprintf(string,"Student %d is waiting to be allocated a slot on a Vaccination Zone\n\n",input->id);
        write(STDOUT_FILENO,string,strlen(string));
        pthread_mutex_lock(&mutex_waiting_student);
        waiting_students++;
        pthread_mutex_unlock(&mutex_waiting_student);

        // we loop through all the zones in a while loop, until the student gets a slot
        int got_slot = 1;
        struct zone* slot_zone;
        float prob;
        int init = 0;
        int index_to_edit=-1;
        while(got_slot){
            // when he ggets a slot, we decrease the number of aiting students by one and the number of availabble slots by one
            struct zone* iter = first_zone->next;
            while(iter!=NULL && got_slot){
                // printf("student %d looks at zone %d\n",input->id,iter->id);
                pthread_mutex_lock(&(iter->mutex));
                if(iter->slots_left>0){
                    init = iter->slots_left;
                    iter->slots_left--;
                    index_to_edit = iter->slots_left;
                    got_slot=0;
                    slot_zone=iter;
                    prob = iter->prob;
                    sprintf(string,"Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n\n",input->id,iter->id);
                    write(STDOUT_FILENO,string,strlen(string));
                    pthread_mutex_unlock(&(iter->mutex));
                    break;
                }
                pthread_mutex_unlock(&(iter->mutex));
                iter=iter->next;
            }
        }
        pthread_mutex_lock(&mutex_waiting_student);
        waiting_students--;
        pthread_mutex_unlock(&mutex_waiting_student);

        // we will wait for the number of available slots to become -1,
        while(slot_zone->slots_left!=-1){
            // printf("student %d is stuck waiting for zone %d that had %d slots and has %d slots\n",input->id,slot_zone->id,init,slot_zone->slots_left);
        }
        slot_zone->slots[index_to_edit]=1;
        // once that happens, we'll see if he got vaccinated.
        // printf("rand mac %d\n",RAND_MAX);
        float immune = (float)rand()/(float)RAND_MAX;
        sprintf(string,"Student %d on Vaccination Zone %d has been vaccinated which has success probability %f\n\n",input->id,slot_zone->id,prob);
        write(STDOUT_FILENO,string,strlen(string));
        
        if(immune<=prob){
            // he is immune
            sprintf(string,"Student %d has tested positive for antibodies.\n\n",input->id);
            write(STDOUT_FILENO,string,strlen(string));
            input->anti_bodies=1;
            input->no_of_tries++;
        }
        else{
            // he has to try again
            sprintf(string,"Student %d has tested negative for antibodies.\n\n",input->id);
            write(STDOUT_FILENO,string,strlen(string));
            input->no_of_tries++;
        }

    }
    pthread_mutex_lock(&mutex_waiting_student);
    total_students--;
    pthread_mutex_unlock(&mutex_waiting_student);
    return NULL;
}



void* zone_func(void* inp){
    // printf("bruh\n");
    // to put exit condition
    struct zone* input = (struct zone*)inp;
    char string[1000];
    // int number_of_i=5;
    while(total_students>0){
        // to search for batches
        
        int found = 0;
        struct pharma* used_from;
        // we search for a batch to take from
        while(input->number_vaccines_left==0 && total_students>0){
            
            struct pharma* iter = first_pharma;
            iter=iter->next;
            while(iter!=NULL && input->number_vaccines_left==0 && total_students>0){
                // to lock with mutex
                pthread_mutex_lock(&(iter->mutex));

                for(int i=0;i<5;i++){
                    if(iter->vac_per_batch[i]!=0){
                        input->number_vaccines_left=iter->vac_per_batch[i];
                        iter->vac_per_batch[i]=0;
                        input->prob = iter->probability;
                        sprintf(string, "Pharmaceutical Company %d has delivered vaccines to Vaccination zone %d, resuming vaccinations now\n\n",iter->id,input->id);
                        write(STDOUT_FILENO,string,strlen(string));
                        // sleep(1);
                        used_from=iter;
                        break;
                    }
                }
                pthread_mutex_unlock(&(iter->mutex));
                iter = iter->next;
            }
        }
        // while you have vaccines in your zone you do this.
        while(input->number_vaccines_left>0 && total_students>0){
            while(waiting_students==0 && total_students>0){}
            if(total_students==0){
                break;
            }
            // to create slots
            for(int i=0;i<10;i++){
                input->slots[i]=0;
            }
            int slots = rand();
            int top = min_int(8,input->number_vaccines_left, waiting_students);
            // printf("top : %d\n",top);
            slots = slots%top + 1;
            input->slots_left = slots;
            sprintf(string,"Vaccination Zone %d is ready to vaccinate with %d slots\n\n",input->id,slots);
            write(STDOUT_FILENO,string,strlen(string));
            // sleep(1);
            // wait for slots to become empty or no students are waiting
            while(input->slots_left!=0 && !(waiting_students==0 && input->slots_left<slots ) && total_students>0){
                // printf("zone %d is waiting with slots left %d waiting tsudents %d and slots %d and total students %d\n",input->id,input->slots_left,waiting_students,slots, total_students);
            }
            if(total_students==0){
                break;
            }
            // doing vaccination
            slots-=input->slots_left;
            input->number_vaccines_left-=slots;
            input->slots_left=0;
            sprintf(string,"Vaccination Zone %d entering Vaccination Phase\n\n",input->id);
            write(STDOUT_FILENO,string,strlen(string));
            input->slots_left=-1;
            int sum =0;
            while(sum<slots){
                sum =0;
                for(int i=0;i<10;i++){
                    sum+=input->slots[i];
                }
            }
            // sleep(1);
            pthread_mutex_lock(&(used_from->mutex));
            used_from->vaccines_in_circulation-=slots;
            // printf("vaccines left in company %d is %d\n",used_from->id,used_from->vaccines_in_circulation);
            if(used_from->vaccines_in_circulation==0){
                sprintf(string,"Vaccination Zone %d has run out of vaccines\n\n",input->id);
                write(STDOUT_FILENO,string,strlen(string));
            }
            pthread_mutex_unlock(&(used_from->mutex));
        }
        // sleep(1);
    
    }
    return NULL;

    
}


int main(){
    srand(time(NULL));
    pthread_mutex_init(&(mutex_waiting_student), NULL);
    scanf("%d %d %d",&n,&m,&o);
    total_students =o;
    first_zone = (struct zone*)malloc(sizeof(struct zone));
    first_zone->slots_left=-1;
    first_zone->number_vaccines_left=-1;
    first_zone->next = NULL;
    first_zone->id = -1;
    first_pharma = (struct pharma*)malloc(sizeof(struct pharma));
    first_pharma->probability=-1;
    first_pharma->next = NULL;
    first_pharma->id = -1;
    first_student = (struct student*)malloc(sizeof(struct student));
    first_student->no_of_tries=-1;
    first_student->anti_bodies=1;
    first_student->next = NULL;
    first_student->id=-1;

    

    struct pharma* latest = first_pharma;
    for(int i=0;i<n;i++){
        float prob; 
        scanf("%f",&prob);
        struct pharma* new;
        new = (struct pharma*)malloc(sizeof(struct pharma));
        new->next = NULL;
        new->probability = prob;
        for(int i=0;i<5;i++){
            new->vac_per_batch[i] =0;
        }
        new->vaccines_in_circulation=0;
        new->id = i+1;
        latest->next = new;
        latest= new;
    }

    if(n==0 || m==0){
        printf("Simulation Over\n");
        return 0;
    }

    struct zone* last = first_zone;
    for(int i=0;i<m;i++){
        struct zone* new;
        new = (struct zone*)malloc(sizeof(struct zone));
        new->next=NULL;
        new->slots_left=0;
        new->number_vaccines_left=0;
        new->id = i+1;
        last->next = new;
        last = new;
    }
    struct student* late = first_student;
    for(int i=0;i<o;i++){
        struct student* new;
        new = (struct student*)malloc(sizeof(struct student));
        new->next=NULL;
        new->no_of_tries=0;
        new->anti_bodies=0;
        new->id=i+1;
        late->next=new;
        late=new;
    }

    pthread_t student_tid[o];
    pthread_t zone_tid[m];
    pthread_t pharma_tid[n];

    latest = first_pharma->next;
    for(int i=0;i<n;i++){

        pthread_mutex_init(&(latest->mutex), NULL);
        pthread_create(&pharma_tid[i], NULL, pharma_func, (void*)latest);
        latest = latest->next;
    }

    last = first_zone;
    last=last->next;
    for(int i=0;i<m;i++){
        pthread_mutex_init(&(last->mutex), NULL);
        pthread_create(&zone_tid[i], NULL, zone_func, (void*)last);
        last = last->next;
    }

    late = first_student->next;
    for(int i=0;i<o;i++){
        pthread_create(&student_tid[i], NULL, stud_func, (void*)late);
        late = late->next;
    }


    // we wait for all threads to end
    for(int i=0;i<m;i++){
        pthread_join(zone_tid[i], NULL);
    }
    for(int i=0;i<n;i++){
        pthread_join(pharma_tid[i], NULL);
    }
    for(int i=0;i<o;i++){
        pthread_join(student_tid[i], NULL);
    }
    

    // we detroy all mutexs
    pthread_mutex_destroy(&(mutex_waiting_student));

    latest = first_pharma->next;
    for(int i=0;i<n;i++){
        pthread_mutex_destroy(&(latest->mutex));
        latest=latest->next;
    }

    last = first_zone;
    last=last->next;
    for(int i=0;i<m;i++){
        pthread_mutex_destroy(&(last->mutex));
        last = last->next;
    }


    printf("Simulation Over\n\n");

}