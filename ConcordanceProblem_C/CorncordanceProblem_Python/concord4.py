#!/usr/bin/env python3

""" Tyson Chan
    V00985911
    Seng 265 A4, concord4"""

import sys
import re
MAX_INDEX = 60
MIN_INDEX = 8
FIRST_LETTER_INDEX = 29
BEFORE_WORD_INDEX = 28
START_BOUND = 65
END_BOUND = 90

class concord:

    # initializes concord
    def __init__(self, inputFile=None, outputFile=None):
        self.inputFile = inputFile
        self.outputFile = outputFile
        if inputFile != None and outputFile != None:
            self.__fileConcordance()

    #calls full concordance and writes output to given file
    def __fileConcordance(self): 
            output = self.full_concordance()
            fileOut = open(self.outputFile, 'w')
            for i in output:
                fileOut.write(i + "\n")
            fileOut.close()
            
            

    # returns desired output with keyword is 30th column and words from preceeding column 10 and proceeding column 60 removed
    def full_concordance(self):
        wholeInput = [] # stores everything from stdin except \n on end
        exclusion = [] # stores exclution words
        sentences = [] # stores each sentence
        words = [] # stores all keywords to be capitalized
        orderedSentences = [] # stores final output    

        wholeInput = self.__getInput()
 
        # terminiates program if wrong test case provided
        if wholeInput[0] == "1" :
            print("Input is version 1, concord2.py expected version 2")
            exit()
        
        exclusion, i = self.__getExclusion(wholeInput)

        sentences = self.__getSentences(wholeInput, i)
     
        words = self.__createWordsList(sentences, exclusion)
    
        words.sort()
    
        # creates and prints output with assistance of other functions
        return self.__output(sentences, words, self.outputFile)

    
    """ obtains and stores exclusion words in exclusion
    Also saves index of when keywords will start """
    def __getExclusion(self, wholeInput): 
        exclusion = []
        i = 2
        while(wholeInput[i] != "\"\"\"\""):
            exclusion.append(wholeInput[i].lower())
            i = i + 1
        return exclusion, i


    # obtains and stores sentences in sentences array
    def __getSentences(self, wholeInput, i): 
        sentences = []
        
        i = i + 1
        while(i < len(wholeInput)):
            sentences.append(wholeInput[i])
            i = i + 1
       
        return sentences
        
    # gets input from stdin or input file if input file is passed in    
    def __getInput(self):
        wholeInput = []

        if self.inputFile == None:
            for i in sys.stdin:
                wholeInput.append(i.strip())
        else:
            fileIn = open(self.inputFile, 'r')
            lines = fileIn.readlines()
            for i in lines:
                wholeInput.append(i.strip())
            fileIn.close()

        return wholeInput

    """ searches through array of strings(sentences) to find keywords withing each string
        Note: there will be no duplicate words in words array
            no exclusion words will be in the words array"""
    def __createWordsList(self, sentences, exclusion):
        words = []

        # obtains and stores all keywords to be capitalized into words array
        for j in sentences:
            temp = j.split()
            for k in temp:
                if k.lower() not in words and k.lower() not in exclusion:
                    words.append(k.lower())
   
        return words    


    """ constructs a string in desired format with word in sentence capitalized
     Desired Format: 
         first letter of word String is at column 30
         no letters/words should come before column 10
         no letters/words should come after column 60
         if a word is cut off in between letters from the previous constraints remove the whole word"""
    def __getFinalPhrase(self, sentence, word):
        lowerCase = sentence.lower()
        separateLowerCase = lowerCase.split() 
        separateWords = sentence.split()
        phrase = sentence 
        indexTilWord = 0  
    
        """ obtains phrase with word capitalized
        obtains indexTilWord as index where word String starts"""
        phrase, indexTilWord  = self.__capitalize(phrase, word) 

        indexTilWord = indexTilWord - FIRST_LETTER_INDEX

        phrase = self.__removeFrontExcess(indexTilWord, phrase)   

        phrase = self.__addLengthSpaces(phrase)

        phrase = self.__addSpaceToIndex(phrase, word)
   
        # returns phrase with characters before column 10 as spaces and characters after column 60 removed
        return self.__edgeManagement(phrase)
   

    """removes characters from front of phrase until indexTilWord number of characters have been remove
       but if part of a word is cut off the whole word will be cut even if characters removed exceeds indexTilWord"""
    def __removeFrontExcess(self, indexTilWord, phrase):

        i = 0
        if indexTilWord > 0:
            temp = phrase[0]
            while(i < indexTilWord or (phrase[0] != " " and temp != " ")):
                temp = phrase[0]
                phrase = phrase[1:len(phrase)]
                i = i + 1    
        
        return phrase


    """adds spaces to phrase until length of them is 30
       Note: to avoid trying to access out of bounds later""" 
    def __addLengthSpaces(self, phrase):
    
        while(len(phrase) < FIRST_LETTER_INDEX + 1):
            phrase = " " + phrase   
    
        return phrase


    """adds spaces to front of phrase until column 30 holds the first letter of capitalized word"""
    def __addSpaceToIndex(self, phrase, word): 

        wordIndex = re.search(r"\b" + word + r"\b", phrase, re.IGNORECASE)    

        while(wordIndex.start() != 29):
            phrase = " " + phrase
            wordIndex = re.search(r"\b" + word + r"\b", phrase, re.IGNORECASE)   

        return phrase


    """creates new phrase with passed word capitalized
       returns phrase and index the word starts at(indexTilWord)"""
    def __capitalize(self, phrase, word):
 
        indexTilWord = 0
 
        phrase = re.sub(r"\b{}\b".format(word), word.upper(), phrase, flags = re.IGNORECASE)

        if word.upper() in phrase:
            indexTilWord = phrase.index(word.upper())

        return phrase, indexTilWord


    """iterates through keywords(words) and adds all sentence Strings in correct format to orderedSentences
       Note: this loop calls getFinalPhrase to construct the sentence in the correct format"""
    def __output(self, sentences, words, output):
        orderedSentences = [] 
  
        # searches though the sentences for string in words then calls getFinalPhrase for creating outputed phrase
        for j in words:
            for k in sentences: 
                if re.search(r"\b{}\b".format(j), k.lower()):
                    returnedPhrase = self.__getFinalPhrase(k, j) 
                    orderedSentences.append(returnedPhrase)

    

        return orderedSentences


    """ replaces characters before column 10 with " "
        removes characters that exceed colomn 60
            if character replaced is within a word perform action on entire word"""
    def __edgeManagement(self, phrase):

        # creates list of phrase to manipulate certain indices
        phraseList = list(phrase)

        phraseList = self.__leftEdge(phraseList)
    
        phraseList = self.__rightEdge(phraseList, phrase)
    
        # construct final string(phrase) to be returned
        i = 0
        phrase = ""
        while(i < len(phraseList)):
            phrase += phraseList[i]
            i = i + 1

    
        # return phrase
        return phrase

    # change characters preceeding column 10 to spaces and continues til no word is cut off in the middle
    def __leftEdge(self, phraseList):
    
        # locates index inwhich characters should be removed from 0-i
        i = MIN_INDEX
        if(phraseList[i] != " "):
            while(phraseList[i] != " "):
                i = i + 1

        # removes characters from 0-i
        while(i >= 0):
            phraseList[i] = " "
            i = i - 1

        
        return phraseList

    # removes characted proceeding column 60 and continues til no word is cut off in the middle 
    def __rightEdge(self, phraseList, phrase):
 
        # locates index inwhich capitalized word ends
        i = FIRST_LETTER_INDEX;
        while (i < len(phraseList) - 1 and (ord(phraseList[i]) < START_BOUND or ord(phraseList[i]) > END_BOUND)):
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
            i = i + 1
    
        return phraseList
