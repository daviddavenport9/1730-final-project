#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <dirent.h>
#include <string>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

void sigHandle(int);

/**
*      
*/
bool tilde = false;
char* buf;
char* restPwd;
int main() {
      /*Tokenizes the first three directories
      of each the pwd and home and compares
      them for equality and substitutes a ~ 
      if necessary */
        char pwd[200];
        char home [200];
        
        char* pwdstr = getenv("PWD");
        
        char* pwdFirst = strtok(pwdstr, "/");
        char* pwdSecond = strtok(NULL, "/");
        char* pwdThird = strtok(NULL, "/");
        restPwd = strtok(NULL, "");//stores the remaining path of the pwd
                        
        char* homeFirst = strtok(getenv("HOME"), "/");
        char* homeSecond = strtok(NULL, "/");
        char* homeThird = strtok(NULL, "/");
        
        if((strcmp(pwdFirst, homeFirst) == 0) && (strcmp(pwdSecond, homeSecond) == 0) && ((strcmp(pwdThird, homeThird) == 0))){
          setenv("HOME","~",1);//if first 3 directories of each are equal, set home variable to ~
          tilde = true;
        }
  
    while(true){
        char input[40];
        char output[40];
        char outputApp[40];
        bool outRedir = false;
        bool inRedir = false;
        bool appRedir = false;
        char* start;
        int status;
        char* argv[11];
        int i = 1;//total arguments
        char line[512];
        int fd;
        if (tilde == true){
        cout << "1730sh:" << getenv("HOME") << "/" << restPwd << "$ ";//prints the ~ plus the rest of the pwd
        }
       else{
        cout << "1730sh:" << getcwd(buf, 2000) << "$ ";//prints out the full cwd otherwise
        }
        if(signal(SIGINT, sigHandle) == SIG_ERR) {
          cout << "error registering handle for sigint" << endl;
          exit(1);
        } // if 
        cin.getline(line, 512);
        start = strtok(line," =");
        char* cmd = start;
        argv[0] = cmd; 
        while(start != NULL){
            start = strtok(NULL," =");
            argv[i] = start;
            i++;
        } // while  
        argv[i+1] = (char*) NULL;
        /**exits the program when exit is called*/
        if (strcmp(argv[0], "exit") == 0){
            exit(0);
        }else if (strcmp(argv[0], "export") == 0){
          /**enables adding and setting env variables*/
          setenv(argv[1],argv[2],1);
          continue;
        }else if (strcmp(argv[0], "cd") == 0){
          /**changes directories*/
          int cd = chdir(argv[1]);
          if (cd == -1){ // cd error                                                                                                                                                                
            perror("cd error");
            exit(1);
          } // if                                                                                                                                                                                   
        } // if    
    
        
        pid_t child = fork();
        if(child < 0){ // if error                                                                                                                                                                        
            perror("fork error");
            return 1;
        }else if(child == 0){ // in child process  
            for (int j = 0; argv[j] != '\0'; j++)//loops to look for >,<, and >>
             {
               if (strcmp(argv[j], ">") == 0)//STDOUT(TRUNC)
               {
              argv[j] = (char*) NULL;
              strcpy(output, argv[j+1]);
              outRedir = true;
              }//if   
              else if (strcmp(argv[j], "<") == 0)//STDIN
               {
              argv[j] = (char*) NULL;
              strcpy(input, argv[j+1]);
              inRedir = true;
              }//if 
               else if (strcmp(argv[j], ">>") == 0)//STDOUT(APPEND)
               {
              argv[j] = (char*) NULL;
              strcpy(outputApp, argv[j+1]);
              appRedir = true;
              }//if           
             }//for
             
             if (outRedir == true)//redirects standard output & truncates
               {
              int fd = open(output, O_CREAT | O_WRONLY | O_TRUNC, 0777);
              if (fd == -1)
              {
                perror("Can't open file");
              }//if
              dup2(fd, STDOUT_FILENO);
              close(fd);
               }//if
               
               if (inRedir == true)//redirects standard inpt
               {
              int fd = open(input, O_RDONLY);
              if (fd == -1)
              {
                perror("Can't open file");
              }//if
              dup2(fd, STDIN_FILENO);
              close(fd);
               }//if
               
               if (appRedir == true)//redirects standard output(append)
               {
              int fd = open(outputApp, O_CREAT | O_WRONLY | O_APPEND, 0777);
              if (fd == -1)
              {
                perror("Can't open file");
              }//if
              dup2(fd, STDOUT_FILENO);
              close(fd);
               }//if
               
           
         int k = execvp(cmd, argv);
         if (k == -1)
         {
          perror("error exec'ing");
          exit(0);
          }
          
        }//else if
        else{//in parent process
            wait(&status);
            
        } // if
    } // if
} // main

/**handles the call to CTRL-C*/
void sigHandle(int signo){
  if(signo == SIGINT){
    cout << "SIGINT blocked" << endl;
    if (tilde == true){
        cout << "1730sh:" << getenv("HOME") << "/" << restPwd << "$ ";
    }
    else{
         cout << "1730sh:" << getcwd(buf, 2000) << "$ ";
    }
  } // if                                                                       
} // sigHandle             
