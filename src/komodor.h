#ifndef KOMODOR_H
#define KOMODOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define KOMODOR_LOG(fmt, ...) printf("[KOMODOR LOG] "fmt, ##__VA_ARGS__)
#define BUFFER_SIZE (16)

typedef struct{
    int log_test;

    char* shell;
    char* shell_flags[256];
    size_t shell_flags_len;
    char* argumants;
}KomodorConfig;

typedef struct{
    char* command;
    int is_pass;

    // command result
    int   exit_code;
    char* std_output;
    char* std_error;

    // expected command result
    int   expected_exit_code;
    char* expected_std_output;
    char* expected_std_error;
    char* std_input;

    KomodorConfig config;
}KomodorTest;

KomodorTest komodor_create_test(char* command, KomodorConfig* config){
    KomodorTest test = {0};
    test.command = command;
    test.std_output=malloc(sizeof(char));
    test.std_error=malloc(sizeof(char));
    *test.std_output='\0';
    *test.std_error='\0';

    if(NULL == config){
        test.config.shell = "sh";
        char* shell_flags[] ={
            "-c",
        };

        memcpy(&test.config.shell_flags, &shell_flags, sizeof(shell_flags));
        test.config.shell_flags_len = sizeof(shell_flags)/sizeof(char*);
        test.config.argumants = NULL;

        // // Get default shell
        // system("echo $SHELL");
        // char default_shell
        // while (read(stdout_pipe[0], buffer, sizeof(buffer) - 1) != 0) {
        //     memset(buffer, 0, BUFFER_SIZE);
        // }

    } else{
        if(config->shell == 0){
            config->shell = "sh";
        }

        if(config->shell_flags[0] ==NULL){
            char* shell_flags[] ={
                "-c",
            };

            memcpy(&config->shell_flags, &shell_flags, sizeof(shell_flags));
            config->shell_flags_len = sizeof(shell_flags)/sizeof(char*);
        }
        test.config = *config;
    }
    return test;
}
void komodor_define_test(KomodorTest* test, const int exit_code, const char* output, const char* input, const char* error){
    if(NULL == output){
        test->expected_std_output=malloc(sizeof(char));
        *test->expected_std_output='\0';
    } else{
        test->expected_std_output = strdup(output);
    }

    if(NULL == input){
        test->std_input=malloc(sizeof(char));
        *test->std_input='\0';
    } else{
        test->std_input = strdup(input);
    }

    if(NULL == error){
        test->expected_std_error=malloc(sizeof(char));
        *test->expected_std_error='\0';
    } else{
        test->expected_std_error = error;
    }
    test->expected_exit_code = exit_code;
}

static inline void concat_buffer(char** dst, char* src){
    size_t dst_len = strlen(*dst),
           src_len = strlen(src),
           dst_cap = sizeof(*dst);
    if(dst_len+src_len>dst_cap){
        *dst = realloc(*dst, dst_len+src_len+1);
    }
    strcat(*dst, src);
}

static inline void komodor_log_test(const KomodorTest *t){
    KOMODOR_LOG("-------------------\n");
    KOMODOR_LOG("\t%s\n", t->is_pass==0 ? "FAILED" : "PASSED" );
    KOMODOR_LOG("-------------------\n");
    KOMODOR_LOG("EXPECTED:\n");
    KOMODOR_LOG("\tstdout: '%s'\n", t->expected_std_output);
    KOMODOR_LOG("\tstderr: '%s'\n", t->expected_std_error);
    KOMODOR_LOG("\texit code: %d\n", t->expected_exit_code);
    KOMODOR_LOG("-------------------\n");
    KOMODOR_LOG("ACTUAL:\n");
    KOMODOR_LOG("\tstdin: '%s'\n", t->std_input);
    KOMODOR_LOG("\tstdout: '%s'\n", t->std_output);
    KOMODOR_LOG("\tstderr: '%s'\n", t->std_error);
    KOMODOR_LOG("\texit code: %d\n", t->exit_code);
    KOMODOR_LOG("-------------------\n");
}

int komodor_exec_test(KomodorTest* test){
    int stdout_pipe[2];
    int stderr_pipe[2];
    int stdin_pipe[2];
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

    if (pipe(stdin_pipe) == -1) {
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
        dup2(stdin_pipe[0], STDIN_FILENO);

        // Close pipes from child
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);

        // Execute
        test->exit_code = 0;
        // shell + shell_flags + command + command_arg
        const size_t argc = 1 + test->config.shell_flags_len + 1 + 1;
        char** argv = malloc(sizeof(char*) * argc);
        // push shell_flags
        argv[0] = test->config.shell;
        for(size_t i=0; i< test->config.shell_flags_len; i++){
            argv[i+1] = test->config.shell_flags[i];
        }

        argv[test->config.shell_flags_len+1] = test->command;
        argv[test->config.shell_flags_len+2] = test->config.argumants;

        test->exit_code =execvp(
            test->config.shell,
            argv
        );

        if(0 != test->exit_code){
            char msg[256]={0};
            sprintf(msg, "%m");
            concat_buffer(&test->std_error, msg);
        }

        exit(test->exit_code);
    } else {  // Parent
        write(stdin_pipe[1], test->std_input, strlen(test->std_input));
        close(stdin_pipe[1]);

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
            printf("\n[KOMODOR ERROR]Command did not terminate normally.\n");
        }
    }


    if(test->exit_code != test->expected_exit_code){
        test->is_pass=0;
    }
    if(0 != strcmp(test->expected_std_output, test->std_output) ||
       0 != strcmp(test->expected_std_error,  test->std_error)
    ){
        test->is_pass=0;
    }
    test->is_pass=1;

    if(test->config.log_test ==1){
        komodor_log_test(test);
    }
    return test->is_pass;
}

void komodor_free(KomodorTest* test){
    free(test->expected_std_output);
    free(test->expected_std_error);
    free(test->std_input);
    free(test->std_output);
    free(test->std_error);
}

#endif // KOMODOR_H
