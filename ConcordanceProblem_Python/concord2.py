#!/usr/bin/env python3

# Tyson Chan
# V00985911
# Seng 265 A2, concord2

import sys
MAX_INDEX = 60
MIN_INDEX = 8
FIRST_LETTER_INDEX = 29
BEFORE_WORD_INDEX = 28
START_BOUND = 65
END_BOUND = 90

def main():
    wholeInput = [] # stores everything from stdin except \n on end
    exclusion = [] # stores exclution words
    sentences = [] # stores each sentence
    words = [] # stores all keywords to be capitalized
    orderedSentences = [] # stores final output  
    
    # transfers all input from stdin to wholeInput excluding final character (\n)   
    for i in sys.stdin:
        wholeInput.append(i.strip())
    
    if wholeInput[0] == "1" :
        print("Input is version 1, concord2.py expected version 2")
        exit()       

    # obtains and stores exclusion words in exclusion
    # Also saves index of when keywords will start
    i = 2
    while(wholeInput[i] != "\"\"\"\""):
        exclusion.append(wholeInput[i])
        i = i + 1

    # obtains and stores sentences in sentences array
    i = i + 1
    while(i < len(wholeInput)):
        sentences.append(wholeInput[i])
        i = i + 1
    
    # calls createWordsList() to construct an array of all keywords without duplicate or exclusion words 
    words = createWordsList(sentences, exclusion)
    
    # sort keywords
    words.sort()
    
    # creates and prints output with assistance of other functions
    output(sentences, words)


# searches through array of strings(sentences) to find keywords withing each string
# Note: there will be no duplicate words in words array
      # no exclusion words will be in the words array
def createWordsList(sentences, exclusion):
    words = []
    # obtains and stores all keywords to be capitalized into words array
    for j in sentences:
        temp = j.split() # temp array of all words of current sentence
        for k in temp:
            isWord = True # used to make sure exclusion words and duplicate words aren't added  
            
            # checks if current word(l) is an exclusion word
            # if it is l isn't added to words array
            for l in exclusion:
                if k.lower() == l.lower():
                    isWord = False
                    break

            # checks if current word(m) is already in words array
            # if it is then m isn't added to words array 
            for m in words:
                if k.lower() == m.lower():
                    isWord = False
                    break

            # adds keyword(k) to words array if not exclusion or duplicate word
            if isWord:
                words.append(k.lower())
   
    return words    


# constructs a string in desired format with word in sentence capitalized
# Desired Format: 
    # first letter of word String is at column 30
    # no letters/words should come before column 10
    # no letters/words should come after column 60
    # if a word is cut off in between letters from the previous constraints remove the whole word
def getFinalPhrase(sentence, word):
    lowerCase = sentence.lower()
    separateLowerCase = lowerCase.split() 
    separateWords = sentence.split()
    phrase = "" 
    lowerPhrase = "" 
    indexTilWord = 0  
    
    # obtains phrase and lowerPhrase with word capitalized
    # obtains indexTilWord as index where word String starts
    phrase, lowerPhrase, indexTilWord  = capitalize(phrase, lowerPhrase, separateWords, separateLowerCase, word) 

    # returns when word isn't in sentence but is within another word in sentence
    if word.upper() not in lowerPhrase:
        return lowerPhrase

    #obtains amount of characters must be removed from front
    indexTilWord = indexTilWord - FIRST_LETTER_INDEX


    # removes characters from front of phrase until indexTilWord number of characters have been remove
    # but if part of a word is cut off the whole word will be cut even if characters removed exceeds indexTilWord
    i = 0
    if indexTilWord > 0:
        temp = lowerPhrase[0]
        while(i < indexTilWord or (lowerPhrase[0] != " " and temp != " ")):
            temp = lowerPhrase[0]
            lowerPhrase = lowerPhrase[1:len(lowerPhrase)]
            phrase = phrase[1:len(phrase)]
            i = i + 1    

    #adds spaces to phrase and lowerPhrase until length of them is 30
    #Note: to avoid trying to access out of bounds later 
    while(len(lowerPhrase) < FIRST_LETTER_INDEX + 1):
        phrase = " " + phrase
        lowerPhrase = " " + lowerPhrase  

    #adds spaces to front of phrase and lowerPhrase until column 30 holds the first letter of capitalized word
    i = 0
    while((lowerPhrase[FIRST_LETTER_INDEX] != word[0].upper() or lowerPhrase[BEFORE_WORD_INDEX] != " ")):
        phrase = " " + phrase
        lowerPhrase = " " + lowerPhrase
   
    # returns phrase with characters before column 10 as spaces and characters after column 60 removed
    return edgeManagement(phrase, lowerPhrase)
    


# creates new phrase and lowerPhrase with passed word capitalized
# returns phrase, lowerPhrase, and index the word starts at(indexTilWord)
def capitalize(phrase, lowerPhrase, separateWords, separateLowerCase, word):

    indexTilWord = 0
    j = 0

    # constructs string(phrase) from sentence with word capitalized
    # saves index inwhich word is added
    while(j < len(separateWords)):
        if separateLowerCase[j] == word:
            indexTilWord = len(phrase);
            phrase = phrase + word.upper()
            lowerPhrase = lowerPhrase + word.upper()
        else:
            phrase = phrase + separateWords[j]
            lowerPhrase = lowerPhrase + separateLowerCase[j]
        phrase = phrase + " "
        lowerPhrase = lowerPhrase + " "
        j = j + 1

    # removes extra space from Strings
    phrase = phrase[:-1]
    lowerPhrase = lowerPhrase[:-1]

    return phrase, lowerPhrase, indexTilWord



# iterates through keywords(words) and adds all sentence Strings in correct format to orderedSentences
# Note: this loop calls getFinalPhrase to construct the sentence in the correct format
def output(sentences, words):
    orderedSentences = []
    
    # searches though the sentences for string in words then calls getFinalPhrase for creating outputed phrase
    for j in words:
        for k in sentences:
            if j in k.lower():
                returnedPhrase = getFinalPhrase(k, j) # holds result of getFinalPhrase for verfication
                if j.upper() in returnedPhrase:  
                    orderedSentences.append(returnedPhrase)
    
    #prints final output 
    for j in orderedSentences:
       print(j)


# replaces characters before column 10 with " "
# removes characters that exceed colomn 60
    # if character replaced is within a word perform action on entire word
def edgeManagement(phrase, lowerPhrase):

    # creates list of phrase and lowerPhrase to manipulate certain indices
    phraseList = list(phrase)
    lowerPhraseList = list(lowerPhrase)

    # locates index inwhich characters should be removed from 0-i
    i = MIN_INDEX
    if(phraseList[i] != " "):
        while(phraseList[i] != " "):
             i = i + 1

    # removes characters from 0-i
    while(i >= 0):
        phraseList[i] = " "
        lowerPhraseList[i] = " "
        i = i - 1
    
    # locates index inwhich capitalized word ends
    i = FIRST_LETTER_INDEX;
    while (i < len(lowerPhraseList) - 1 and (ord(lowerPhraseList[i]) < START_BOUND or ord(lowerPhraseList[i]) > END_BOUND)):
        i = i + 1
    
    # if capitalized word does not exceed max index characters should appear set i to max index
    if i < MAX_INDEX :
        i = MAX_INDEX
    
    # obtains index i inwhich characters should be removed from i - max index character appear
    if(len(phraseList) > i and phraseList[i] != " "):
        while(phraseList[i] != " "):
            i = i - 1
    
    # sets characters from i - phrase length to empty string
    while i < len(phrase):
        phraseList[i] = ""
        lowerPhraseList[i] = ""
        i = i + 1
    
    # construct final string(phrase) to be returned
    i = 0
    phrase = ""
    while(i < len(phraseList)):
        phrase += phraseList[i]
        i = i + 1

    
    # return phrase
    return phrase


    




 


if __name__ == "__main__":
    main()
