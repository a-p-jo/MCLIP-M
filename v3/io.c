#include "mclipm.h"

struct opts getopts(const char *const restrict *restrict argv)
{
	/* Intitialize to defaults */
	struct opts opts = {
		.pwdlen = DEFAULT_PWD_LEN, 
		.outfile = NULL,
		.comments = "",
		.chrange = {
			.max = DEFAULT_PWD_MAXCHAR,
			.min = DEFAULT_PWD_MINCHAR
		}
	};

	size_t  l = 0, o = 0, r = 0; /* Indexes for options */
	
	for (size_t i = 2; argv[i]; i++) {
		if(!l && streq("-l", argv[i])) /* Once set, constant */
			l = i;
		else if(!o && streq("-o", argv[i]))
			o = i;
		else if(!r && streq("-r", argv[i]))
			r = i;
		else if(o && l && r) /* Break if all matches found */
			break;
	}

	if (o) {
		size_t i = UCHKDSUM(size_t, 1, o), j = UCHKDSUM(size_t, 1, i);
		if(i && argv[i] && !strempty(argv[i])) {
			opts.outfile = argv[i];
			if(j && argv[j])
				opts.comments = argv[j];
		} else /* With no/empty arg, save to default file */
			opts.outfile = DEFAULT_PWD_FILE_NAME;
	}

	if (l) {
		size_t i = UCHKDSUM(size_t, 1, l);
		if(i && argv[i]) {
			if(!strtosize_t(&opts.pwdlen, argv[i]))
				ERROR_EXIT("Invalid length");
		} else 
			ERROR_EXIT("No lenght argument following \"-l\".");
	}

	if (r) {
		size_t i = UCHKDSUM(size_t, 1, r);
		if(i && argv[i] && !strempty(argv[i])) {
			opts.chrange.min = argv[i][0];
			
			if(argv[i][1]) /* Upper range arg is optional */
				opts.chrange.max = argv[i][1];
			if(opts.chrange.min >= opts.chrange.max)
				ERROR_EXIT("Invalid range");
		} else 
			ERROR_EXIT("No range argument following \"-r\".");
	}

	return opts;
}

void mkentry(FILE *restrict dst, const char *restrict pswd, const char *restrict comments)
{
	char *tm = csvfmt(time_str()), *pwd = csvfmt(pswd), *cmnt = csvfmt(comments);

	if (!tm || !pwd || !cmnt) err : {
		free(tm), free(pwd), free(cmnt);
		ERROR_EXIT("Couldn't save entry to file");
	}
		
	if (fprintf(dst, "%s,%s,%s\n", tm, pwd, cmnt) < 0)
		goto err;
	else
		free(tm), free(pwd), free(cmnt);
}

void find_del(const char *restrict fname, bool deleting, const char *const restrict *restrict terms)
{                      
	FILE *f = fopen(fname, "r");
	if (!f)
		ERROR_EXIT("Couldn't open file to read");
	
	/* deleting means copying retained content over to a temp file,
	 * removing the original, and renaming the temp file to it.
	 */
	FILE *tmp;
	if (deleting) {
		tmp = fopen(DEFAULT_TEMPORARY_FILE_NAME, "w");
		if (!tmp)
			ERROR_EXIT("Couldn't open temporary file to write changes");
	}

	dystr buf = {0};
	uintmax_t matches = 0;
	for(uintmax_t line = 1 ;; buf.len = 0, line++) {
		if (!getln(&buf, f)) {
			if (feof(f))
				break;
			else {
				if (deleting)
					fclose(tmp), remove(DEFAULT_TEMPORARY_FILE_NAME);
				ERROR_EXIT("Couldn't allocate line buffer");
			}
		}
		
		bool found = false;
		char *arr = dystr_arr(&buf);
		for (int i = 0; terms[i]; i++) {
			if (iscasesubstr(arr, terms[i])) {
				found = true;
				break;
			}
		}
		if (found)
			printf("%ju : %s\n", line, rm_newline(arr, buf.len)), matches++;
		else if(deleting && fputs(arr, tmp) == EOF) {
			fclose(tmp), remove(DEFAULT_TEMPORARY_FILE_NAME);
			ERROR_EXIT("Couldn't save to file");
		}
	}
	dystr_destroy(&buf);

	if (deleting) {
		if(fclose(tmp) != 0)
			ERROR_EXIT("Couldn't save changes to temp file");
		fclose(f);
		remove(fname);
		if(rename(DEFAULT_TEMPORARY_FILE_NAME, fname) != 0) 
			ERROR_EXIT("Couldn't save changes to temp file");
	}

	printf("%ju match%s found.\n", matches, matches != 1? "es" : "");
}
