/*
 * concord3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2022.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "seng265-list.h"

//constants
#define LINE_LENGTH 100
#define KEYWORD_LENGTH 40
#define MAX_INDEX 60
#define MIN_INDEX 8
#define FIRST_LETTER_INDEX 29
#define BEFORE_WORD_INDEX 28
#define START_BOUND 65
#define END_BOUND 90

node_t *sort(node_t *start);

node_t *getWords(node_t *sentenceHead, node_t *exclusionHead, node_t *wordsHead);

node_t *deleteDuplicates(node_t *wordsHead);

node_t *createExclusion(node_t *exclusionHead);

node_t *createSentences(node_t *sentenceHead);

node_t *construct(node_t *wordsHead, node_t *sentenceHead, node_t *finalHead);

node_t *makePhrase(char* sentenceLow, char* wordLow, node_t *finalHead, node_t *tempNode, char* sentence);

int addFrontKeywordSpaces(char *phrase, char *lower, char *word);

// prints text from node in format from arg
void print_word(node_t *node, void *arg)
{
    char *format = (char *)arg;
    printf(format, node->text);
}

// removes \n character in char*
void removeNewLine(char* line){
    char* t = strtok(line, "\n");
    while(t != NULL){
        t = strtok(NULL, "\n");
    }
}


#ifdef DEBUG

/*
 * Just showing the use of the linked-list routines.
 */

void _demo() {
printf("DEBUG: in _demo\n");
    char *words_german[] = {"Der", "Kater", "mit", "Hut."};
    int   words_german_len = 4;

    char *words_english[] = {"The", "cat", "in", "the", "hat."};
    int   words_english_len = 5;

    node_t *temp_node = NULL;
    node_t *head = NULL;

    int i;

    /* Add the words in German, at the front. */
    for (i = 0; i < words_german_len; i++) {
        temp_node = new_node(words_german[i]);
        head = add_front(head, temp_node);
    }

    /* Add the words in English, at the end. */
    for (i = 0; i < words_english_len; i++) {
        temp_node = new_node(words_english[i]);
        head = add_end(head, temp_node);
    }

    /* Print the list of words. */

    apply(head, print_word, "--> %s\n");

    /* Free up the memory. This is done rather deliberately
     * and manually.  Asserts are liberally used here as they
     * express state that *must* be true if all of the code is
     * correctly working.
     */

    temp_node = head;
    while (temp_node != NULL) {
        assert(temp_node != NULL);
        head = remove_front(head);
        free(temp_node);
        temp_node = head;
    }
   
    assert(head == NULL); 
}

#endif



int main(int argc, char *argv[])
{
       
    node_t *tempNode = NULL;
    node_t *sentenceHead = NULL;// sentence linked list
    node_t *exclusionHead = NULL;// exclusion words linked list
    node_t *wordsHead = NULL;// keywords linked list
    node_t *finalHead = NULL;// final output linked list 

    // get exclusion and sentence linked list
    exclusionHead = createExclusion(exclusionHead);   
    sentenceHead = createSentences(sentenceHead);

    //sends data from input into getWords to find all keywords making a new linked list
    wordsHead = getWords(sentenceHead, exclusionHead, wordsHead); 

    // sorts keywords
    wordsHead = sort(wordsHead);

    // remove all duplicate keywords
    wordsHead = deleteDuplicates(wordsHead);
   
    //free exclusion linked list as it doesn't need to be used again
    freeList(exclusionHead);

    // iterates through keywords and sentences then sends data to helper functions to return each final node one at a time
    tempNode = wordsHead;
    
    finalHead = construct(wordsHead, sentenceHead, finalHead);   
    
    // print out final sentences in format
    apply(finalHead, print_word, "%s\n");
    
    //free remaining linked list
    
    freeList(sentenceHead);
    freeList(wordsHead);
    freeList(finalHead);

    exit(0);
}

// creates exclusion linked lists from stdin
node_t *createExclusion(node_t *exclusionHead){
    
    node_t *tempNode = NULL;
    char tempIn[LINE_LENGTH] = "";
    
    fgets(tempIn, LINE_LENGTH, stdin);

    versionCheck(tempIn);

    fgets(tempIn, LINE_LENGTH, stdin);

    // takes input in and assigns it to exclusion words or sentence linked list
    while(fgets(tempIn, LINE_LENGTH, stdin) != NULL){
        removeNewLine(tempIn);               
       
        // end loop once marker of end of exclusion is encountered
        if(strncmp(tempIn, "\"\"\"\"", LINE_LENGTH) == 0){
            return exclusionHead;  
        }

        // if word is in series of exclusion words add node containing word to exclusion word linked list
        // make exclusion words lowercase as extra precaustion
        char tempLower[LINE_LENGTH] = "";
        strncpy(tempLower, tempIn, LINE_LENGTH);
        stringToLower(tempLower);
        tempNode = new_node(tempLower);
        exclusionHead = add_end(exclusionHead, tempNode);                   
    }

    return exclusionHead;    
}

// creates sentences linked list from stdin
node_t *createSentences(node_t *sentenceHead){
    char tempIn[LINE_LENGTH] = "";
    node_t *tempNode = NULL;
    
    // loops through remainder of input for sentences   
    while(fgets(tempIn, LINE_LENGTH, stdin) != NULL){
         
        // ignores empty lines
        if(strncmp(tempIn, "\n", LINE_LENGTH) == 0){
            continue;
        }

        removeNewLine(tempIn);

        tempNode = new_node(tempIn);
        sentenceHead = add_end(sentenceHead, tempNode);
    }

    return sentenceHead;
}



//frees the memory from the linked list starting at head
void freeList(node_t *head){
    node_t *tempNode; 
    tempNode = head;

    // loops through linked list freeing everything
    while(tempNode != NULL){
        assert(tempNode != NULL);
        head = remove_front(head);
        free(tempNode);
        tempNode = head;
    }

    assert(head == NULL);
}

// core loops of construction of final lines
node_t* construct(node_t *wordsHead, node_t *sentenceHead, node_t *finalHead){

    // iterates through words
    for(node_t *tempNode = wordsHead;tempNode != NULL;tempNode = tempNode->next){
       
        //iterates through sentences
        for(node_t *sentenceCur = sentenceHead;sentenceCur != NULL;sentenceCur = sentenceCur->next){
            char sentence[LINE_LENGTH] = "";// current sentence
            char sentenceLow[LINE_LENGTH] = "";// lowercase version of current sentence
            char wordLow[KEYWORD_LENGTH] = "";// lowercase version of keyword

            // copy strings into local temp variables
            strncpy(sentence, sentenceCur->text, LINE_LENGTH);
            strncpy(sentenceLow, sentenceCur->text, LINE_LENGTH);
            strncpy(wordLow, tempNode->text, KEYWORD_LENGTH);
            
            // set lowercase versions to lowercase
            stringToLower(sentenceLow);
            stringToLower(wordLow);
             
            finalHead = makePhrase(sentenceLow, wordLow, finalHead, tempNode, sentence);
            
        }
    }

    return finalHead;
}   

//checks if word is in sentence and if so calls for construction of final line
node_t* makePhrase(char* sentenceLow, char* wordLow, node_t *finalHead, node_t *tempNode, char* sentence){

    // checks if keyword is in sentence and if so sent to helper functions for formatting
    if(strstr(sentenceLow, wordLow)){       
        char save[LINE_LENGTH] = "";
        char upper[KEYWORD_LENGTH] = "";

        //final sentence to save
        getFinalPhrase(wordLow, sentence, save);
       
        //save[0] = getFinalPhrase(wordLow, sentence);          
      
        //copy keyword into upper 
        strncpy(upper, tempNode->text, KEYWORD_LENGTH);
                 
        // create uppercase keyword
        stringToUpper(upper);
          
        // confirms that the capitalized keyword exists in sentence returned from getFinalPhrase
        if(strstr(save, upper)){ 
            finalHead = add_end(finalHead, new_node(save));
        }
    }

    return finalHead;
}

// checks if input is of version 1 and prints correlating messege
void versionCheck(char* tempIn){
    if(strncmp(tempIn, "1", 1) == 0){
        printf("%s\n", "Input is version 1, concord3 expected version 2");
        exit(0);
    }
}   


// remove all duplicate keywords
node_t *deleteDuplicates(node_t *wordsHead){
    
    node_t *tempNode = wordsHead;

    // iterate through keywords and remove any
    while(tempNode->next != NULL){
        if(strncmp(tempNode->text, tempNode->next->text, LINE_LENGTH) == 0){
            node_t *freeNode = tempNode->next;
            tempNode->next = tempNode->next->next;
            freeNode->next = NULL;
            freeList(freeNode);
            continue;
        }

        tempNode = tempNode->next;
    }

    return wordsHead;
}

// modifies wordsHead to contain all words from the sentences linked list excluding words in the exclusion words linked list
node_t *getWords(node_t *sentenceHead, node_t *exclusionHead, node_t *wordsHead){

    //iterates through sentences and keywords 
    for(node_t *sentenceCur = sentenceHead;sentenceCur != NULL;sentenceCur = sentenceCur->next){
        char sentence[LINE_LENGTH] = "";
        strncpy(sentence, sentenceCur->text, LINE_LENGTH);
        stringToLower(sentence);
        char* t;

        // tokenize sentence
        for(t = strtok(sentence, " ");t != NULL;t = strtok(NULL, " ")){

            int addToList = 1;

            // checks if current word(t) of sentence is an exclusion word 
            for(node_t *exclusionCur = exclusionHead;exclusionCur != NULL;exclusionCur = exclusionCur->next){
                if(strlen(exclusionCur->text) != strlen(t)){
                    continue;
                }else if(strncmp(exclusionCur->text, t, strlen(t)) == 0){
                    addToList = 0;
                    continue;
                }
                
            }

            // adds current word(t) to keywords linked list if not exclusion word 
            if(addToList){
                node_t *temp = new_node(t);
                wordsHead = add_end(wordsHead, temp);
            }            

        }
        
    }

    return wordsHead;
}

// adds spaces to front then calls formatPhrase
void getFinalPhrase(char *word, char *phrase, char *newPhrase){ 
    char lowerPhrase[LINE_LENGTH] = "";// lowercase version of phrase used as guide
    char lower[LINE_LENGTH] = "";
    char upperWord[KEYWORD_LENGTH] = "";
    int indexTilWord;
 
    // create copy of word in upper case
    strncpy(upperWord, word, KEYWORD_LENGTH);
    
    stringToUpper(upperWord);

    // create lowercase version of phrase
    strncpy(lower, phrase, LINE_LENGTH);
    stringToLower(lower); 


    // modifies last char* so that it is the same has first char* with keyword in uppercase  
    tokenizePhrase(phrase, word, upperWord, newPhrase, &indexTilWord);
    tokenizePhrase(lower, word, upperWord, lowerPhrase, &indexTilWord);
 
    // checks if capitalized keyword is in lowerPhrase and if so calls formatPhrase 
    if(strstr(lowerPhrase, upperWord)){
        formatPhrase(newPhrase, upperWord, indexTilWord, lowerPhrase);       
    }

}

// adds spaces to front of phrase and lower until keyword is in column 30
// calls edgeManagement to remove edge words when needed
void formatPhrase(char *phrase, char* word, int indexTilWord, char *lower){
   
    indexTilWord -= FIRST_LETTER_INDEX;

    shiftPhrase(phrase, word, indexTilWord, lower);

    addFrontIndexSpaces(phrase, lower);     

    int size = 0;

    size =  addFrontKeywordSpaces(phrase, lower, word);

    // removes edge words that come before column 10 and after column 60
    edgeManagement(phrase, lower, size);
    
}    

// shifts phrase to the left if keyword comes after column 30 until keyword starts on column 30
void shiftPhrase(char *phrase, char *word, int indexTilWord, char *lower){

    if(indexTilWord > 0){

        char temp = lower[0];
        for(int i = 0;i < indexTilWord;i++){       
            temp = lower[0];
            
            // shifts lower and phrase left
            for(int j = 0;j < strlen(lower);j++){
                phrase[j] = phrase[j + 1];
                lower[j] = lower[j + 1];
            }   
        }
    }
}


// adds spaces to front of current sentence until lower and phrase are length 30
void addFrontIndexSpaces(char *phrase, char *lower){
    
    // iterates til size of line is 30 characters long
    while(strlen(lower) < 30){
        char space[LINE_LENGTH] = " ";
        char lowerSpace[LINE_LENGTH] = " ";
        strcat(space, phrase);
        strcat(lowerSpace, lower);
        strncpy(phrase, space, LINE_LENGTH);
        strncpy(lower, lowerSpace, LINE_LENGTH);
    }

}

// adds space to front of lower and phrase until column 30 contains start of keyword
int addFrontKeywordSpaces(char *phrase, char *lower, char *word){
 
    int size = strlen(phrase);

    // iterates until first letter of keyword is in column 30
    while(lower[FIRST_LETTER_INDEX] != word[0] || lower[BEFORE_WORD_INDEX] != ' '){ 
        char space[LINE_LENGTH] = " ";
        char lowerSpace[LINE_LENGTH] = " ";
        strcat(space, phrase);
        strcat(lowerSpace, lower);
        strncpy(phrase, space, LINE_LENGTH);
        strncpy(lower, lowerSpace, LINE_LENGTH);
        size++;
    }

    return size;

}

// removes edge words that completely or partiallu preceed column 10 or exceed column 60
void edgeManagement(char *phrase, char *lower, int size){

    leftEdge(phrase, lower);

    rightEdge(phrase, lower, size);   
    
}

// removes left edge words
void leftEdge(char *phrase, char *lower){
    int i;    

    // finds index inwhich characters from 0-i should be changed to white space
    for(i = MIN_INDEX; lower[i] != ' ';i++){}

    // changes characters from index 0-i to whitespace
    while(i >= 0){
        phrase[i] = ' ';
        lower[i] = ' ';
        i--;
    }
}

// removes right edge words
void rightEdge(char *phrase, char *lower, int size){

    // finds index where keyword ends
    int i = FIRST_LETTER_INDEX;
   // size = strlen(phrase);
    while(i < size - 1 && (lower[i] < START_BOUND || lower[i] > END_BOUND)){
        i++;
    }

    // if keyword isn't on ending edge set i to max index
    if(i < MAX_INDEX){
         i = MAX_INDEX;
    }
    
    // finds index inwhich null character should be placed to cut off words exceeding column 60
    if(size > i){
        while(lower[i] != ' '){
            i--;      
        } 
    }
 
    // place null character to cut off words exceeding column 60
    lower[i] = '\0';
    phrase[i] = '\0';
}

// constructs newPhrase from phrase inwhich word in phrase is capitalized
void tokenizePhrase(char* phrase, char* word, char* upperWord, char* newPhrase, int* indexTilWord){
    
    char* t = strtok(phrase, " ");
    
    // tokenization loop
    while(t != NULL){
        
        // creates temp char* of current word(t) in lowercase for comparison
        char keyword[KEYWORD_LENGTH] = "";
        strncpy(keyword, t, KEYWORD_LENGTH);
        stringToLower(keyword);
        
        // if current word(t) is the same as word then strcat word into newPhrase
        // else: strcat word(t) into new phrase
        if((strncmp(keyword, word, KEYWORD_LENGTH) == 0)){
            *indexTilWord = strlen(newPhrase);
            strcat(newPhrase, upperWord);
        }else{
            char temp[LINE_LENGTH] = "";
            strncpy(temp, t, KEYWORD_LENGTH);
            strcat(newPhrase, temp);
        }

        t = strtok(NULL, " ");

        // add space to end of newPhrase unless its the end of the string 
        if(t != NULL){
            strcat(newPhrase, " ");
        }
       

    }

}

// converts string passed in to all lowercase
void stringToLower(char* string){
    for(int i = 0;i < strlen(string);i++){
        string[i] = tolower(string[i]);
    }
}

// converts string passed in to all lowercase
void stringToUpper(char* string){
    for(int i = 0;i < strlen(string);i++){
        string[i] = toupper(string[i]);
    }
}

// bubble sort linked list
node_t *sort(node_t *start){
    int switched;
    node_t *cur;
    node_t *prev = NULL;

    // returns if empty linked list
    if(start == NULL){
        return;
    }

    // loops through linked list
    do{
        switched = 0; // used as boolean of if a switch occured
        cur = start;

        // swaps values if the text of cur node is greater than text of next node
        while(cur->next != prev){
            if(strncmp(cur->text, cur->next->text, KEYWORD_LENGTH) > 0){
                swap(cur, cur->next);
                switched = 1;
            }
           
            cur = cur->next;
        }
        prev = cur;
        
    }while(switched);

    // returns sorted linked list
    return start;
}

// swaps text of 2 passed nodes
void swap(node_t *a, node_t *b){
    char temp[KEYWORD_LENGTH] = "";
    strncpy(temp, a->text, KEYWORD_LENGTH);
    strncpy(a->text, b->text, KEYWORD_LENGTH);
    strncpy(b->text, temp, KEYWORD_LENGTH);
    
}
