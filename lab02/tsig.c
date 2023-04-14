#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

#define NUM_CHILD 3 // number of child processes to create

unsigned int cpidc = 0; // number of child processes successfully created
pid_t cpidv[NUM_CHILD]; // array of created child processes

unsigned int kc = 0; // Ctrl+C(SIGINT) occurence

// send SIGTERM to all child processes and exit with error code 1
void exit_all()
{
    pid_t pid = getpid();
    for (int i = 0; i < cpidc; i++)
    {
        printf("parent[%d]: sending SIGTERM signal to child %d\n", pid, cpidv[i]);
        kill(cpidv[i], SIGTERM);
    }
    printf("parent[%d]: exiting with error code 1\n", pid);
    exit(1);
}

// SIGTERM handler
void term_handler()
{
    printf("child[%d]: received SIGTERM signal, terminating\n", getpid());
    exit(0);
}

// keyboard handler
void kint_handler()
{
    printf("parent[%d]: received SIGINT signal, occurence %d\n", getpid(), ++kc);
}

// set all signals to the given signal
void set_signals(void *sig)
{
    for (int i = 0; i < NSIG; i++)
        signal(i, sig);
}

// child process
void child_proc()
{
#ifdef WITH_SIGNALS
    signal(SIGTERM, term_handler);
    signal(SIGINT, SIG_IGN);
#endif
    pid_t pid = getpid();
    printf("child[%d]: ppid -> %d\n", pid, getppid());
    sleep(10);
    printf("child[%d]: execution complete\n", pid);
    exit(0);
}

// extended fork
void _fork()
{
    if (kc > 0)
        exit_all();
    pid_t cpid = fork();
    if (cpid == 0)
        child_proc();
    else if (cpid > 0)
    {
        printf("parent[%d]: created child %d\n", getpid(), cpid);
        cpidv[cpidc++] = cpid;
    }
    else
        exit_all();
}

// main(parent) process
int main()
{
    if (NUM_CHILD > 0)
    {
        // ignore all signals, restore default handler for SIGCHLD and set custom handler for SIGINT
#ifdef WITH_SIGNALS
        set_signals(SIG_IGN);
        signal(SIGCHLD, SIG_DFL);
        signal(SIGINT, kint_handler);
#endif
        for (int i = 1; i < NUM_CHILD; i++)
        {
            _fork();
            sleep(1);
        }
        _fork();
        for (int i = 0; i < cpidc; i++)
            printf("parent[%d]: child %d exited\n", getpid(), wait(NULL));
            // restore default handlers for all signals
#ifdef WITH_SIGNALS
        set_signals(SIG_DFL);
#endif

        printf("parent[%d]: execution complete\n", getpid());
    }
    return 0;
}
