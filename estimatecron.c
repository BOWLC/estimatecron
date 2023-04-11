/* ABOUT:
 * Project 1 for 2022 for UWA's CITS2002 course.
 * The purpose for me to complete this project was to refresh and build my C 
 * programming skills.
 *
 * USAGE: Enter month(e.g. aug, 4), crontab file name, estimates file name, as program parameters in order.
 *
 * Check validity of a crontab file by 'simulating' the programs running,
 * based on the estimated length of each run in estimates file.
 * Does NOT check for leap year TODO
 * Only set for 2022 TODO
 *
 * 	Author: Lachlan Campbell
 * 	Date: 11/4/2022
 * 	*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define LINE_SIZE 100
#define LINE_AMOUNT 20
#define COMMAND_MAX 40

#define DEFAULT_YEAR 2022

/* Struct pointed to by CRONTAB struct
 * each instance is a program that is currently 'running'.
 * linked list implementation. End list (NULL ptr) will always stop first
 * */
typedef struct runNode
{
    int timer;
    struct runNode *next;
} CRONRUN;

/* CRONTAB structure for simulating schedule
 * when int = -1, '*' equivalent; program will run on any value.
 * */
typedef struct
{
	int min;
	int hour;
	int day;
	int month;
	int dayweek;
	char *name;
    int time;
	int count;//# of times run
    CRONRUN *runner;
} CRONTAB;

const char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
const char days[7][4] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
const int daysinmonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

int countRunner(CRONTAB *crontab);
void addRunner(CRONTAB *crontab);
void decRunner(CRONTAB *crontab);
void simulate(CRONTAB schedule[], int numcron, char *mostfreqname, int *totalruns, int *maxsimul, int month);
void makeCrontab(char cronargs[][6][COMMAND_MAX+1], char estargs[][6][COMMAND_MAX+1], int numcron, int numestimates, CRONTAB file[]);
int wordInt(char *word);
int dayInt(char *dayweek);
int monthInt(char *month);
int checkMonthWord(char *arg);
int checkDayWord(char *arg);
int checkStar(char *arg);
int checkMin(int minute);
int checkHour(int hour);
int checkDayMonth(int day);
int checkMonth(int month);
int checkDayWeek(int day);
int checkIntArg(char *a);
int checkArgLen(int maxlen, int charpos);
void addArg(char *dest, char *word, int wordlen);
void printargs(char fileargs[][6][COMMAND_MAX+1], int numCron, int numargs);
int getWordLen(int Type, int wordno);
int getFile(char *argv, char fileargs[][6][COMMAND_MAX+1], int* numCron, int Type);
int checkCron(char fileargs[][6][COMMAND_MAX+1], int linenum);
int checkEstimates(char fileargs[][6][COMMAND_MAX+1], int linenum);

int main(int argc, char *argv[]){

	/* initial error checking */
	if(!(argc == 4)){
		printf("Error: Incorrect # of parameters passed to program.\n");
		printf("Usage: Enter 3 parameters:\n");
		printf("1. Month to 'simluate' crontab execution (e.g. 1, 2, feb, dec)\n");
		printf("2. Filename of crontab file\n");
		printf("3. Filename of Estimates file\n");
		printf("==EXITING==\n");
		exit(EXIT_FAILURE); 
	}
	for(int i = 0 ; i < argc; i++){
		if(strlen(argv[i]) == INT_MAX){
			printf("Error: parameter too long.\n");
			exit(EXIT_FAILURE);
		}
	}
	char cronwords[LINE_AMOUNT][6][COMMAND_MAX+1];
	char estimates[LINE_AMOUNT][6][COMMAND_MAX+1];
	int numcron = 0;
	int numestimates = 0;
	int* numcronp = &numcron;
	int* numestimatesp = &numestimates;

	getFile(argv[2],cronwords, numcronp, 0);//crontab
	getFile(argv[3],estimates, numestimatesp, 1); 
	printargs(cronwords, numcron, 6);
	printargs(estimates, numestimates, 2);

	if(checkCron(cronwords, numcron)){
		printf("\nError: Incorrect argument values in cron file\n");
		exit(EXIT_FAILURE);
	}
	if(checkEstimates(estimates, numcron)){
		printf("\nError: Incorrect argument values in estimates file\n");
		exit(EXIT_FAILURE);
	}
	
	//files valid, fill structs
	CRONTAB schedule[numcron];
	makeCrontab(cronwords, estimates, numcron, numestimates, schedule);

	printf("PRINTING CRONTAB FROM STRUCTURE\n");

    for(int i = 0; i < numcron; i++){
        CRONTAB pr = schedule[i];
        printf("%s: %d, %d, %d, %d, %d, takes %d mintes\n",pr.name, pr.min, pr.hour, pr.day, pr.month, pr.dayweek, pr.time);
    }

	int month;
	if(!checkIntArg(argv[1]) && !checkMonth(atoi(argv[1]))){
		month = atoi(argv[1]);
	}
	else if(!checkMonthWord(argv[1])) month = monthInt(argv[1]);
	else{
		printf("argv[1] incorrect format.\n");
		exit(EXIT_FAILURE);
	}

    char mostfreqname[COMMAND_MAX+1]; 
    int totalruns = 0;
    int *totalrunsp = &totalruns;
    int maxsimul = 0;
    int *maxsimulp = &maxsimul;
    simulate(schedule, numcron, mostfreqname, totalrunsp, maxsimulp, month); 
    printf("Most Frequently run program: %s\t|\tTotal Runs: %d\t|\tMaximum Simulatenous Programs: %d\n", mostfreqname, totalruns, maxsimul);
    //printf("%s %d %d\n", mostfreqname, totalruns, maxsimul);
	return 0;
}	
/* Create new runner to crontab list
 * */
void addRunner(CRONTAB *crontab){
    CRONRUN *newrun;
    CRONRUN *runw;
    if(!(newrun = (CRONRUN*)malloc(sizeof(CRONRUN)))){
        printf("Memory overflow\n");
        exit(EXIT_FAILURE);
    }
    runw = crontab->runner;
    newrun->timer = crontab->time;
    newrun->next =  runw;
    crontab->runner = newrun;
    crontab->count++;
}

void decRunner(CRONTAB *crontab){
    CRONRUN *runpre = NULL;
    CRONRUN *runnext;
    runnext = crontab->runner;
    while(runnext != NULL){
        runnext->timer--;
        //if(runnext->timer%5 == 0)printf("Program %s time left:%d\n", crontab->name, runnext->timer);
        if(runnext->timer == 0){
            if(runpre == NULL){
                crontab->runner = NULL;
            }
            else{
                runpre->next = NULL;
            }
            free(runnext);
            break;
        }
        else{
            runpre = runnext;
            runnext = runnext->next;
        }
    }
}

int countRunner(CRONTAB *crontab){
//    printf("counting runner\n");
    CRONRUN *runw;
    int count = 0;
    runw = crontab->runner;
    while(runw != NULL){
        runw = runw->next;
        count++;
    }
    return count;
}
/* 'Simluates' Each second in the month and begins running of scheduled programs
 * the same program can run in multiple instances at once (linked list CRONRUN in struct CRONTAB)
 * */
void simulate(CRONTAB schedule[], int numcron,char *mostfreqname, int *totalruns, int *maxsimul, int month){  
    int daymonth = daysinmonth[month] +1;
    int dayweek;//days from sunday (weekday)
    int total_runs = 0;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));//set values to 0 for autofill
    tm.tm_year = DEFAULT_YEAR -1900;
    tm.tm_mon = month;
    tm.tm_mday = 1;
    mktime(&tm);
    dayweek = tm.tm_wday;

    for(int day = 1; day < daymonth; day++){
        int dayweekc = (day + dayweek -1) % 7;   
        for(int hour = 0; hour < 24; hour++){
            for(int minute = 0;minute < 60; minute++){
                //Decrement timers on active processes
                for(int i=0; i < numcron; i++){
                    CRONTAB *cronw;
                    cronw = &schedule[i];
                    if(cronw->runner != NULL){
                        decRunner(cronw);
                    }
                }
                //start processes
                for(int i = 0; i < numcron; i++){
                    CRONTAB *cronw;
                    cronw = &schedule[i];
                    if(!(cronw->month == month || cronw->month == -1)) continue;
                    if(!(cronw->day == day || cronw->day == -1)) continue; 
                    if(!(cronw->hour == hour || cronw->hour == -1)) continue;
                    if(!(cronw->min == minute || cronw->min == -1)) continue;
                    if(!(cronw->dayweek == dayweekc || cronw->dayweek == -1)) continue;
                    printf("begin running %s at %02d:%02d %d/%d/%d...\n", cronw->name, minute, hour, day, month, DEFAULT_YEAR);
                    addRunner(cronw); //start process
                    total_runs++;
                }
                
                int numsim = 0;
                //count simultaneous processes
                for(int i=0; i < numcron; i++){
                    CRONTAB *cronw;
                    cronw = &schedule[i];
                    if(cronw->runner != NULL){
                        int numthis;
                        numthis = countRunner(cronw);
                        numsim+= numthis;
                    }
                } 
                if(numsim>*maxsimul) *maxsimul = numsim;
            }
        }
    }
    //find most often run program
    char mostrunname[COMMAND_MAX+1] = "NO PROGRAMS WERE RUN";
    int maxrun = 0;
    for(int i=0; i < numcron; i++){
        CRONTAB *cronw;
        cronw = &schedule[i];
        printf("program %s was run %d times\n", cronw->name, cronw->count);
        if(cronw->count > maxrun){
            maxrun= cronw->count;
            strcpy(mostrunname, cronw->name);
        }
    }
    strcpy(mostfreqname, mostrunname);
    *totalruns = total_runs;
}

/* Move words/arguments from ***char to CRONTAB struct
 * */
void makeCrontab(char cronargs[][6][COMMAND_MAX+1], char estargs[][6][COMMAND_MAX+1], int numcron, int numestimates, CRONTAB schedule[]){
	for(int i = 0 ; i < numcron ; i++){
        CRONTAB *cronw = &schedule[i];
		cronw->name = cronargs[i][5];
		for(int j = 0; j < numestimates; j++){
			if(!strcmp(cronw->name,estargs[j][0])){
				cronw->time = atoi(estargs[j][1]);
				break;
			}
		}
		cronw->min = wordInt(cronargs[i][0]);
		cronw->hour = wordInt(cronargs[i][1]);
		cronw->day = wordInt(cronargs[i][2]);
		cronw->month = monthInt(cronargs[i][3]);
		cronw->dayweek = dayInt(cronargs[i][4]);
        cronw->count = 0;
        cronw->runner = NULL;
	}
}

int wordInt(char *mhd){
	if(*mhd == '*') return -1;
	return atoi(mhd);
}

/* TODO: use hashmaps in dayInt, monthInt
 * */
int dayInt(char *dayweek){
	if(*dayweek == '*') return -1;
	for(int i = 0 ; i < 7; i++){
		if(!strcmp(dayweek,days[i])) return i;
	}
	return atoi(dayweek);
}

int monthInt(char *month){
	if(*month == '*') return -1;
	for(int i = 0; i < 12; i++){
		if(!strcmp(month,months[i])) return i;
	}
	return atoi(month);	
}

/* ===checkCron===
 * check each argument is correct type and range in cron file
 * TODO: improve the dictionary search with hash maps
 */
int checkCron(char fileargs[][6][COMMAND_MAX+1], int linenum){
	printf("CHECKING CRON FILE --------\n");
	for(int i = 0; i < linenum; i++){
		printf("\n");
		for(int j = 0; j < 5; j++){
				printf("\t%s/%d",fileargs[i][j], atoi(fileargs[i][j]));
			switch(j){
				case 0:/*minute*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j]))	return 1;
					if(checkMin(atoi(fileargs[i][j])))return 1;
					break;
				case 1:/*hour*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])) return 1;
					if(checkHour(atoi(fileargs[i][j])))return 1;
					break;
				case 2:/*day of month*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])) return 1;
					if(checkDayMonth(atoi(fileargs[i][j])))return 1;
					break;
				case 3:/*month*/
					if(!checkStar(fileargs[i][j])) break;
					if(!checkMonthWord(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])) return 1;
					if(checkMonth(atoi(fileargs[i][j])))return 1;
					break;
				case 4:/*day of week*/
					if(!checkStar(fileargs[i][j])) break;
					if(!checkDayWord(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])) return 1;
					if(checkDayWeek(atoi(fileargs[i][j])))return 1;
					break;
			}
		}
	}
	printf("\n");
	return 0;
}

int checkEstimates(char fileargs[][6][COMMAND_MAX+1], int linenum){
	printf("CHECKING ESTIMATES FILE --------\n");
	for(int i = 0; i < linenum; i++){
		printf("\n\t%s/%d",fileargs[i][1], atoi(fileargs[i][1]));
		if(checkIntArg(fileargs[i][1])) return 1;
	}
	printf("\n");
	return 0;
}

int checkMonthWord(char *a){
	for(int i = 0; i < 12; i++){
		if(!strcmp(a, months[i])) return 0;
	}
	return 1; //no match
}

int checkDayWord(char *a){
	for(int i = 0; i < 7; i++){
		if(!strcmp(a,days[i])) return 0;
	}
	return 1; //no match
}

int checkStar(char *a){
	if(*a  == '*' && a[1] == '\0') return 0;
	return 1;
}
int checkMin(int minute){
	//Is minute within range
	if(minute < 0 || minute > 59) return 1;
	//return 0 for in range
	return 0;
}

int checkHour(int hour){
	//Is hour within range
	if(hour < 0 || hour > 23) return 1;
	//return 0 for in range
	return 0;
}

int checkDayMonth(int day){
	//Is day within range
	if(day < 1 || day > 31) return 1;
	//return 0 for in range
	return 0;
}

int checkMonth(int month){
	//Is month within range
	if(month < 0 || month > 11) return 1;
	//return 0 for in range
	return 0;
}

int checkDayWeek(int day){
	//Is year within range
	if(day < 0 || day > 6) return 1;
	//return 0 for in range
	return 0;
}

int checkIntArg(char *a){
	for(; *a != '\0' ; a++){
		if(*a < '0' || *a > '9') return 1; //not an integer or *
	}
	return 0;
}

int checkArgLen(int maxlen, int charpos){
	if(charpos> maxlen){
		return 1;
	}
	return 0;

}

void addArg(char *dest, char *word, int wordlen){
	for(int i = 0; i < wordlen+1; i++){
		*dest = *word;
		++dest;
		++word;
	}
	return;
}

void printargs(char fileargs[][6][COMMAND_MAX+1], int numCron, int numargs){

	printf("PRINTING %d ARGUMENTS FROM ARRAY:\n", numCron);
	//each line
	for(int i = 0; i < numCron; i++){

		for(int j = 0; j < numargs; j++){
			printf("%s\t", fileargs[i][j]);
		}
		printf("\n");
	}
	return;

}

int getWordLen(int Type, int wordno){
	/* Type = 0 crontab-file, 1 is estimates-file*/

	int wordlen;
	switch(Type){
		case 0:if( wordno < 3){//ints
				wordlen = 2; 
			} else if( wordno < 4){
				wordlen = 3; // day or month e.g. "mon", "aug"
			} else{
				wordlen = COMMAND_MAX;
			}
			break;
	       
		case 1:	if(wordno == 0) wordlen = COMMAND_MAX;
			else wordlen = 10;//max 10 digits long for int
			break;
	       
	}
	return wordlen;
	
}


/* === getFile ===
 * fill array fileargs with strings of correct length
 * fails when argument incorrect length, incorrect # of arguments each line.
 * */

int getFile(char *argv, char fileargs[][6][COMMAND_MAX+1], int* numCron, int Type){

	const char *CRONTAB_FILE = argv;
	//open files
	FILE *crontab = fopen(CRONTAB_FILE, "r");
	char line[LINE_SIZE];
	int lineNo = 0;
	//check for file errors
	if(crontab == NULL){
		printf("%s file failed to open.\n", CRONTAB_FILE);
		exit(EXIT_FAILURE);
	}

	//loop through files
	printf("\n\t***** %s *****\n\n", CRONTAB_FILE);
	while( fgets(line, LINE_SIZE, crontab) != NULL && lineNo < LINE_AMOUNT){

		printf("%s", line);
		
		char* startp = line; //start position of current word
		char* currentp = line; //char window over current word
		int wordlen;

		//numver of args in each line
		int argnum;
		argnum = 6 - (Type*4);
		
		/*ignore comment lines*/
		if(*startp == '#') continue;
		
		//Iterate through each word in the line
		for( int i = 0 ; i < argnum ; ++i){

			//set max word length
			wordlen = getWordLen(Type, i);
		
			char word[wordlen +1];
			char* wordp = word;

			//find the first character in the word
			while(*startp== ' ' || *startp == '\t'){
				++startp;
				if(*startp == '\r' || *startp == '\n'){
					/*newline char before full arg set*/
					printf("INCOMPLETE LINE\n");	
					exit(EXIT_FAILURE);
				}
			}

			currentp = startp;

			while(*currentp != ' '){
				if(*currentp== '\n'
					|| *currentp== '\r'){
					//newline before complete argument set
					if(i < (argnum-1)){
						printf("INCOMPLETE LINE\n");
						exit(EXIT_FAILURE);
					}
					else break;
				}
				//add char to current word
				*wordp = *currentp;
				currentp++;
				wordp++;
			}
			long int wordlength= currentp - startp;
			if(checkArgLen(wordlen, wordlength)){
				printf("Error: Line %d, word %d,  max arg length %d allowed. Argument length invalid.\n", lineNo, i, wordlen);
					exit(EXIT_FAILURE);

			}

			startp = currentp; //move start pointer to word end
			word[wordlength] = '\0'; //add null byte
			addArg(fileargs[lineNo][i], word, wordlength);
			
		}	
		if(!(*currentp == '\n' || *currentp == '\t')){
			printf("Error: Too many arguments or line not newline terminated, line %d\n", lineNo);
			exit(EXIT_FAILURE);
		}
		lineNo++;
	}
	*numCron = lineNo;
	//close files
	fclose(crontab);
	return 0;
}

