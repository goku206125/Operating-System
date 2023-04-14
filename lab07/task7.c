#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING        1
#define HUNGRY          2
#define EATING          3

#define THINKING_TIME  0
#define HUNGRY_TIME    0
#define EATING_TIME    0
pthread_mutex_t	m = PTHREAD_MUTEX_INITIALIZER;		//initialized to 1
int	state[N];	        //initiated to THINKING's
pthread_mutex_t	s[N];		//initialized to 0's
int food[N];
void grab_forks( int i );
void put_away_forks( int i );
void test( int i );
bool meal_comp(int r);
void print_meals(int y){

    for (int y = 0; y < N; y++)
    {
        printf("Philosospher % d at %d .\n",y,food[y]);
    }
    
        
}
void *thread_fun(void *vargp)
{
    int *myid = (int *)vargp;
    printf("Philosopher %d is on the table.\n", *myid);
    while (1)
    {
        // Think
		printf("Philosopher %d is thinking...\n", *myid);
		sleep(THINKING_TIME);
		// Hungry
		grab_forks(*myid);
		// Eating
		printf("Philosopher %d is eating...\n", *myid);
		sleep(EATING_TIME);
		// Think again
		put_away_forks(*myid);
        printf("Philosopher %d ate ==> %d meals.\n",*myid,food[*myid]);
        //signal(SIGTERM, print_meals(*myid));
    }
    
    return NULL;

}

/////////////////////////////////////////////////
int main()
{
    //pthread_mutex_init(&m, 1);
    //pthread_mutex_init(&s, 0);

    for (int i = 0; i < N; i++)
    {
        pthread_mutex_lock(s+i);
    }
    //geeksforgeeks (allocating memory)
    int* ptr = (int*)malloc(N * sizeof(int));
    
    if (ptr == NULL) {
        printf("Memory not allocated.\n");
        _exit(0);
    }
    else {
 
        // Memory has been successfully allocated
        printf("Memory successfully allocated using malloc.\n");
    }
    pthread_t philosopher[N];

    for (int i = 0; i < N; i++)
    {
        ptr[i] = i;
        int create_check = pthread_create(&philosopher[i],NULL,thread_fun,(void *)&ptr[i]);
        if (create_check != 0)
        {
            printf("An error occured while creating threads.\n");
            _exit(0);
        }
        
    }
    
    sleep(HUNGRY_TIME);
    for(int i = 0; i < N; i++){
        pthread_join(philosopher[i], NULL);
    }
    pthread_mutex_destroy(&m);

    for(int i = 0; i < N; i++){
        pthread_mutex_destroy(&s[i]);
    }
    pthread_exit(NULL);
    return 0;
}

//////////////////////////////////////////////////////
void grab_forks( int i )
{
	pthread_mutex_lock( &m );
	state[i] = HUNGRY;
    printf("Philosopher %d is trying to GRAB forks.\n",i);
	test( i );
	pthread_mutex_unlock( &m );
	pthread_mutex_lock( &s[i] );
}

////////////////////////////////////////////////////
void put_away_forks( int i )
{
	pthread_mutex_lock( &m );
    food[i] = food[i] + 1;
	state[i] = THINKING;
    printf("Philosopher %d is putting AWAY forks.\n",i);
	test( LEFT );
	test( RIGHT );
	pthread_mutex_unlock( &m );
}

///////////////////////////////////////////////////
void test( int i )
{
	if( state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING )
	{
        if (!meal_comp(i))
        {
            //state[i] = THINKING;
        }else{

                state[i] = EATING;
                pthread_mutex_unlock( &s[i] );
        }
        
		
		
	}
}

bool meal_comp(int r)
{   
    
    int total_meal = 0;
    for (int k = 0; k < N; k++)
    {
        total_meal = total_meal +food[k];
    }
    int avg_total_meal = total_meal / N;
    
    if (food[r]>avg_total_meal)
    {
        return false;
    }
    
    return true;


}
