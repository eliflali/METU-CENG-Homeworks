#include "parser.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;
int quit = 0; // like a bool

int exec_command(command cmd);
int exec_pipeline(single_input pip_input);
int exec_subshell(single_input subshell_command, bool ispiped);
int processCommand(single_input command);
int exec_sequential(parsed_input input);
int exec_parallel(parsed_input input);
int parseInput(const std::string& userCommand);
int mainloop();
int executor(parsed_input input);

int exec_command(command cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        if (execvp(cmd.args[0], cmd.args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }

    } else { // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
    }
    return 0;
}


int exec_pipeline(single_input pip_input) {
    pipeline pline = pip_input.data.pline;
    int num_commands = pline.num_commands;
    std::vector<int> pipes((num_commands - 1) * 2);
    std::vector<pid_t> pids(num_commands);

    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipes.data() + i * 2) != 0) {
            perror("pipe");
            return -1;
        }
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        }
        else if (pid == 0)
        { // Child process
            if (i > 0)
            {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num_commands - 1)
            {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }
            for (auto &pipe_end : pipes)
            {
                close(pipe_end);
            }

            single_input s_input;
            memset(&s_input, 0, sizeof(single_input));
            s_input.type = INPUT_TYPE_COMMAND;
            s_input.data.cmd = pline.commands[i];

            if (s_input.type == INPUT_TYPE_COMMAND)
            {
                exec_command(pline.commands[i]);
                exit(EXIT_FAILURE);
            }
            else if (s_input.type == INPUT_TYPE_SUBSHELL)
            {
                int result = exec_subshell(s_input, true);
                exit(result);
            }

        }
        else
        {
            pids[i] = pid;
        }
    }

    for (auto &pipe_end : pipes) {
        close(pipe_end);
    }

    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }

    return 0;
}

void repeater(const std::vector<int>& pipe_write_ends)
{
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
    {
        for (int fd : pipe_write_ends)
        {
            write(fd, buffer, bytes_read);
        }
    }

    for (int fd : pipe_write_ends)
    {
        close(fd);
    }
}

void subshell_parser(const std::string& userCommand, parsed_input& input)
{
    const char* line = userCommand.c_str();

    if (parse_line(const_cast<char*>(line), &input))
    {

        //pretty_print(&input);
    }
    else
    {
        fprintf(stderr, "Failed to parse the input line.\n");
    }
}

int exec_subshell(single_input subshell_command, bool ispiped) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) { // child process
        char* subshell_str = subshell_command.data.subshell;
        parsed_input input;
        memset(&input, 0, sizeof(parsed_input));
        subshell_parser(subshell_str, input);

        bool is_parallel = ispiped && input.num_inputs > 1 && input.separator == SEPARATOR_PARA;

        if (is_parallel) // parallel input
        {
            std::vector<int> pipe_write_ends;

            for(int i = 0; i<input.num_inputs; i++)
            {
                int pipe_fds[2];
                if(pipe(pipe_fds) != 0)
                {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }

                pipe_write_ends.push_back(pipe_fds[1]);

                pid_t cmd_pid = fork();
                if(cmd_pid == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if(cmd_pid == 0) //child process
                {
                    close(pipe_fds[1]);
                    dup2(pipe_fds[0], STDIN_FILENO);
                    close(pipe_fds[0]);

                    exec_command(input.inputs[i].data.cmd);
                    exit(EXIT_SUCCESS);
                }

                else //parent process
                {
                    close(pipe_fds[0]);
                }
            }



            pid_t  repeater_pid = fork();
            if(repeater_pid == 0) // repeater child
            {
                repeater(pipe_write_ends);
            }
            else // repeater parent
            {
                for (int fd: pipe_write_ends)
                {
                    close(fd);
                }

                waitpid(repeater_pid, NULL, 0);
            }

            //wait all command processes to finish
            for(int i = 0; i< input.num_inputs; i++)
            {
                wait(NULL);
            }

            exit(0);
        }

        else // normal input
        {
            parseInput(subshell_str);
        }

        free_parsed_input(&input);
    }

    else {

        int status;
        waitpid(pid, &status, 0);
    }
    return 0;
}



int processCommand(single_input command)
{
    switch (command.type) {
        case INPUT_TYPE_COMMAND:
            // command structure created -- u can then use it with commands
            single_input wrapped_command;
            wrapped_command.type = INPUT_TYPE_COMMAND;
            wrapped_command.data.cmd = command.data.cmd;

            return exec_command(wrapped_command.data.cmd);
        case INPUT_TYPE_PIPELINE:
            return exec_pipeline(command);
        case INPUT_TYPE_SUBSHELL:
            return exec_subshell(command, false);
        default:
            return -1;
    }
}

int exec_sequential(parsed_input input) {
    int result = 0;
    for (int i = 0; i < input.num_inputs; ++i) {
        result = processCommand(input.inputs[i]);
        if (result != 0) { // error occured in execs
            break;
        }
    }
    return result;
}


int exec_parallel(parsed_input input) {
    std::vector<pid_t> pids;

    for (int i = 0; i < input.num_inputs; ++i) {
        pid_t pid = fork();
        if (pid == -1)  // fork error
        {
            perror("fork");
            for (pid_t p : pids) // waiting others
            {
                waitpid(p, NULL, 0);
            }
            return -1;
        }
        else if (pid == 0) // child
        {
            if (input.inputs[i].type == INPUT_TYPE_COMMAND) {
                execvp(input.inputs[i].data.cmd.args[0], input.inputs[i].data.cmd.args);
                perror("execvp");
                exit(EXIT_FAILURE);
            } else if (input.inputs[i].type == INPUT_TYPE_PIPELINE) {
                exec_pipeline(input.inputs[i]);
                exit(0);
            }
        }
        else // parent
        {
            pids.push_back(pid);
        }
    }

    for (pid_t p : pids) // wait processes -- zombie? check
    {
        waitpid(p, NULL, 0);
    }

    return 0;
}

int exec_pip_seperator(parsed_input input)
{
    //creating pipeline
    single_input pip_input;
    pip_input.type = INPUT_TYPE_PIPELINE;
    pip_input.data.pline.num_commands = input.num_inputs;
    vector<std::string> types;

    for (int i = 0; i < input.num_inputs; ++i) {
        pip_input.data.pline.commands[i] = input.inputs[i].data.cmd;
    }
    pipeline pline = pip_input.data.pline;
    int num_commands = pline.num_commands;
    std::vector<int> pipes((num_commands - 1) * 2);
    std::vector<pid_t> pids(num_commands);

    /*
    A|B|C
    for (comm in A,B,C) { pipe_i = pipe()
            fork
               dup(pipe_i-1, 0)  (if i>0)
               dup(pipe_i, 1)    (if i<n)
               exec(comm)
    for (process in A,B,C)
        wait  (after forks complete)
     */
    for (int i = 0; i < num_commands - 1; ++i)
    {
        if (pipe(pipes.data() + i * 2) != 0)
        {
            perror("pipe");
            return -1;
        }
    }

    for (int i = 0; i < num_commands; ++i)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return -1;
        }
        else if (pid == 0)  // child
        {
            if (i > 0) // stdin -> fd[0] ((i-1)*2)
            {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num_commands - 1) // stdout -> fd[1] (i*2+1)
            {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }
            for (auto &pipe_end : pipes) {
                close(pipe_end);
            }

            single_input s_input;
            memset(&s_input, 0, sizeof(single_input));
            s_input.type = input.inputs[i].type;
            s_input.data.cmd = pline.commands[i];

            if (s_input.type == INPUT_TYPE_COMMAND)
            {
                exec_command(pline.commands[i]);
                exit(EXIT_FAILURE);
            } else if (s_input.type == INPUT_TYPE_SUBSHELL)
            {
                int result = exec_subshell(s_input, true);
                exit(result);
            }

        }
        else // parent
        {
            pids[i] = pid;
        }
    }

    for (auto &pipe_end : pipes) {
        close(pipe_end);
    }

    for (pid_t pid : pids) // wait childs -- check zombies
    {
        waitpid(pid, NULL, 0);
    }

    return 0;
}

int executor(parsed_input input)
{
    if (input.separator == SEPARATOR_PARA) {
        return exec_parallel(input);
    }

    if (input.separator == SEPARATOR_SEQ) {
        return exec_sequential(input);
    }

    if (input.separator == SEPARATOR_PIPE) {
        //construct a pipeline -- already exists?
        single_input pipeline_command;
        pipeline_command.type = INPUT_TYPE_PIPELINE;
        pipeline_command.data.pline.num_commands = input.num_inputs;
        vector<std::string> types;

        for (int i = 0; i < input.num_inputs; ++i) {
            pipeline_command.data.pline.commands[i] = input.inputs[i].data.cmd;
        }

        // this func handles when pip is outside -- not type but seperator
        return exec_pip_seperator(input);
    }

    else
    {
        for (int i = 0; i < input.num_inputs; i++)
        {
            if (processCommand(input.inputs[i]) != 0)
            {
                break; // error in exec
            }
        }
    }
    return 0;
}

int parseInput(const std::string& userCommand)
{
    // converting to string -- probably working:
    const char* line = userCommand.c_str();

    parsed_input input;
    memset(&input, 0, sizeof(parsed_input));

    if (parse_line(const_cast<char*>(line), &input))
    {
        //pretty_print(&input);
        executor(input);

    }
    else // error - finish the program w/ -1
    {
        return -1;
    }

    free_parsed_input(&input); // free the mem -- leak check
    return 0;
}




int mainloop()
{
    while(!quit)
    {
        std::string userCommand;
        cout << "/> "; // prompt
        std::getline(std::cin, userCommand); // line got -- any funcs in header?

        if(userCommand == "quit") // when quit, terminate
        {
            quit = 1;
        }
        else
        {
            parseInput(userCommand);
        }
    }
    return 0;
}

int main()
{
    mainloop(); // input in loop
    return 0;
}



