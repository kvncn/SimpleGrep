/*
 * Kevin Nisterenko CSC 352 Fall 2022
 * PA4 - sgrep.c
 *
 * This program defines the necessary functions
 * to implement the simple grep command structure.
 * It takes between 0 and 3 flag arguments, one for
 * the grep mode and another for the text to look
 * for in the grep search. The command line arguments
 * can be in any order, the program will recognize
 * them appropriately by parsing thorugh the arguments
 * and checking for their validity. Then, the actual 
 * searching will be performed in whatever was given 
 * by standard input.
 * 
 */
#include <stdio.h>
#include <string.h>

/*
 * This function gets a character as a 
 * parameter and checks by ascii value 
 * if it represents a numerical digit. 
 *
 * @param character, char representing 
 * the character to be checked
 * @return int, 1 if the character 
 * represents a digit, 0 otherwise
 */
int isDigit(char character) {
 	if (character > 57 || character < 48) {
	 	return 0;
	}
	return 1;
}

/*
 * This function gets a character as a 
 * parameter and checks by ascii value 
 * if it represents a letter (upper 
 * or lower case). 
 *
 * @param character, char representing 
 * the character to be checked
 * @return int, 1 if the character 
 * represents a letter, 0 otherwise
 */
int isLetter(char character) {
    if (character >= 'A' && character <= 'Z') {
        return 1;
    } else if (character >= 'a' && character <= 'z') {
        return 1;
    }
    return 0;
}

/*
 * This function gets a string as a parameter, this string
 * represents the pattern to compare. It checks the string
 * for invalid patterns or characters inside it, returns 
 * an int to indicate if it was valid or not. 
 * 
 * @param pattern, char array representing 
 * the pattern string to be checked
 * @return int, 1 if the pattern is valid, 0 otherwise
 */
int validatePattern(char pattern[]) {
    int size = strlen(pattern);
    for (int i = 0; i < size; i++) {
        char curr = pattern[i];
        // if the character is a hashtag, check that it is between letters
        if (curr == '#') {
            if ((i == 0 || i == size-1) || (!isLetter(pattern[i-1]) || !isLetter(pattern[i+1]))) {
                return 0;
            }
        // if the character is not a letter, digit, or period, not valid as well
        } else if (!isLetter(curr) && !isDigit(curr) && curr != '.') {
            return 0;
        }
    }
    return 1;
}

/**
 * This function takes a string and modifies it so that all letters 
 * become lowercase. 
 * 
 * @param *string, pointer to the first character of a string
 */
void toLowerCase(char *string) {
    for (int i = 0; i < strlen(string); i++) {
        char curr = string[i]; 
        // ascii to lower case algebra
        if (curr >= 'A' && curr <= 'Z') {
            string[i] = curr + 32;
        } else {
            string[i] = curr;
        }
    }
}

/*
 * This function builds the comparison string from the
 * given pattern, used when the -e flag is active from 
 * the command line. It loops through the pattern and checks 
 * the specific cases (#, ., or just a char) and does the 
 * necessary building and comparison to return whether or not
 * the pattern was found in the input line.
 * 
 * @param line, char array represeting line of input to check 
 * for pattern
 * @param text, char array representing 
 * the pattern string to be checked
 * @param compare, char array representing the string that
 * has the match from the line
 * @param lowLine, char array for lowercase version of line
 * @param lowText, char array for lowercase version of text
 * @param idx, integer representing starting index in line
 * @param insensitive, integer representing whether or not 
 * to do insensitive matching
 * @return int, 0 if the pattern was found in the line, 
 * 1 otherwise
 */
int patternMatch(char line[], char text[], char compare[], char lowLine[], char lowText[], int idx, int insensitve) {
    int lineLen = strlen(line);
    int textLen = strlen(text);

    // if the first character is a match
    if (line[idx] == text[0] || (lowLine[idx] == lowText[0] && insensitve)) {
        int i;
        int k = 0;
        // we iterate over the text pattern and make the necessary comparison and string builiding
        for (i = 0; i < textLen; i++) {
            // if we are at a hasthag, we need a special case because the string might end up 
            // of a different length than the text pattern
            if (text[i] == '#') {
                char stop = text[i+1];
                char lowStop = lowText[i+1];
                // while we haven't reached the stopping character, keep building the 
                // comparison string
                while (line[idx] != stop && lowLine[idx] != lowStop && idx < lineLen) {
                    compare[k] = line[idx];
                    compare[k+1] = '\0';
                    k++;
                    idx++;
                }
                idx--;
                // dud for correction of string end
                compare[k] = '3';
                k--;
            // if it is a period just add the character in line
            } else if (text[i] == '.') {
                compare[k]  = line[idx];
            // if it doesn't match and we aren't insensitive, return
            } else if (text[i] != line[idx] && !insensitve) {
                compare[0] = '\0';
                return 1;
            // if it doesn't match and we are insensitive, return
            } else if (text[i] != line[idx] && insensitve && lowText[i] != lowLine[idx]) {
                compare[0] = '\0';
                return 1;
            // just add what's left because it is valid
            } else {
                compare[k] = line[idx];
            }
            k++;
            idx++;
            compare[k] = '\0';
        }
        // string correction
        compare[k] = '\0';
        return 0;
    }
    return 1;
}

/*
 * This function takes the input line and the grep flags and 
 * text pattern to do the appropriate search depending on 
 * whether or not it is exact printing, case sensitive and
 * if we want to match the string or a pattern. It uses 
 * a loop throughout the line to build the comparisons
 * and check for matches.
 * 
 * @param line, char array represeting line of input to check 
 * for pattern
 * @param text, char array representing 
 * the pattern string to be checked
 * @param exact, integer representing whether or not 
 * to do exact printing
 * @param insensitive, integer representing whether or not 
 * to do insensitive matching
 * @param pattern, integer representing whether or not 
 * to do pattern matching
 */
void grepLine(char line[], char text[], int exact, int insensitive, int pattern) {
    int lineLen = strlen(line);
    int textLen = strlen(text);

    // set lowercase for insensitve comparison by using a copy
    char lowText[textLen];
    char lowLine[lineLen];
    strcpy(lowText, text);
    strcpy(lowLine, line);
    toLowerCase(lowLine);
    toLowerCase(lowText);

    // in the worst case it is just going to be the whole line (+1 for the null)
    char compare[lineLen+1];
    char lowComp[lineLen+1];
    
    // set found flag to not found at first
    int found = 1;
    // for every character in the line
    for (int i = 0; i < lineLen; i++) {
        compare[0] = '\0';
        if (pattern) {
            found = patternMatch(line, text, compare, lowLine, lowText, i, insensitive);
        } else {
            // if we want to compare a string, not a pattern, just build both
            // lowercase and regular versions
            int j;
            for (j = 0; j < textLen; j++) {
                compare[j] = line[i + j];
                lowComp[j] = lowLine[i + j];
            }
            compare[j] = '\0';
            lowComp[j] = '\0';
            // do the comparison on the necessary version of the string
            if (insensitive) {
                found = strcmp(lowComp, lowText);
            } else {
                found = strcmp(compare, text);
            }
        }

        // if found, check whether we want to print exact or whole line
        // if we want the whole line, we can also just return because 
        // even if there is another match later we print regardless
        if (found == 0) {
            if (exact) {
                printf("%s\n", compare);
            } else {
                printf("%s\n", line);
                return;
            }
        }
        found = 1;
    }

}

int main(int argc, char * argv[]) {
    char buffer[202];
    int exact = 0;
    int insensitive = 0;
    int pattern = 0;

    if (argc < 2) {
        fprintf(stderr, "Requires more command-line arguments.\n");
        return 1;
    }

    // just set it for now
    char *text = argv[0];
    int count = 0;
    // skip the first argv (it's just the executable name)
    for (int i = 1; i < argc; i++) {
        // get the argument string
        char *arg = argv[i];
        // if it's a flag argument, we set them to true
        if (arg[0] == '-' && strlen(arg) == 2) {
            count++;
            switch (arg[1]) {
                case 'o':
                    exact = 1;
                    break;
                case 'i':
                    insensitive = 1;
                    break;
                case 'e':
                    pattern = 1;
                    break;
                default: 
                    return 3;
            }
        } else {
            text = argv[i];
        }
    }
    
    if (validatePattern(text) == 0 || text[0] == '#') {
        fprintf(stderr, "Invalid search term.\n");
        return 2;
    }

    // while the user input is valid, keep getting it
	while (fgets(buffer, 200, stdin) != NULL) {
		// make sure to correctly end strings
		if (buffer[strlen(buffer) - 1] == '\n') {
			buffer[strlen(buffer) - 1] = '\0';
		}
		grepLine(buffer, text, exact, insensitive, pattern);
	}
	return 0;
}