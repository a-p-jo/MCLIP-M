#include "mclipm.h"

int main(int argc, char **argv)
{              
        if(argc < 2 || *argv[1] == 'h' || !strcmp("-h",argv[1]) ) {
                fputs(
		NAME " : Minimalist Command-Line Password Manager.\n"
		"See https://github.com/a-p-jo/mclipm for git repo.\n"
		"\n"
		"4 main commands : \"g\"(generate), \"s\"(save), \"f\"(find), \"d\"(delete)\n"
		"\n"
		"1. \"g\" :\n"
		"   Generates a random password.\n"
		"   Example : mclipm g\n"
		"          -> Generates and output a password containing 32 non-whitespace printable ASCII chars.\n"
		"   Format  : mclipm g [options (optional)] [option's argument (optional)]...\n"
		"\n"
                "   Optionally, \"g\" takes any of 3 options, in any order :\n"
		"\n"
		"   1.1 \"-l\" : Specify the number of characters in the password.\n"
		"   Example  : mclipm g -l 16\n"
		"           -> Outputs a 16-char password.\n"
		"   Format   : mclipm g -l <any natural number less than 2^64 - 2>...\n" 
		"   1.2 \"-o\" : Save password, optional comments and automatic timestamp as CSV entry to a file.\n"
		"   Example  : mclipm g -l 16 -o foo.csv \"This is a comment\"\n"
                "           -> Nothing is displayed except in case of errors.\n"
		"           -> Appends the 16-char password to \"foo.csv\", like so :\n"
		"              \"Thu Jun 09 18:00:12 2016\",\"a79&8*kjn!ok)$31\",\"This is a comment\"\n"
		"   Format   : mclipm g -o [filename (optional, default: \"mclipm.csv\")] [comments (optional, default: \"\")]...\n"
		"   1.3 \"-r\" : Specify range of password's  ASCII characters.\n"
		"              By default, the range is \"!~\", i.e. for any char, \'~\' >= char >= \'!\'.\n"
	        "              This allows any non-whitespace printable (English) chars. You may consult a chart.\n"
		"   Example  : mclipm g -r \" \"\n"
		"           -> The lower limit char is now \' \'(space); password may include spaces.\n"
		"   Format   : mclipm g -r \"<lower_limit>[upper_limit(optional, default \'~\')]\"\n"
		"\n"
		"2. \"s\" :\n"
		"   Save a password with optional comments, automatic timestamp & encoded as CSV\n"
		"   Example : mclipm s 12345\n"
		"          -> Saves the password \"12345\" with empty (\"\") comments to mclipm.csv\n"
		"             mclip s 1234 \"strong & complicated password\" \"donread.csv\"\n"
		"          -> Saves the same password but with \"strong...\" comment and to \"donread.csv\"\n"
		"   Format  : mclipm s <password> [comments (optional)] [filename (optional, mclipm.csv default)]\n"
		"\n"
		"3. \"f\" :\n"
		"   Searches for an entry case-insensitively with sub-string matching, and displays it.\n"
		"   Example : mclipm f \"\" \"foo@gmail.com\"\n"
		"          -> Searches for entry with instance of foo@gmail.com in default (mclipm.csv) file.\n"
		"          -> Multiple search terms, say like \"mclipm f x.csv gmail arch\", also possible.\n"
		"   Format  : mclipm f <filename (if empty, saves to mclipm.csv)> <search term> [search term]...\n"
		"\n"
		"4. \"d\" : Same as \"f\"; except it deletes an entry with a match.\n"
		,argc < 2? stderr : stdout);
                exit(argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS);
        }

	switch(*argv[1]) {
        case 'g' : case 'G' :
        {
		struct opts options = getopts(argv);
		char *pswd = mkpswd(options.pswd_len, options.range.upper, options.range.lower);

		if(pswd) {
			if(options.save_to) {
				FILE *fp = fopen(options.save_to,"a");
				if(fp) 
					write_entry(fp, pswd, options.comments);
				else {
					fprintf(stderr,"Error : Couldn't open file to save : %s\n",
							errno? strerror(errno) : "unknown error");
					free(pswd);
					exit(EXIT_FAILURE);
				}
			} else
				printf("%s\n",pswd);
			free(pswd);
		} else {
			fprintf(stderr,"Error : Couldn't make password%s%s\n",
					errno? " : " : "",
					errno? strerror(errno) : "");
			exit(EXIT_FAILURE);
		}
		break;
        }
        case 's' : case 'S' :
        {
		if(argc >= 3 ) {
			char *pswd = argv[2], *comments, *fname;
			if(argc >= 4) {
				comments = argv[3];
				if(argc >= 5)
					fname = argv[4];
				else
					fname = DEFAULT_PWD_FILE;
			} else {
				comments = ""; 
				fname = DEFAULT_PWD_FILE;
			}

			FILE *fp = fopen(fname,"a");
			if(fp)
				write_entry(fp,pswd,comments);
			else {
				fprintf(stderr, "Error : Couldn't open file to save%s%s\n",
						errno? " : " : "",
						errno? strerror(errno) : "");
				exit(EXIT_FAILURE);
			}
		} else {
			fputs("Error : Not enough arguments.\n"
			      "For help, try : mclipm h\n", stderr);
			exit(EXIT_FAILURE);
		}
		break;
	}
        case 'f' : case 'F' : case 'd' : case 'D' :
	{
		if(argc >= 4) {
			bool deleting = *argv[1] == 'd' || *argv[1] == 'D';
			find_del(*argv[2]? argv[2] : DEFAULT_PWD_FILE,deleting,argv+3);
		} else {
			fputs("Error : Not enough arguments.\n"
			      "For help, try : mclipm h\n", stderr);
			exit(EXIT_FAILURE);
		}	
		break;
	}
        default :
                fputs("Error : Unrecognised arguments.\n"
		      "For help, try : mclipm h\n", stderr);
                exit(EXIT_FAILURE);
	}
        exit(EXIT_SUCCESS);
}
