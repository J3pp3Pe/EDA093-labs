/*
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file(s)
 * you will need to modify the CMakeLists.txt to compile
 * your additional file(s).
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Using assert statements in your code is a great way to catch errors early and make debugging easier.
 * Think of them as mini self-checks that ensure your program behaves as expected.
 * By setting up these guardrails, you're creating a more robust and maintainable solution.
 * So go ahead, sprinkle some asserts in your code; they're your friends in disguise!
 *
 * All the best!
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <limits.h> // For PATH_MAX
#include <signal.h> // For signal handling
#include <fcntl.h>  // For file control operations (open)

// The <unistd.h> header is your gateway to the OS's process management facilities.
#include <unistd.h>

#include "parse.h"

static void print_cmd(Command *cmd);
static void print_pgm(Pgm *p);
void stripwhite(char *);
void execute_cmd(Command *cmd_list);
void handle_sigint(int signal);
void handle_sigchld(int signal);

int main(void)
{
  // Handle SIGINT (Ctrl+C). Currently using default behaviour.
  signal(SIGINT, SIG_DFL);
  signal(SIGCHLD, handle_sigchld); // Prevent zombie processes
  for (;;)
  {
    char *line;
    line = readline("> ");

    // EOF handling
    if (line == NULL)
    {
      printf("EOF\n");
      break; 
    }

    // Remove leading and trailing whitespace from the line
    stripwhite(line);

    // If stripped line not blank
    if (*line)
    {
      add_history(line);

      Command cmd;
      if (parse(line, &cmd) == 1)
      {
        // Just prints cmd
        print_cmd(&cmd);
        
        //execute command
        execute_cmd(&cmd);
      }
      else
      {
        printf("Parse ERROR\n");
      }
    }

    // Clear memory
    free(line);
  }

  return 0;
}

void execute_cmd(Command *cmd_list)
{
  int depth = 0;
  int i = 0;
  for (Pgm *p = cmd_list->pgm; p; p = p->next) depth++;
  // pgm stack, for reversing the order
  Pgm *pgm_stack[depth];
  for (Pgm *p = cmd_list->pgm; p; p = p->next) pgm_stack[i++] = p;

  int prev_pipe = -1;
  Pgm *pgm = cmd_list->pgm;
  pid_t pids[depth];

  for (int i = 0; i < depth; i++, pgm = pgm->next)
  {
    // reverses the order of commands
    Pgm *pgm = pgm_stack[depth - i - 1];
    int pipefd[2] = {-1, -1};
    if (i != depth - 1)
    {
      if (pipe(pipefd) < 0)
      {
          perror("works on my machine");
          if (prev_pipe != -1) close(prev_pipe);
          return;
      }
    }

    int pid = fork();
    if (pid < 0)
    {
      perror("Fork forked");
      if (pipefd[0] != -1) close(pipefd[0]);
      if (pipefd[1] != -1) close(pipefd[1]);
      if (prev_pipe != -1) close(prev_pipe);
      return;
    }

    if (pid == 0)
    {
      // rstdin
      if (i == 0 && cmd_list->rstdin) {
        int fd = open(cmd_list->rstdin, O_RDONLY);
        if (fd < 0) { perror("error opening at line 133"); _exit(1); }
        if (dup2(fd, STDIN_FILENO) < 0) { perror("error line 134"); _exit(1); }
        close(fd);
      } else if (prev_pipe != -1) {
        if (dup2(prev_pipe, STDIN_FILENO) < 0) { perror("error line 137"); _exit(1); }
      }

      // rstdout
      if (i != depth - 1) {
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) { perror("error line 142"); _exit(1); }
      } else if (cmd_list->rstdout) {
        int fd = open(cmd_list->rstdout, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) { perror("error line 145"); _exit(1); }
        if (dup2(fd, STDOUT_FILENO) < 0) { perror("error line 146"); _exit(1); }
        close(fd);
      }

      // Manage pipes
      if (pipefd[0] != -1) close(pipefd[0]);
      if (pipefd[1] != -1) close(pipefd[1]);
      if (prev_pipe != -1) close(prev_pipe);

      if (!pgm->pgmlist || !pgm->pgmlist[0])
      {
        perror("No command");
        _exit(127);
      }
      execvp(pgm->pgmlist[0], pgm->pgmlist);
      perror("execvp failed");
      _exit(127);
    }
    // Parent
    pids[i] = pid;

    if (prev_pipe != -1) close(prev_pipe);
    if (pipefd[1] != -1) close(pipefd[1]);
    prev_pipe = pipefd[0];
  }

  if (prev_pipe != -1) close(prev_pipe);

  if (cmd_list->background)
  {
    int parent_pid = getpid();
    for (int i = 0; i < depth; i++) printf("Process running in background with PID: %d\nParent PID: %d\n", pids[i], parent_pid);
  }
  else
  {
    int running;
    for (int i = 0; i < depth; i++)
    {
      waitpid(pids[i], &running, 0);
    }
  }
}

void handle_sigint(int signal)
{
  printf("\nHopefully did something\n");
  exit(0);
}

void handle_sigchld(int signal) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/*
 * Print a Command structure as returned by parse on stdout.
 *
 * Helper function, no need to change. Might be useful to study as inspiration.
 */
static void print_cmd(Command *cmd_list)
{
  printf("------------------------------\n");
  printf("Parse OK\n");
  printf("stdin:      %s\n", cmd_list->rstdin ? cmd_list->rstdin : "<none>");
  printf("stdout:     %s\n", cmd_list->rstdout ? cmd_list->rstdout : "<none>");
  printf("background: %s\n", cmd_list->background ? "true" : "false");
  printf("Pgms:\n");
  print_pgm(cmd_list->pgm);
  printf("------------------------------\n");
}

/* Print a (linked) list of Pgm:s.
 *
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
static void print_pgm(Pgm *p)
{
  if (p == NULL)
  {
    return;
  }
  else
  {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    print_pgm(p->next);
    printf("            * [ ");
    while (*pl)
    {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}


/* Strip whitespace from the start and end of a string.
 *
 * Helper function, no need to change.
 */
void stripwhite(char *string)
{
  size_t i = 0;

  while (isspace(string[i]))
  {
    i++;
  }

  if (i)
  {
    memmove(string, string + i, strlen(string + i) + 1);
  }

  i = strlen(string) - 1;
  while (i > 0 && isspace(string[i]))
  {
    i--;
  }

  string[++i] = '\0';
}