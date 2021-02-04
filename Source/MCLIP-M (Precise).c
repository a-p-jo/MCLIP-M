#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PROGRAM_NAME "MCLIP-M Precise"

#define MAX_FILENAME_SIZE 501
#define MAX_COMMENT_CHARS 301
#define PSWD_LENGTH 32
#define LINE_BUFFER 1025

#define EAT int eat; while ((eat = getchar()) != '\n' && eat != EOF);
#define GET_FILENAME printf("Filename : "); char filename[MAX_FILENAME_SIZE] = ""; fgets(filename,MAX_FILENAME_SIZE,stdin); rm_newline(filename);
#define GET_COMMENTS printf("Comments : "); char comments[MAX_COMMENT_CHARS]; fgets(comments,MAX_COMMENT_CHARS,stdin); rm_newline(comments);

void rm_newline(char * string)
{
    size_t lenght = strlen(string);
    if(string[lenght-1] == '\n')
    {
        string[lenght-1] = '\0';
    }
}

char * get_time_str(void)
{
    time_t timenumber = time(0);
    char * time_now = ctime(&timenumber);
    time_now[strlen(time_now)-1] = '\0';
    return time_now;
}

int main(int frequency, char * arguments[])
{   
    if (frequency >= 2)
    {
        char choice = *arguments[1];

        if(choice == 'g' || choice == 'G')
        {
            srand(time(0));

            char pswd[PSWD_LENGTH+1] = "";
            printf("Password : \" ");

            for (int i = 0; i < PSWD_LENGTH; i++)
            {
                int holder = (rand() % (126 - 33 + 1)) + 33; /* Credits : https://stackoverflow.com/a/65931015/13651625 */
                pswd[i] = holder;
            }
            pswd[PSWD_LENGTH] = '\0';

            printf("%s \"\nSave ? (Y/N) : ", pswd);
            char yorn;
            scanf(" %c", &yorn);
            EAT

            if(yorn == 'y' || yorn == 'Y')
            {
                GET_FILENAME

                FILE * file = fopen(filename,"a");

                if(file != NULL)
                {
                    GET_COMMENTS

                    fprintf(file,"%s : %s : %s : %s\n", PROGRAM_NAME, get_time_str(), pswd, comments);

                    fclose(file);
                    printf("Success.\n");
                }
                else
                {
                    perror("Failed ");
                }
                
            }
        }

        else if((choice == 's'|| choice == 'S') && frequency >= 5)
        {
            FILE * file = fopen(arguments[4],"a");
            // 4th arg is filename

            if(file != NULL)
            {
                fprintf(file,"%s : %s : %s : %s\n", PROGRAM_NAME, get_time_str(), arguments[2], arguments[3]);
                // 2nd arg is password , 3rg arg is comments

                fclose(file);
                printf("Success.\n");
            }
            else
            {
                perror("Failed ");
            }

        }

        else if((choice == 'f' || choice == 'F') && frequency >= 4)
        {
            FILE * file = fopen(arguments[3],"r");
            //3rd ard is filename

            if(file != NULL)
            {
                char * find = arguments[2];
                //2nd arg is search term
                int c;
                long long unsigned line = 1, col = 1, count = 1;
                char buff[LINE_BUFFER] = "", *p, *LastReadingPoint;

                while((c = fgetc(file)) != EOF)
                {
                    if(c =='\n')
                    {
                        line++; col = 1;
                    }
                    else{
                        fseek(file,-1,SEEK_CUR);
                        fgets(buff,LINE_BUFFER,file);
                        LastReadingPoint = buff;

                        while((p = strstr(LastReadingPoint,find)) != NULL){
                            intptr_t x = p - LastReadingPoint;
                            col += x; 
                            printf("-> %s\t(Ln %lld, Col %lld)\n", buff, line, col);
                            count++;
                            LastReadingPoint = p + strlen(find);
                            col+= (long long) strlen(find);
                        }

                        fseek(file,1,SEEK_CUR);
                        int iseof = fgetc(file);
                        if(iseof == EOF) 
                        {
                            break;
                        }
                        else
                        {
                            fseek(file,-3,SEEK_CUR);
                        }
                    }
                }
                if(count == 1)
                {
                    printf("\nNo instances of \"%s\" found in file \"%s\".\n",find,arguments[3]);
                }
                else
                {
                    printf("\n%llu result(s) for \"%s\" in file \"%s\".\n",count-1,find,arguments[3]);
                }
            }
            else 
            {
                perror("Failed ");
            }
        }

        else if((choice == 'r'|| choice == 'R') && frequency >= 3)
        {
            FILE * file = fopen(arguments[2],"r");
            //2nd arg is filename

            if(file != NULL)
            {
                int c;
                while((c = fgetc(file)) != EOF)
                {
                    putchar(c);
                }
                fclose(file);
            }
            else
            {
                perror("Failed ");
            }
        }

        else 
        {
            printf("Failed : Insufficient/Incorrect Arguments.\n");
        }
    }
    else 
    {
        printf("Failed : Insufficient Arguments.\n");
    }
    return 0;
}
