#include "mclipm.h"

int main(int argc, char * argv[])
{   
    if (argc >= 2) // Minimum arguments are 1 , for the command "g" or generate (a password)
    {
        char choice = *argv[1];

        if(choice == 'g' || choice == 'G') // COMMAND : g | EXPANSION : generate 
        {
   	        /* Each character of the password is generated using rand()'s return value appropriately "cast" to be within printable ASCII range.
             * The formula for doing so is generally : (rand() % (UPPER_LIM - LOWER_LIM + 1)) + LOWER_LIM
	         * ASCII 33 = '!' ; ASCII 126 = '~'
             * ASCII 32 is a blank  , i.e, ' '. While a printable character, I find it inappropriate, as it can very easily be mistaken or ignored.
	         * rand() has limitations, but as this is not cryptographic  usage, rand() is good enough (and crucially : fast + portable).
	         */     
	        srand(time(0)); // Set seed to current time.
            uint_fast8_t pswd[PSWD_LENGTH+1] = ""; // Store bytes in array so that we can save to file if needed.

            printf("Password : \" ");
            for (int i = 0; i < PSWD_LENGTH; i++)
            {
                uint_fast8_t holder = (rand() % (126 - 33 + 1)) + 33; /* Credits : https://stackoverflow.com/a/65931015/13651625 */
                pswd[i] = holder;
            }
            pswd[PSWD_LENGTH] = '\0';
            printf("%s \"\nSave ? (Y/N) : ", pswd);
	        fflush(stdout); /* Ensure data is printed so user can respond */
           
	        char yorn;
            scanf(" %c", &yorn);
            EAT // Clean leftover input from stdin
            if(yorn == 'y' || yorn == 'Y')
            {
                GET_FILENAME
                FILE * file = fopen(filename,"a");

                if(file != NULL)
                {
                    GET_COMMENTS

		            char * time = time_str();

                    fprintf(file,"%s : %s : %s : %s\n", PROGRAM_NAME, (time == NULL)? "" : time, pswd, comments);
                    // time_str() may fail and return NULL, don't send NULL to printf as a %s

                    if(!fclose(file))
                        printf("Success.\n");
		            else
                    {
			            perror("Failed ");
                        return FCLOSE_ERR;
                    }
                }
                else
                {
                    perror("Failed ");
                    return FOPEN_ERR;
                }                
            }
        }
        else if((choice == 's'|| choice == 'S') && argc >= 5) // COMMAND : s | EXPANSION  : save
        {
	        // Saves comment + password to file with standard formmating ( time, program name,etc)

            FILE * file = fopen(argv[4],"a");
            // 4th arg is filename

            if(file != NULL)
            {
  		        char * time = time_str();
              
		        fprintf(file,"%s : %s : %s : %s\n", PROGRAM_NAME, (time == NULL)? "" : time, argv[2], argv[3]);
                // 2nd arg is password , 3rg arg is comments

                if(!fclose(file))
               		printf("Success.\n");
		        else
                { 
			        perror("Failed ");
                    return FCLOSE_ERR;
                }
            }
            else
            {
                perror("Failed ");
                return FOPEN_ERR;
            }

        }
        else if((choice == 'f' || choice == 'F') && argc >= 4) // COMMAND : f | EXPANSION : find
        {
	        // Find a string in the given file (search for passwords)

            FILE * file = fopen(argv[3],"r");
            //3rd ard is filename

            if(file != NULL)
            {
                char * find = argv[2];
                //2nd arg is search term

                /* String Search  line-by-line.
                * 1. Read a character from file in loop, to check for EOF.  Exit loop if EOF.
                * 2. If character is EOL ('\n') , increment line counter and bring column counter to 1.
                * 3. Else, seek back (to read the character we just read too) and read the whole line.
                * 4. Iterate over the line, searching the line for matches of said string until there are none.
                * 5. Every time a match is found, register its position column within the line, and print the position.
                * 6. Do for all lines in file .
                */
                
		        int c;
                long long unsigned line = 1, col = 1, count = 1;               
		        char buff[LINE_BUFF_SIZE] = "", *p, *LastReadingPoint;

                while((c = fgetc(file)) != EOF)
                {
                    if(c =='\n')
                    {
                        line++; col = 1;
                    }
                    else
		            {
                        fseek(file,-1,SEEK_CUR);
                        fgets(buff,LINE_BUFF_SIZE,file);
                        LastReadingPoint = buff;

                        while((p = strstr(LastReadingPoint,find)) != NULL)
			            {
                            intptr_t x = p - LastReadingPoint; // Get number of bytes into the line that match was found.
                            col += x; // Update position to current char
                            printf("-> %s\t(Ln %lld, Col %lld)\n", buff, line, col); // Print current position
                            count++; // Increment count of matches found
                            LastReadingPoint = p + strlen(find);// Move pointer over the match for next strstr()
                            col+= (long long) strlen(find); // Likewise update position in line
                        }

                        fseek(file,1,SEEK_CUR); // Check for EOF
                        int iseof = fgetc(file);
                        if(iseof == EOF) 
                        {
                            break;
                        }
                        else
                        {
                            fseek(file,-3,SEEK_CUR); // Rewind to last meaningfully read location
                        }
                    }
                }
                if(count == 1)
                {
                    printf("\nNo instances of \"%s\" in \"%s\".\n",find,argv[3]);
                }
                else
                {
                    printf("\n%llu result(s) for \"%s\" in \"%s\".\n",count-1,find,argv[3]);
                }
		        fclose(file);
            }
            else 
            {
                perror("Failed ");
                return FOPEN_ERR;
            }
        }

        else if((choice == 'r'|| choice == 'R') && argc >= 3) //COMMAND r | EXPANSION read
        {
	        // Read  the file and print it to screen.

            FILE * file = fopen(argv[2],"r");
            //2nd arg is filename

            if(file != NULL)
            {
                int c;
                while((c = fgetc(file)) != EOF)
                {
                    putchar(c); // Reasonable since we only deal with textfiles of small sizes.
                }
                fclose(file);
            }
            else
            {
                perror("Failed ");
                return FOPEN_ERR;
            }
        }

        else 
        {
            printf("Failed : Insufficient/Incorrect Arguments.\n");
            return INAPPROPIATE_ARGS_ERR;
        }
    }
    else 
    {
        printf("Failed : No Arguments.\n");
        return NO_ARGS_ERR;
    }
    return 0;
}

void rm_newline(char * string)
{
    size_t lenght = strlen(string);
    if(string[lenght-1] == '\n')
    {
        string[lenght-1] = '\0';
    }
}
char * time_str(void)
{
    time_t timenumber = time(0);
    char * time_now = ctime(&timenumber);

    if(time_now == NULL) return NULL; // ctime() may fail, in which case it returns NULL, so do the same.

    rm_newline(time_now); // ctime() returns string of fixed size, fixed field-widths and formatting. 25th is newline '\n' 
    return time_now;
}
