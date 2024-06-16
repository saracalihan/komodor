#ifndef KOMODOR_H
#define KOMODOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE (16)

typedef struct{
    char* shell;
    char* shell_flags;
    char* argumans;
}KomodorConfig;

typedef struct{
    char* command;

    // command result
    int exit_code;
    char* std_output;
    char* std_error;
    char* error_message;

    // expected command result
    int expected_exit_code;
    char* expected_std_output;
    char* expected_std_error;

    KomodorConfig config;
}KomodorTest;

KomodorTest komodor_create_test(char* command, KomodorConfig* config){
    KomodorTest test = {0};
    test.command = command;
    test.std_output=malloc(sizeof(char));
    test.std_error=malloc(sizeof(char));
    test.error_message=malloc(sizeof(char));
    *test.std_output='\0';
    *test.std_error='\0';
    *test.error_message='\0';

    if(NULL ==config){

        test.config.shell = "/bin/sh";
        test.config.shell_flags = ""; // "sh", "-c"
        test.config.argumans = NULL;

        // // Get default shell
        // system("echo $SHELL");
        // char default_shell
        // while (read(stdout_pipe[0], buffer, sizeof(buffer) - 1) != 0) {
        //     memset(buffer, 0, BUFFER_SIZE);
        // }

    } else{
        test.config = *config;
    }
    return test;
}
void komodor_define_test(KomodorTest* test, const int exit_code, const char* output, const char* error){
    if(NULL == output){
        test->expected_std_output=malloc(sizeof(char));
        *test->expected_std_output='\0';
    } else{
        test->expected_std_output = output;
    }

    if(NULL == error){
        test->expected_std_error=malloc(sizeof(char));
        *test->expected_std_error='\0';
    } else{
        test->expected_std_error = error;
    }
    test->expected_exit_code = exit_code;
}

void concat_buffer(char** dst, char* src){
    size_t dst_len = strlen(*dst),
           src_len = strlen(src),
           dst_cap = sizeof(*dst);
    if(dst_len+src_len>dst_cap){
        *dst = realloc(*dst, dst_len+src_len+1);
    }
    strcat(*dst, src);
}

int komodor_exec_test(KomodorTest* test){
    int stdout_pipe[2];
    int stderr_pipe[2];
    pid_t pid;
    int status;
    char buffer[BUFFER_SIZE];

    // Create pipes
    if (pipe(stdout_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(stderr_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Forward std to pipe
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        // Close pipes from child
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);

        // Execute
        test->exit_code = 0;
        test->exit_code =execl(
            test->config.shell,
            test->config.shell_flags,
            test->command,
            test->config.argumans,
            (char*)NULL
        );

        if(0 != test->exit_code){
            char msg[256]={0};
            sprintf(msg, "%m");
            concat_buffer(&test->error_message, msg);
            concat_buffer(&test->std_error, msg);
        }

        exit(test->exit_code);
    } else {  // Parent
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Read output
        while (read(stdout_pipe[0], buffer, sizeof(buffer) - 1) != 0) {
            concat_buffer(&test->std_output, buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }
        close(stdout_pipe[0]);

        // Read error
        while (read(stderr_pipe[0], buffer, sizeof(buffer) - 1) != 0) {
            concat_buffer(&test->std_error, buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }
        close(stderr_pipe[0]);

        // Wait until child prosess finish
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            test->exit_code =WEXITSTATUS(status);
        } else {
            printf("\nCommand did not terminate normally.\n");
        }
    }

    if(test->exit_code != test->expected_exit_code){
        return 0;
    }
    if(0 != strcmp(test->expected_std_output, test->std_output) ||
       0 != strcmp(test->expected_std_error,  test->std_error)
    ){
        return 0;
    }
    return 1;
}

#endif // KOMODOR_H
