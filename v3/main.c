#include "mclipm.h"

int main(int argc, const char *const *argv)
{              
        if (argc < 2 || streq("-h",argv[1])) {
                fprintf( argc < 2 ? stderr : stdout,
		"%s v"MCLIPM_VERSION" : Minimalist CLI Password Manager.\n"
		"See https://github.com/a-p-jo/mclipm for git repo.\n"
		"\n"
		"4 main commands : \"g\"(generate), \"s\"(save), \"f\"(find), \"d\"(delete)\n"
		"\n"
		"1. \"g\" :\n"
		"   Generates a random password.\n"
		"   Example : mclipm g\n"
		"          -> Displays a random %d-char (no spaces) password.\n"
		"   Format  : mclipm g [option] <option's argument>...\n"
		"\n"
                "   \"g\" may be given any combination of 3 options, in any order :\n"
		"\n"
		"   1.1 \"-l\" : Specify length of password.\n"
		"   Example  : mclipm g -l 16\n"
		"           -> Resulting password is 16 chars long.\n"
		"   Format   : mclipm g -l <n, {0 < n < SIZE_MAX-1}>...\n" /* +1 for '\0' */
		"   1.2 \"-o\" : Specify outfile and (optionally) comments.\n"
		"   Example  : mclipm g -l 16 -o foo.csv \"xyz.com, uname abc\"\n"
                "           -> Nothing is displayed except in case of errors.\n"
		"           -> Appends CSV-formatted output to a new line in \"foo.csv\" :\n"
		"              \"Thu Jun 09 18:00:12 2016\",\"a79&8*kjn!ok)$31\",\"xyz.com, uname abc\"\n"
		"   Format   : mclipm g -o [filename (optional, default: \"mclipm.csv\")] [comments (optional, default: \"\")]...\n"
		"   1.3 \"-r\" : Specify range of password's  ASCII characters.\n"
		"              By default, the range is \"!~\", i.e. \'~\' >= char >= \'!\'.\n"
	        "              This allows all non-whitespace printable ASCII chars.\n"
		"   Example  : mclipm g -r \" \"\n"
		"           -> The lower limit is now \' \'(space); password may include spaces.\n"
		"   Format   : mclipm g -r \"<lower_limit>[upper_limit(optional, default \'~\')]\"\n"
		"\n"
		"2. \"s\" :\n"
		"   Save a password with optional comments, automatic timestamp; formatted as CSV\n"
		"   Examples: mclipm s 12345\n"
		"          -> Saves the password \"12345\" with empty (\"\") comments to mclipm.csv\n"
		"   Format  : mclipm s <password> [comments] [filename (optional, default: \"mclipm.csv\")]\n"
		"\n"
		"3. \"f\" :\n"
		"   Find any entries in given file which matches given search terms.\n"
		"   Example : mclipm f \"\" \"x.com\"\n"
		"          -> Searches for entry with instance of \"x.com\" in default (mclipm.csv) file.\n"
		"          -> Multiple terms, like \"foo\", \"BAR\" in \"mclipm f x.csv foo BAR\", also possible.\n"
		"   Format  : mclipm f <filename (if empty, saves to mclipm.csv)> <search term>...\n"
		"\n"
		"4. \"d\" : Same as \"f\"; except it deletes any entries with a match.\n",
		argv[0], DEFAULT_PWD_LEN);
                exit(argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS);
        }

	if (streq("g", argv[1])) {
		struct opts opts = getopts(argv);
		char *pwd = mkpwd(opts.pwdlen, opts.chrange.min, opts.chrange.max);
		if (pwd) {
			if (opts.outfile) {
				FILE *f = fopen(opts.outfile, "a");
				if (f) 
					mkentry(f, pwd, opts.comments);
				else {
					free(pwd);
					ERROR_EXIT("Couldn't open file");
				}
			} else
				puts(pwd);
			free(pwd);
		} else 
			ERROR_EXIT("Couldn't generate password");

        } else if (streq("s", argv[1])) {
		if (argc >= 3) {
			const char *pwd = argv[2], *comments, *fname;
			if (argc >= 4)
				comments = argv[3], fname = argc >= 5? argv[4] : DEFAULT_PWD_FILE_NAME;
			else
				comments = "", fname = DEFAULT_PWD_FILE_NAME;

			FILE *f = fopen(fname, "a");
			if (f)
				mkentry(f, pwd, comments);
			else
				ERROR_EXIT("Couldn't open file");
		} else
			ERROR_EXIT("Not enough arguments.\nFor help : mclipm -h\n");

	} else if (streq("f", argv[1]) || streq("d", argv[1])) {
		if (argc >= 4)
			find_del(strempty(argv[2])? DEFAULT_PWD_FILE_NAME : argv[2], streq("d", argv[1]), argv+3);
		else
			ERROR_EXIT("Not enough arguments.\nFor help : mclipm -h\n");
	} else
		ERROR_EXIT("Unrecognised arguments.\nFor help : mclipm -h\n");

        exit(EXIT_SUCCESS);
}
