int okay = semaphore time wait (electric or acoutsic or Sm)

mutex_lock()
if(okay==-1){
    if(common_var==1){
        exit appropriately
    }
    else if(common_var==0){
        common_var=-1;
        exit appropriately
    }
    else if(common_var==-1){
        common_var=-2;
        exit appropriately
    }
    else if(common_var==-2){
        common_var=-3;
        exit with impatience
    }
    
}
else{
    if(common_var==0){
        common_var=1;
    }
    else if(common_var==1){
        exit the thing appropriately
    }
    else{
        common_var=1;
    }
}
mutex_unlock()

stage_mutex_lock()
edit stage accordingly
stage_mutex_unlock()


perform

semaphor_mutex_unlock



wait_sempahore_coordinator()

sleep(2)

unlock_semaphore_cooordinator()



