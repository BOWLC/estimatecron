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

int checkmin(int minute){
	//Is minute within range
	if(minute < 0 || minute > 59) return -1;
	//return 0 for in range
	return 0;
}

int checkhour(int hour){
	//Is hour within range
	if(hour <0 || hour > 23) return -1;
	//return 0 for in range
	return 0;
}

int checkdaymonth(int day){
	//Is day within range
	if(day < 1 || day > 31) return -1;
	//return 0 for in range
	return 0;
}

int checkmonth(int month){
	//Is month within range
	if(month < 0 || month > 11) return -1;
	//return 0 for in range
	return 0;
}

int checkdayweek(int day){
	//Is year within range
	if(day < 0 || day > 6) return -1;
	//return 0 for in range
	return 0;
}

int checkIntArg(char a){
	if(a  == '*') return 0;
	if(a < '0' || a > '9') return -1; //not an integer or *
	return 0; //within range
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

void printCron(char cronWords[][6][COMMAND_MAX], int numCron, int numargs){

	printf("PRINTING %d ARGUMENTS FROM ARRAY:\n", numCron);
	//each line
	for(int i = 0; i < numCron; i++){

		for(int j = 0; j < numargs; j++){
			printf("%s\t", cronWords[i][j]);
		}
		printf("\n");
	}
	return;

}

int getWordLen(int Type, int wordno){

	int wordlen;
	switch(Type){
		case 0:if( wordno < 4){
				wordlen = 2; 
			} else if( wordno < 5){
				wordlen = 3; // day e.g. "mon"
			} else{
				wordlen = COMMAND_MAX -1;
			}
			break;
	       
		case 1:	if(wordno == 0) wordlen = COMMAND_MAX;
			else wordlen = 10;//max 10 digits long
			break;
	       
	}
	return wordlen;
	
}

int getcron(char *argv, char cronWords[][6][40], int* numCron, int Type){

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

		//ignore comments
		if(line[0] == '#') continue;
		printf("%s", line);
		
		char* startp = line; //start position of current word
		char* currentp = line; //char window over current word
		int wordlen;

		int argnum;
		//numver of args in each line (1 is crontab, 2 is estimates)
		switch(Type){
			case 0: argnum = 6;
				break;

			case 1: argnum = 2;
				break;

			default: argnum = 6;	
				break;
		}

		//Iterate through each word in the line
		for( int i = 0 ; i < argnum ; ++i){


			//set max word length
			wordlen = getWordLen(Type, i);
		
			char word[wordlen +1];
			char* wordp = word;

			//find the first character in the word
			while(*startp== ' ' || *startp == '\t'){
				++startp;
			}

			currentp = startp;

			while(*currentp != ' '){
				if(*currentp== '\n'
					|| *currentp== '\r'){
					//newline before complete argument set
					if( (i < 4 || currentp == startp)&& Type ==0){
						printf("INCOMPLETE LINE\n");
						exit(EXIT_FAILURE);
						break;
					}
					else break;
				}

				if(i < 4 && Type == 0){
					if(checkIntArg(*currentp)){
						//int argument incorrect char
						printf("Error: Line %d.  Incorrect value for argument type integer.\n", lineNo);
						exit(EXIT_FAILURE);
					}
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
			addArg(cronWords[lineNo][i], word, wordlength);
			
		}	
		lineNo++;
	}
	*numCron = lineNo;
	//close files
	fclose(crontab);
	return 0;


}

int main(int argc, char *argv[]){

	char cronwords[LINE_AMOUNT][6][COMMAND_MAX];
	char estimates[LINE_AMOUNT][6][COMMAND_MAX];
	int numcron = 0;
	int numestimates = 0;
	int* numcronp = &numcron;
	int* numestimatesp = &numestimates;

	getcron(argv[2],cronwords, numcronp, 0);//crontab
	getcron(argv[3],estimates, numestimatesp, 1); 
	printf("COMPLETED.  FOUND %d ARGUMENTS...........\n", numcron);
	printCron(cronwords, numcron, 6);
	printCron(estimates, numestimates, 2);
	return 0;
}	
