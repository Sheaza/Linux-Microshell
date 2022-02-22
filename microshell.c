#define _DEFAULT_SOURCE
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXBUFFER 1024
#define clear() printf("\033[H\033[2J")
#define HOME getenv("HOME")


int TakeArg(char* input);
void CutArg(char* input,char** arg,int *argc);
void CheckArg(char** arg,int argc);
void ChangeDir(char** arg,int argc);
void HelpCom();
void RemoveCom(char** arg,int argc);
void ExecArg(char** arg);
void StartSystem();
void GrepCom(char** arg,int argc);

char** history;
int n;
int argc;

int main()
{
	char input[MAXBUFFER];
	char* arguments[MAXBUFFER];
	StartSystem();
    while(1)
    {
        if(TakeArg(input)==1)
        {
            CutArg(input,arguments,&argc);
            CheckArg(arguments,argc);
        }
    }
    return 0;
}

void StartSystem()
{
	n=0; /*start histori*/
	clear();
	history=(char**)calloc(MAXBUFFER,sizeof(char));
	return;

}

int TakeArg(char* input)
{
    char* username = getenv("USER");
	char cdr[MAXBUFFER];
	getcwd(cdr,MAXBUFFER);
    char *buf;
    char prompt[MAXBUFFER];

    snprintf(prompt,sizeof(prompt),"[\033[96m%s\033[0m:\033[92m%s\033[0m]\033[93m$\033[0m ",username,cdr);
	buf = readline(prompt);
	if(strlen(buf)!=0)
	{
        add_history(buf);
		history[n]=strdup(buf);
		n++;
        strcpy(input,buf);
        free(buf);
		return 1;
	}
	else return 0;
}
void CutArg(char* input,char** arg,int *argc)
{
	int i;
	*argc=0;
    for(i=0;i<MAXBUFFER;i++)
	{
        arg[i] = strsep(&input," ");
		*argc=*argc+1;
        if(arg[i]==NULL)
		{
			*argc=*argc-1;
			 break;
		}
	}
	return;
}

void CheckArg(char** arg,int argc)
{
  	const int size=6;
  	char *myCom[size];
  	int switchNum=0;
  	int i;
  	int flag=0;
  	myCom[0]="cd";
  	myCom[1]="help";
  	myCom[2]="exit";
  	myCom[3]="rm";
  	myCom[4]="history";
    myCom[5]="grep";

	for(i=0;i<size;i++)
  	{
    		if(strcmp(arg[0],myCom[i])==0)
   		{
     			flag=1;
     			switchNum=i+1;
     			break;
   		}
  	}
	if(flag==0)
	{
 		ExecArg(arg);
	}
	else
	{
		switch(switchNum)
		{
  		case 1:
			printf("\033[92mChanging directory..\033[0m\n");
			ChangeDir(arg,argc);
  			break;
  		case 2:
  			HelpCom();
  			break;
  		case 3:
			printf("\033[91mExiting\n");
			for(i=0;i<3;i++)
			{
				printf(".\n");
				sleep(1);
			}
            printf("\033[0m");
			clear();
			free(history);
  			exit(0);
  			break;
  		case 4:
			RemoveCom(arg,argc);
  			break;
  		case 5:
			i=0;
			while(history[i]!=0&&i<=MAXBUFFER)
			{
				printf("[%d]%s\n",i,history[i]);
				i++;
			}
			break;
  		case 6:
			GrepCom(arg,argc);
  			break;
  		default:
			printf("\033[91mUnknown error occurred...\033[0m");
			break;
		}
		return;
	}
	return;
}
void HelpCom()
{
  	printf("\n\
	\033[93m###################### WITAM W POWLOCE MICROSHELL ######################\033[0m\n\n\
		\033[91mAutorem projektu jest: Natalia Wołowiec\033[0m\n\n\
	Dostepne komendy:\n\
	1. help\n\
	2. exit\n\
	3. cd\n\
	4. rm (flaga -r)\n\
	5. history\n\
        6. grep\n\
	7. Reszta komend dostepnych w powloce unixowej\n\n\
	\033[93m########################################################################\033[0m\n\n");
	return;
}

void ExecArg(char** arg)
{
	pid_t pid=fork();

	if(pid==-1)
	{
		perror("\033[91mThe following error occurred");
        printf("\033[0m");
		return;
	}
	else if(pid==0)
	{
		if(execvp(arg[0],arg)<0)
		{
			perror("\033[91mThe following error occured");
            printf("\033[0m");
            exit(0);
		}
		return;
	}
	else
	{
		wait(NULL);
		return;
	}

}

void ChangeDir(char** arg,int argc)
{
	if(argc>2)
	{
		printf("\033[91mError occurred: Too many arguments...\033[0m\n");
		return;
	}
	else if(argc==1)
	{
		if(chdir(HOME)<0)
        {
            perror("\033[91mFollowing error occurred");
            printf("\033[0m");
        }
        return;
	}
	else if(argc==2)
	{
		if(chdir(arg[1])<0)
        {
            perror("\033[91mFollowing error occurred");
            printf("\033[0m");
        }
		return;
	}
	return;
}
void RemoveCom(char** arg,int argc)
{

    struct stat statbuf;
    int rflag=0;

    if((argc>3) || (argc<2))
    {
        printf("\033[91mFollowing error occurred: Wrong number of arguments...\033[0m\n");
        return;
    }


    else if(argc==2)
    {
        if(stat(arg[1],&statbuf)<0)
        {
            perror("\033[91mFollowing error occurred");
            printf("\033[0m");
            return;
        }

	if(S_ISREG(statbuf.st_mode))
    {
		if(unlink(arg[1])<0)
		{
			perror("\033[91mFollowing error occurred");
            printf("\033[0m");
			return;
		}
	return;
	}


	else if(S_ISDIR(statbuf.st_mode))
	{
		if(rmdir(arg[1])<0)
		{
			perror("\033[91mFollowing error occurred");
            printf("\033[0m");
			return;
		}
	return;
	}
}

else if(argc==3)

{
	struct stat statbuf1;

	if(strcmp(arg[1],"-r")==0)
	{
		rflag=1;
	}


	if(rflag==1)
	{

		if(stat(arg[2],&statbuf1)<0)
		{
			perror("\033[91mFollowing error occurred");
            printf("\033[0m");
			return;
		}

		if(S_ISREG(statbuf1.st_mode))
		{
			if(unlink(arg[2])<0)
			{
				perror("\033[91mFollowing error occurred");
                printf("\033[0m");
				return;
			}
		return;
		}

		else if(S_ISDIR(statbuf1.st_mode))
		{

			char* argv[3];
			argv[0]="rm";
			argv[1]="-r";
			size_t dir_len = strlen(arg[2]);

			DIR *dir;

			if(!(dir=opendir(arg[2])))
			{
				perror("\033[91mFollowing error occurred");
                printf("\033[0m");
				return;
			}

			struct dirent *dir_entry;


			while((dir_entry = readdir(dir)))
			{

				if((strcmp(dir_entry->d_name,".")!=0)&&(strcmp(dir_entry->d_name,"..")!=0))
				{
					char* buffor;
					size_t length;
					length = dir_len + strlen(dir_entry->d_name) + 2;
					buffor=malloc(length);

					snprintf(buffor,length,"%s/%s",arg[2],dir_entry->d_name);
					argv[2]=buffor;
					RemoveCom(argv,3);
					free(buffor);
				}
			 }
		closedir(dir);
		rmdir(arg[2]);
		return;
	}

	else
	{
		printf("\033[91mFollowing error occurred: Wrong argument...\033[0m\n");
        return;
	}
}
}
}

void GrepCom(char** arg,int argc)
{

    if(argc==3)
    {
        FILE *fd;
        char *string;
        size_t length = 0;
        ssize_t num;
        char* pointer;
        if((fd = fopen(arg[2],"r"))==NULL)
        {
		    perror("\033[91mFollowing error occurred");
            printf("\033[0m");
            return;
        }
        printf("Sentences that contain the word \033[91m'%s'\033[0m:\n",arg[1]);
        while((num = getline(&string,&length,fd))!=-1)
        {
            if((pointer = strstr(string,arg[1]))!=NULL)
            {
                printf("%s",string);

            }

        }
        fclose(fd);
        return;
    }
    else
    {
        printf("\033[91mFollowing error occurred: Wrong number of arguments...\033[0m\n");
        return;
    }

}
