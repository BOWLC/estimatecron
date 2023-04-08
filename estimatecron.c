/* Check validity of a crontab file
 * 	Author: Lachlan Campbell
 * 	Date: 4/3/2023
 * 	*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 100
#define LINE_AMOUNT 20
#define COMMAND_MAX 40

const char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov"};
const char days[7][4] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};

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
void printargs(char fileargs[][6][COMMAND_MAX], int numCron, int numargs);
int getWordLen(int Type, int wordno);
int getFile(char *argv, char fileargs[][6][40], int* numCron, int Type);
int checkCron(char fileargs[][6][40], int linenum);
int checkEstimates(char fileargs[][6][40], int linenum);

int main(int argc, char *argv[]){

	char cronwords[LINE_AMOUNT][6][COMMAND_MAX];
	char estimates[LINE_AMOUNT][6][COMMAND_MAX];
	int numcron = 0;
	int numestimates = 0;
	int* numcronp = &numcron;
	int* numestimatesp = &numestimates;

	getFile(argv[2],cronwords, numcronp, 0);//crontab
	getFile(argv[3],estimates, numestimatesp, 1); 
	printf("COMPLETED.  FOUND %d VALID LINES...........\n", numcron);
	printargs(cronwords, numcron, 6);
	printargs(estimates, numestimates, 2);
	if(checkCron(cronwords, numcron)){
		printf("\nError: Incorrect argument values in cron file\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}	

/* ===checkCron===
 * check each argument is correct type and range in cron file
 * TODO: improve the dictionary search with hash maps
 */
int checkCron(char fileargs[][6][40], int linenum){
	printf("CHECKING CRON FILE --------\n");
	for(int i = 0; i < linenum; i++){
		printf("\n");
		for(int j = 0; j < 5; j++){
				printf("\t%s/%d",fileargs[i][j], atoi(fileargs[i][j]));
			switch(j){
				case 0:/*minute*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])){
						printf("\nError: checkIntArg failed\n");
						return 1;
					}
					if(checkMin(atoi(fileargs[i][j])))return 1;
					break;
				case 1:/*hour*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])){
						printf("\nError: checkIntArg failed\n");
						return 1;
					}
					if(checkHour(atoi(fileargs[i][j])))return 1;
					break;
				case 2:/*day of month*/
					if(!checkStar(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])){
						printf("\nError: checkIntArg failed\n");
						return 1;
					}
					if(checkDayMonth(atoi(fileargs[i][j])))return 1;
					break;
				case 3:/*month*/
					if(!checkStar(fileargs[i][j])) break;
					if(!checkMonthWord(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])){
						printf("\nError: checkIntArg failed\n");
						return 1;
					}
					if(checkMonth(atoi(fileargs[i][j])))return 1;
					break;
				case 4:/*day of week*/
					if(!checkStar(fileargs[i][j])) break;
					if(!checkDayWord(fileargs[i][j])) break;
					if(checkIntArg(fileargs[i][j])){
						printf("\nError: checkIntArg failed\n");
						return 1;
					}
					if(checkDayWeek(atoi(fileargs[i][j])))return 1;
					break;
			}
		}
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

void printargs(char fileargs[][6][COMMAND_MAX], int numCron, int numargs){

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
				wordlen = 3; // day e.g. "mon"
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

int getFile(char *argv, char fileargs[][6][40], int* numCron, int Type){

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

