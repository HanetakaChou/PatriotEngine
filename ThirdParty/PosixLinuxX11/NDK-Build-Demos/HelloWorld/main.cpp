#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>

int main()
{
  char addr2line_result[4096];
  {
    addr2line_result[0] = '\0';
    int sv_stdout[2];
    int sv_stderr[2];
    int ret_sv_stdout = socketpair(AF_UNIX, SOCK_STREAM, 0, sv_stdout);
    int ret_sv_stderr = socketpair(AF_UNIX, SOCK_STREAM, 0, sv_stderr);
    if (ret_sv_stdout != -1 && ret_sv_stderr != -1)
    {
      shutdown(sv_stdout[0], SHUT_WR);
      shutdown(sv_stdout[1], SHUT_RD);
      shutdown(sv_stderr[0], SHUT_WR);
      shutdown(sv_stderr[1], SHUT_RD);

      pid_t pid_child = fork();
      if (pid_child == 0)
      {
        close(sv_stdout[0]);
        close(sv_stderr[0]);

        close(STDOUT_FILENO);
        dup2(sv_stdout[1], STDOUT_FILENO);
        close(sv_stdout[1]);
        close(STDERR_FILENO);
        dup2(sv_stderr[1], STDERR_FILENO);
        close(sv_stderr[1]);

        char arg_0[] = {"addr2line1"};
        char arg_1[] = {"-fCe"};
        char arg_2[4096] = {"111"};
        char arg_3[] = {"ffffffffffffffff"};
        //snprintf(arg_2, 4096, "%s", filename.c_str());
        //snprintf(arg_3, sizeof(arg_3) / sizeof(char), "%" PRIxPTR, addr2line_addr);
        char *argv[] = {arg_0, arg_1, arg_2, arg_3, NULL};
        int ret_evec = execvp(argv[0], argv);

        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        std::exit(ret_evec);
      }

      close(sv_stdout[1]);
      close(sv_stderr[1]);

      printf("50\n");

      if (pid_child != -1)
      {

        printf("55 %d\n", static_cast<int>(pid_child));

        int stat_loc;
        if (waitpid(pid_child, &stat_loc, 0) != -1) //release zombie
        {

          printf("61\n");

          if ((WIFEXITED(stat_loc)))
          {

            printf("66\n");

            if ((WEXITSTATUS(stat_loc)) == 0)
            {
              int ret_rd = read(sv_stdout[0], addr2line_result, 4095);
              if (ret_rd != -1)
              {
                addr2line_result[ret_rd] = '\0';
              }
              else
              {
                addr2line_result[0] = '\0';
              }
            }
            else
            {

              printf("83\n");

              char addr2line_stdout[4096];
              int ret_rd_stdout = read(sv_stdout[0], addr2line_stdout, 4095);

              printf("88 %d\n", static_cast<int>(ret_rd_stdout));

              if (ret_rd_stdout != -1 && ret_rd_stdout != 0)
              {
                addr2line_stdout[ret_rd_stdout] = '\0';
                for (int i = 0; i < ret_rd_stdout; ++i)
                {
                  if (addr2line_stdout[i] == '\r' || addr2line_stdout[i] == '\n' || addr2line_stdout[i] == '\0')
                  {
                    addr2line_stdout[i] = ' ';
                  }
                }
                printf("%s", addr2line_stdout);
              }

              char addr2line_stderr[4096];
              int ret_rd_stderr = read(sv_stderr[0], addr2line_stderr, 4095);

              printf("106 %d\n", static_cast<int>(ret_rd_stdout));

              if (ret_rd_stderr != -1 && ret_rd_stderr != 0)
              {
                addr2line_stderr[ret_rd_stderr] = '\0';
                for (int i = 0; i < ret_rd_stderr; ++i)
                {
                  if (addr2line_stderr[i] == '\r' || addr2line_stderr[i] == '\n' || addr2line_stderr[i] == '\0')
                  {
                    addr2line_stderr[i] = ' ';
                  }
                }
                printf("%s", addr2line_stderr);
              }
            }
          }
        }
      }

      close(sv_stdout[0]);
      close(sv_stderr[0]);
    }
  }

  return 0;
}
