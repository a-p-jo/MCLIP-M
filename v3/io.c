#include "mclipm.h"

struct opts getopts(char **argv)
{
	/* Intitialize to defaults */
	struct opts opts = {
		.pwdlen = DEFAULT_PWD_LEN, 
		.outfile = NULL,
		.comments = "",
		.range = {
			.upper = DEFAULT_RANGE_UPPER,
			.lower = DEFAULT_RANGE_LOWER
		}
	};

	size_t  l = 0, o = 0, r = 0; /* Indexes for options */
	
	for(size_t i = 2; argv[i]; i++) {
		if(!l && streq("-l", argv[i])) /* Once set, constant */
			l = i;
		else if(!o && streq("-o", argv[i]))
			o = i;
		else if(!r && streq("-r", argv[i]))
			r = i;
		else if(o && l && r) /* Break if all matches found */
			break;
	}

	if(o) {
		size_t i = CHKDADD(1, o), j = CHKDADD(1, i);
		if(i != OVRFLW && argv[i] != NULL && !strempty(argv[i])) {
			opts.outfile = argv[i];
			if(j != OVRFLW && argv[j] != NULL)
				opts.comments = argv[j];
		} else /* With no/empty arg, save to default file */
			opts.outfile = DEFAULT_PFILE;
	}

	if(l) {
		size_t i = CHKDADD(1, l);
		if(i != OVRFLW && argv[i] != NULL) {
			if(!strtosize_t(&opts.pwdlen, argv[i]))
				ERROR_EXIT("Invalid length");
		} else 
			ERROR_EXIT("No lenght argument following \"-l\".");
	}

	if(r) {
		size_t i = CHKDADD(1, r);
		if(i != OVRFLW && argv[i] != NULL && !strempty(argv[i])) {
			opts.range.lower = argv[i][0];
			
			if(argv[i][1] != '\0') /* Upper range arg is optional */
				opts.range.upper = argv[i][1];

			if(opts.range.lower >= opts.range.upper) 
				ERROR_EXIT("Invalid range");
		} else 
			ERROR_EXIT("No range argument following \"-r\".");
	}

	return opts;
}

void mkentry(FILE *dst, const char *restrict pswd, const char *restrict comments)
{
	char *tm = csv_fmt(time_str()), *pwd = csv_fmt(pswd), *cmnt = csv_fmt(comments);

	if(tm == NULL || pwd == NULL || cmnt == NULL) 
	error :
	{
		free(tm);
		free(pwd);
		free(cmnt);
		ERROR_EXIT("Couldn't save entry to file");
	}
		
	if(fprintf(dst, "%s,%s,%s\n", tm, pwd, cmnt) < 0)
		goto error;
	else {
		free(tm);
		free(pwd);
		free(cmnt);
	}
}

void find_del(const char *restrict fname, bool deleting, char **terms)
{                      
	FILE *f = fopen(fname, "r");
	if(f == NULL)
		ERROR_EXIT("Couldn't open file to read");
	
	/* deleting means copying retained content over to a temp file,
	 * removing the original, and renaming the temp file to it.
	 */
	FILE *tmp;
	if(deleting) {
		tmp = fopen(TMPF, "w");
		if(tmp == NULL)
			ERROR_EXIT("Couldn't open temporary file to write changes");
	}

	struct dystr buf = dystr_create(DYSTR_INITCAP); 
	uintmax_t matches = 0;
	for(uintmax_t line = 1 ;; line++) {
		if(getln(&buf, f) == NULL) {
			if(deleting) {
				fclose(tmp); 
				remove(TMPF);
			}
			ERROR_EXIT("Couldn't allocate line buffer");
		} 

		if(strempty(buf.arr)) /* EOF */
			break;

		bool found = false;
		for(int i = 0; terms[i] != NULL; i++) {
			if(strcasestr(buf.arr, terms[i])) {
				found = true;
				break;
			}
		}
		if(found) {
			printf("%ju : %s\n", line, rm_newline(buf.arr, buf.len-1));
			matches++;
		} else if(deleting && fputs(buf.arr, tmp) == EOF) {
			fclose(tmp);
			remove(TMPF);
			ERROR_EXIT("Couldn't save to file");
		}

		buf.len = 0; /* clear buffer & reuse for next line. */
	}

	dystr_destroy(&buf);

	if(deleting) {
		if(fclose(tmp) != 0)
			ERROR_EXIT("Couldn't save changes to temp file");
		fclose(f);
		remove(fname);
		if(rename(TMPF, fname) != 0) 
			ERROR_EXIT("Couldn't save changes to temp file");
	}

	printf("%ju match%s found.\n", matches, matches != 1? "es" : "");
}
