#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
char *search_paths[11];

#define MAX_INPUT_SIZE 1024

char *trim_whitespace(char *str) {
    // Skip leading whitespace characters
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

void built_in(char *all_args[], int num_args){


    if (strcmp(all_args[0], "exit") == 0 && num_args == 1) {
        exit(0);
    }
    else if (strcmp(all_args[0], "exit") == 0 && num_args > 1){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(0);
    }
    else if (strcmp(all_args[0],"cd")==0){
        if(num_args != 2){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        chdir(all_args[1]);
    }
    else if (strcmp(all_args[0],"path")==0){

        for (int i =0; i < 11; i++){
            search_paths[i] = NULL;
        }

        for (int i = 0; i < num_args; ++i) {
            search_paths[i] = all_args[i+1];
        }
    }

}


void execute_command(char *input) {
    char *input_dupe = NULL;
    char *arguments;
    char *argumentstest;
    char *all_args[11];
    char *all_argstest[11];
    char *token;
    int num_args = 0;
    int num_argstests = 0;
    bool redirect = false;
    bool executable = false;
    char *output_filename = NULL;
    char *outfile_dupe;

    input_dupe = strdup(input);

    int out_count = 0;

    for (int i = 0; input_dupe[i] != '\0'; i++) {
        if (input_dupe[i] == '>') {
            out_count++;
        }
    }

    if(out_count > 1){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(0);
    }

    while ((argumentstest= strsep(&input_dupe, ">")) != NULL && num_argstests <10){
        if (argumentstest[0] != '\0') {
            all_argstest[num_argstests++] = argumentstest;
        }
    }

    outfile_dupe = all_argstest[1];

    for (int i = 0; outfile_dupe[i] != '\0'; i++) {
        if (outfile_dupe[i] == ' ') {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
    }

    if (num_argstests > 1){
        redirect = true;
    }


    if (redirect){
        output_filename = all_argstest[1];
    }

    while ((arguments = strsep(&all_argstest[0], " ")) != NULL && num_args < 10 ) {
//        if (strcmp(arguments, ">") == 0) {
//            redirect = true;
//            if ((arguments = strsep(&input_dupe, " ")) != NULL) {
//                output_filename = arguments;
//            }
//
//            if ((arguments= strsep(&input_dupe, " ")) != NULL) {
//                char error_message[30] = "An error has occurred\n";
//                write(STDERR_FILENO, error_message, strlen(error_message));
//                exit(0);
//            }
//
//            break;
//        }

        if (arguments[0] != '\0') {
            all_args[num_args++] = arguments;
        }
    }


    if (strcmp(all_args[0], "cd") == 0 || strcmp(all_args[0], "exit") == 0 || strcmp(all_args[0], "path") == 0) {
        built_in(all_args, num_args);

        if (strcmp(all_args[0], "path") == 0) {
            executable = false;
        }
    } else {
        int num_paths = 0;
        while (search_paths[num_paths] != NULL) {
            all_args[num_args] = NULL;
            char executable_path[MAX_INPUT_SIZE];

            strcpy(executable_path, search_paths[num_paths]);
            if (search_paths[num_paths][strlen(search_paths[num_paths]) - 1] != '/') {
                strcat(executable_path, "/");
            }
            strcat(executable_path, all_args[0]);

            if (access(executable_path, X_OK) == 0) {
                pid_t pid = fork();
                if (pid == -1) {
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                } else if (pid == 0) {
                    if (redirect) {
                        output_filename = trim_whitespace(output_filename);
                        int fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        if (fd == -1) {
                            char error_message[30] = "An error has occurred\n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                    if (execvp(executable_path, all_args) == -1) {
                        exit(1);
                    }
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                    executable = true;
                    break;
                }
            }
            else{
                executable = false;
            }

            num_paths++;
        }
        if (!executable){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }

    free(input_dupe);
}


void interactive_mode(){

    while (1){
        char *input = NULL;

        size_t bufsize = 0;

        printf("witsshell> ");
        ssize_t length = getline(&input, &bufsize, stdin);

        if (length == -1) {
            if (feof(stdin)) {
                free(input);
                printf("\n");
                exit(0);
            } else {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                free(input);
                exit(1);
            }
        }
        if (input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }

        int isEmpty = 1;
        for (int i = 0; input[i] != '\0'; i++) {
            if (!isspace((unsigned char)input[i])) {
                isEmpty = 0;
                break;
            }
        }

        if (isEmpty) {
            free(input);
            continue;
        }

        execute_command(input);
        free(input);
    }
}

void batch_mode(const char *filename){
    FILE *batch = fopen(filename, "r");
    if (batch == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    char *input = NULL;
    size_t bufsize = 0;

    while (getline(&input, &bufsize, batch) != -1){
        input[strcspn(input, "\n")] = '\0';
        int isEmpty = 1;
        for (int i = 0; input[i] != '\0'; i++) {
            if (!isspace((unsigned char)input[i])) {
                isEmpty = 0;
                break;
            }
        }

        if (isEmpty) {
            free(input);
            continue;
        }
        execute_command(input);
    }

    fclose(batch);
}

int main(int MainArgc, char *MainArgv[]){
    for (int i =0; i < 11; i++){
        free(search_paths[i]);
    }

    search_paths[0]= "/bin/";

    if (MainArgc == 1){
        interactive_mode();
    }
    else if (MainArgc == 2){
        batch_mode(MainArgv[1]);
    }
    else {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    return(0);
}