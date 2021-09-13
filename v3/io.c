#include "mclipm.h"

struct opts getopts(char **argv)
{
	/* Intitialize to defaults */
	struct opts input = {
		.pswd_len = DEFAULT_PWD_LEN, 
		.save_to = NULL,
		.comments = "",
		.range = {
			.upper = DEFAULT_RANGE_UPPER,
			.lower = DEFAULT_RANGE_LOWER
		}
	};

	/* Position holders for options in argv */
	size_t  l = 0, o = 0, r = 0;
	
	for(size_t i = 2; argv[i]; i++) {
		/* !l : once option set, reassignments ignored */
		if(!l && !strcmp("-l",argv[i]))
			l = i;
		else if(!o && !strcmp("-o",argv[i]))
			o = i;
		else if(!r && !strcmp("-r",argv[i]))
			r = i;
		/* Stop iterating if all possible options specified */
		else if(o && l && r)
			break;
	}

	if(o) {
		size_t arg1 = CHKDADD(1,o), arg2 = CHKDADD(2,o);
		if(arg1 && argv[arg1] && *argv[arg1]) {
			input.save_to = argv[arg1];
			if(arg2 && argv[arg2])
				input.comments = argv[arg2];
		}
		/* With no/empty arg, save to default file */
		else
			input.save_to = DEFAULT_PWD_FILE;
	}

	if(l) {
		size_t arg = CHKDADD(1,l);
		if(arg && argv[arg] && *argv[arg]) {
			char *err = strtosize_t(argv[arg],&input.pswd_len);
			if(err) {
				fprintf(stderr, "Bad lenght \"%s\" : %s\n", argv[arg], err);
				exit(EXIT_FAILURE);
			}
			
		} else {
			fputs("No length argument following \"-l\".\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	if(r) {
		size_t arg = CHKDADD(1,r);
		if(arg && argv[arg] && argv[arg][0]) {
			input.range.lower = argv[arg][0];
			
			if(argv[arg][1])
				input.range.upper = argv[arg][1];

			if(input.range.lower >= input.range.upper) {
				fprintf(stderr,"Invalid range : Upper limit \'%c\'(%u) not greater than lower limit \'%c\'(%u)\n",
						input.range.upper, input.range.upper,
						input.range.lower, input.range.lower);
				exit(EXIT_FAILURE);
			}
		} else {
			fputs("No range argument following \"-r\".\n",stderr);
			exit(EXIT_FAILURE);
		}
	}

	return input;
}

void write_entry(FILE *restrict fp, char *restrict pswd, char *restrict comments)
{
	char *time = time_str();
	if(!time)
		time = "";
	
	time = csv_fmt(time);
	pswd = csv_fmt(pswd);
	comments = csv_fmt(comments);

	if(!time || !pswd || !comments) 
	error :
	{
		free(time);
		free(pswd);
		free(comments);

		fprintf(stderr,"Error : Couldn't save entry to file%s%s\n",
				errno? " : " : "",
				errno? strerror(errno) : "");
		exit(EXIT_FAILURE);
	}
		
	if(fprintf(fp,"%s,%s,%s\n",time,pswd,comments) < 0 || fflush(fp))
		goto error;
	else { 
		free(pswd);
		free(time);
		free(comments);
	}
}

void find_del(const char *restrict fname, const bool deleting, char **terms)
{                      
	FILE *fp = fopen(fname,"r");
	if(!fp) {
		fprintf(stderr, "Error : Couldn't open file to read%s%s\n",
				errno? " : " : "",
				errno? strerror(errno) : "");
		exit(EXIT_FAILURE);
	}
	
	FILE *tmp = NULL;
	if(deleting) {
		tmp = fopen(TMP_FNAME,"w");
		if(!tmp) {
			fprintf(stderr,"Error : Couldn't open temporary file to write changes%s%s\n",
					errno? " : " : "",
					errno? strerror(errno) : "unknown error");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Make all terms lowercase */
	for(size_t i = 0; terms[i]; i++) {
		for(size_t q = 0; terms[i][q]; q++)
			terms[i][q] = tolower(terms[i][q]);
	}


	/* Initialize dynamic strings that will hold a line and its lowercase copy */
	struct dystr buf = {.str = NULL, .sz = 0, .len = 0},
		     buf_lower = {.str = NULL, .sz = 0, .len = 0};

	uintmax_t matches = 0;
	for(uintmax_t line = 1;;line++) { 
		if(!getln(fp,&buf))  
		buf_err :
		{
			fprintf(stderr,"Error : Couldn't buffer line %ju%s%s\n",
					line, errno? " : " : "", 
					errno? strerror(errno) : "unknown error");
			cleanup :
			free(buf.str); free(buf_lower.str);
			if(deleting) {
				fclose(tmp);
				if(!remove(TMP_FNAME))
					fputs("Couldn't delete temp file \""TMP_FNAME"\"...\n",stderr);
			}
			exit(EXIT_FAILURE);
		}
		else if(buf.str[0] == '\0')
			break; /* Empty string == EOF crossed */

		if(!strlower(&buf_lower,buf.str))
			goto buf_err;

		bool found = false;
		for(int i = 0; terms[i]; i++) {
			if(strstr(buf_lower.str,terms[i])) {
				found = true;
				break; /* Only 1st match matters */
			}
		}

		if(found) {
			printf("%ju : %s\n", line, rm_newline(buf.str,buf.len));
			matches++;
		} else if(deleting && (fputs(buf.str,tmp) == EOF || fflush(tmp))) {
			fprintf(stderr,"Error : Couldn't save to file%s%s\n",
					errno? " : " : "",
					errno? strerror(errno) : "");
			goto cleanup;
		}
	}

	free(buf.str); free(buf_lower.str);
	if(deleting) {
		if(fclose(tmp))
		{
			fprintf(stderr,"Error : Couldn't save changes to temp file : %s\n",
					errno? strerror(errno) : "unknown error");
			exit(EXIT_FAILURE);
		}

		fclose(fp);
		remove(fname);

		if(rename(TMP_FNAME,fname))
		{
			fprintf(stderr,"Error : Couldn't save changes to file : %s\n",
					errno? strerror(errno) : "unknown error");
			exit(EXIT_FAILURE);
		}
	}
	printf("%ju match%s found.\n", matches, matches != 1? "es" : "");
}
