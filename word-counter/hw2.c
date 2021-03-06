#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define MAX_HASH 10000019
#define MAX_LINE 1026

//BELOW IS THE FULL COMMAND TO COMPILE THE PROGRAM TO SEE ALL DEBUG INFO WHEN RAN
//gcc -Wall -Werror hw2.c -D DEBUG_MODE -D INDEPTH_DEBUG_MODE -D FREQ_DEBUG_MODE
char * AP89[50] = {"the","of","to","a","and","in","said","for","that","was","on","he","is","with","at","by","it","from","as","be","were","an","have","his","but","has","are","not","who","they","its","had","will","would","about","i","been","this","their","new","or","which","we","more","after","us","percent","up","one","people"};

struct list{
    char * line;
    int count;
    struct list * next;
    struct list * prev;
};
//  stores word-frequency pairs so they can be easily retrieved together
struct word_freq{
    char * valid_word;
    int freq_counter;
};

//  SEE INFORMATION ABOUT THE FOLLOWING FUNCTIONS ABOVE THEIR DEFINITIONS BELOW
void print_list(struct list * alist, int to_print);
int open_file(const char * filename, FILE ** fp);
struct list * read_file(FILE * fp, const char * regex_file, int regex_bits_read, int * lines_count);
void add_to_list(struct list ** words, char * remaining_words, struct list ** words_head);
int match(char * cur, char file_char, const int cur_len);
void sort_freq_list(struct list ** freq_list, char * word, int freq, int counter);
void sort_helper(struct word_freq ** n_word_freq, int * counter, int * total_words, struct list ** freq_list);
void free_list(struct list ** list, int inner);
void free_freq_list(struct word_freq *** array);


///////////////////////////////////////////////////                      MATCHING                        ///////////////////////////////////////////////////////
/*
 *  Gets the next character returning it as a "string"
 *  to accomodate for special characters that inluded a '\'
 *  case1: '\', case2: '[]', case3: 'any'
 */
void get_char(const char * regex, char ** reg_char, int * reg_char_len, const int regex_len){
    if(regex[0]=='\\'){
        strncpy(*reg_char ,regex, 2);
        *reg_char_len =2;
    }
    else if(regex[0]=='['){
        int itr = 1;
        while(regex[itr]!=']' && itr<regex_len)itr++;
        strncpy(*reg_char ,regex, itr+1);
        *reg_char_len =itr+1;
    }
    else {
        strncpy(*reg_char, regex, 1);
        *reg_char_len =1;
    }
}
/*
 *  Function implemented specifically for matching bracket regex inputs
 */
int bracket(char * cur, char file_char, const int cur_len){
    
    int itr, not;
    itr = 1; not = 0;
    if(cur[1]=='^'){not = 1; itr++;}
    char * next_char = calloc(128, sizeof(char));
    int n_char_len = 0;
    
    //checks each character in the brackets
    while(itr<(cur_len-1)){
        get_char(cur+itr, &next_char, &n_char_len, cur_len-itr);
        if(not){
            if(match(next_char, file_char, 0)){
                free(next_char);
                return 0;
            }
        }
        else {
            if(match(next_char, file_char, 0)){
                free(next_char);
                return 1;
            }
        }
        itr+=n_char_len;
    }
    free(next_char);
    if(not)return 1;
    else return 0;
}
/*
 *  Returns true if the characters match else false
 */
int match(char * cur, char file_char, const int cur_len){
    // ///////////////////////////     .     //////////////////////////////////
    // match any character
    if(strcmp(cur,".")==0)return 1;
    // ///////////////////////////     d     //////////////////////////////////
    // match digit character
    if(strcmp(cur,"\\d")==0){
        if(isdigit(file_char)!=0)return 1;
    }
    // ///////////////////////////     D     //////////////////////////////////
    // match non digit character
    if(strcmp(cur,"\\D")==0){
        if(isdigit(file_char)==0)return 1;
    }
    // ///////////////////////////     w     //////////////////////////////////
    // match alpha character
    if(strcmp(cur,"\\w")==0){
        if(isalpha(file_char)!=0)return 1;
    }
    // ///////////////////////////     W     //////////////////////////////////
    // match non alpha character
    if(strcmp(cur,"\\W")==0){
        if(isalpha(file_char)==0)return 1;
    }
    // ///////////////////////////     s     //////////////////////////////////
    // match space
    if(strcmp(cur,"\\s")==0){
        if(file_char == ' '|| file_char == '\t')return 1;
    }
    // ///////////////////////////     \     //////////////////////////////////
    // match \ character
    if(strcmp(cur,"\\\\")==0){
        if(file_char == '\\')return 1;
    }
    // /////////////////////////     [] & [^]     /////////////////////////////
    // match atleast one character given in the brackets
    // if delimited by ^ then match a caharacter that doesn't match any of the characters in the bracket
    if(cur_len !=0 && cur[0]=='['){
        return bracket(cur, file_char, cur_len);
    }
    else {
        if(cur[0]==file_char)return 1;
    }
    return 0;
}
/*
 *  helper function for line_check. It is just in charge of freeing
 *  the memory when an iteration is ending
 */
void line_free(char * remaining_regex, char * remaining_file_line, char * reg_char){
    free(remaining_regex);
    free(remaining_file_line);
    free(reg_char);
}
/*
 *  This is the main regex matching functions. It does matching by recurssively going through the
 *  line and greedily matches the regex to the line. If a section fails it back tracks to a
 *  previous point of success until it can no longer do so in which case the current comparison
 *  fails
 *  rep is short for repetition (keeps track of "+" and "*" repetitions)
 *  reg is short for regex
 *  start_loc and end_loc delimite the indices where the regex match starts and ends relative to the
 *  line if the line has a match to the regex
 *  n.b end is is index after the last matched character
 */
int line_check(const char * file_line, const int constant_file_len, const char * regex, const int constant_regex_len, const int file_len, int rep_count, char rep_char, const int regex_len, int * start_loc, int * end_loc){
    
#ifdef INDEPTH_DEBUG_MODE
    printf("DEBUG: regex_len: %d, regex: %d, file_len: %d\n", constant_regex_len, regex_len, file_len);
#endif
    
    if(regex_len<=0){
        *end_loc = constant_file_len-file_len;
        return 1;
    }
    if(file_len<=0 && regex_len!=0)return 0;
    
    int match_response, condition, ret, used_count, reg_char_len, remaining_file_len;
    char * remaining_regex;
    char * remaining_file_line;
    
    match_response = condition = used_count = reg_char_len = 0;
    remaining_file_len = file_len;
    ret =-1;
    
    //  Get the characters to be matched from the regex and text file
    char * reg_char = calloc(128, sizeof(char));
    get_char(regex, &reg_char, &reg_char_len, regex_len);
    char file_char = file_line[0];
    remaining_file_len--;
    
#ifdef INDEPTH_DEBUG_MODE
    if(reg_char_len==1)printf("DEBUG:    reg_char %c   file_line %c\n", *reg_char, file_char);
    else if(reg_char_len>1) printf("DEBUG:    reg_char %s   file_line %c\n", reg_char, file_char);
#endif
    
    remaining_regex = calloc(256, sizeof(char));
    remaining_file_line = calloc(MAX_LINE, sizeof(char));
    if(strlen(file_line)>1)strcpy(remaining_file_line, (file_line+1));
    
    //  checks whether the current regex character to be compared is
    //  followed by a repetition special character
    if(regex[reg_char_len] == '?')condition=1;
    else if(regex[reg_char_len] == '*')condition=2;
    else if(regex[reg_char_len] == '+')condition=3;
    
    int a_cond, rem_reg_len;;
    a_cond = (condition!=0)?1:0;
    strcpy(remaining_regex, (regex+reg_char_len+a_cond));
    match_response = match(reg_char, file_char, reg_char_len);
    (condition==2)? rem_reg_len = regex_len-(reg_char_len+1): 0;
    
    //if character matches then adjust remaining regex
    if(match_response){
        if(regex_len == constant_regex_len && (condition<=1 || rep_count<1)) {*start_loc = constant_file_len-file_len;}
        
        //CASE: '*': --- at the end of file string just return match on case of '*' since 0 reps is possible
        if(condition==2 && rem_reg_len==0 && remaining_file_len==0){
            line_free(remaining_regex,remaining_file_line,reg_char);
            
            if(*end_loc<constant_file_len-remaining_file_len-1)
                *end_loc = constant_file_len-remaining_file_len-1;
    
            return 1;
        }
        
        //CASES: '*','+' --- handles repetition characters
        if(rep_char == '*'||rep_char =='+')used_count=rep_count;
        else used_count=0;
        
        //To setup inputs for different cases that will be passed into the line_check bellow
        char * arg3; char arg7;
        int arg6, arg8 = regex_len;
        if(condition<=1){
            arg3=remaining_regex;
            arg6=0; arg7='\0';
            arg8-=reg_char_len;
            if(condition==1)arg8-=1;
        }
        else{
            arg3=(char *)regex;
            arg6=used_count+1;
            if(condition==2)arg7='*';
            else arg7='+';
        }
        ret = line_check(remaining_file_line, constant_file_len, arg3, constant_regex_len, remaining_file_len, arg6, arg7, arg8, start_loc, end_loc);
    }
    
    //CASES: '?','*','+'
    if(condition!= 0 && ret!=1 && (condition!=3 || (condition==3 && rep_count!=0))){
        ret = line_check(file_line, constant_file_len, remaining_regex, constant_regex_len, file_len, 0, '\0', regex_len-(reg_char_len+1), start_loc, end_loc);
    }
        
    //Success State
    if(ret==1){
        line_free(remaining_regex,remaining_file_line,reg_char);
        //assign end_loc on the final match --- this keeps it from reassigning during return
        if(*end_loc<constant_file_len-remaining_file_len-1)
            *end_loc = constant_file_len-remaining_file_len-1;
        
        return 1;
    }
    
    //  if this iteration did not match and it is the begining of the
    //  regex check next char in line
    else if(constant_regex_len == regex_len && ((condition>1 && rep_count==0)||condition <=1)) {
        ret = line_check(remaining_file_line, constant_file_len, regex, constant_regex_len,
                         remaining_file_len, 0, '\0', regex_len, start_loc, end_loc);
        line_free(remaining_regex,remaining_file_line,reg_char);
        return ret;
    }
    //The characters match in no scenerio
    else {
        line_free(remaining_regex,remaining_file_line,reg_char);
        return -1;
    }
    
    line_free(remaining_regex,remaining_file_line,reg_char);
    return 0;
}
/*  regex_match() applies the given regex to each line of the file
 *  specified by filename; all matching lines are stored in a
 *  dynamically allocated array called matches; note that it is
 *  up to the calling process to free the allocated memory here
 *
 *  regex_match() returns the number of lines that matched (zero
 *   or more) or -1 if an error occurred
 */
int regex_match( const char * filename, const char * regex, char *** matches ){
    
    FILE * fp2;
    int regex_bits_read = strlen(regex);
    
#ifdef DEBUG_MODE
    printf("DEBUG: %s\n", regex_file);
#endif
    //  file to match to
    if(open_file(filename, &fp2)==-1)return -1;
    int lines_count = 0;
    struct list * matched_lines = read_file(fp2, regex, regex_bits_read, &lines_count);
    
    (*matches) = calloc(lines_count, sizeof(char **));
    
    int itr = 0;
    struct list * iterator = matched_lines;
    while(iterator!=NULL && itr<lines_count){
        (*matches)[itr]=iterator->line;
        itr++;
        iterator=iterator->next;
    }
    
    fclose(fp2);
    free_list(&matched_lines, 0);
    return lines_count;
}

///////////////////////////////////////////////////                  WORD FREQUENCY                    ///////////////////////////////////////////////////////
/*
 *  "removes" characters designated by open tag left and open tag right
 *  (refering to start and end position) if action != 1 or 4. Otherwise
 *  it removed all between open tag left and close tag right if. close tags
 *  are assigned if the regex finds a match in file_line.
 *  VARIABLES:
 *      file_line: string on which removal is conduction
 *      open_tag_L: starting index of match found in calling function
 *      open_tag_R: ending index of match found in calling function
 *      action: either 1 or 4 --- 1 means the removal is for a script so removel all between as well
 *                                 4 has the same purpose as the above but for multiline tags and another
 *                                 purpose in outside functions
 */
int remove_section(char * file_line, int file_bits_read, char * regex, int regex_bits,
                   int open_tag_L, int open_tag_R, char * remaining, int action, struct list ** maybe_remove){
    
    //this structure is accessed and manipulated as file_line and remaining are pointing to the same thing.
    char * temp = calloc(MAX_LINE, sizeof(char));
    strcpy(temp, file_line);
    
    int close_tag_L, close_tag_R;
    close_tag_L = close_tag_R = 0;
    if(action!=1&&action!=4){close_tag_L= open_tag_L;close_tag_R=open_tag_R;}
    
    if((action!=1 && action!=4) || line_check(file_line, file_bits_read, regex, regex_bits, file_bits_read, 0, '\0', regex_bits, &close_tag_L, &close_tag_R)){
        //copy segments of line before the tag is introduced
        if(strlen(remaining)<1)strncpy(remaining, temp, open_tag_L);
        //if strncpy isn't long enough and remaing had other stuff, it stays a part of remain
        //so terminate the string to get rid of extra values
        remaining[open_tag_L]='\0';
#ifdef FREQ_DEBUG_MODE
        printf("DEBUG: pre-combination: %s\n", remaining);
        printf("removing all or part of: %s\n", temp+open_tag_L);
#endif
        //A removed section only needs to be stored in the case of multiline for the edge case mentioned in parse_to_words
        if(action==5||action==4){
            char * temp_remv_sec = calloc(MAX_LINE, sizeof(char));
            strncpy(temp_remv_sec, temp+open_tag_L, close_tag_R);
            struct list * temp_head = NULL;
            add_to_list(maybe_remove, temp_remv_sec, &temp_head);
        }
        
        if(close_tag_R!=file_bits_read) {
            //copy everything after the end script in the line
            if(open_tag_L!=0){
                if(action==2 || action==5){strcat(remaining, " ");}
                strcat(remaining, temp+close_tag_R);
            }
            else strcpy(remaining, temp+close_tag_R);
        }
#ifdef FREQ_DEBUG_MODE
        printf("DEBUG: post-combination: %s...\n", remaining);
#endif
        
        free(temp);
        return 0;
    }
    // it's in some other line (for multiline scripts and tags)
    else {
        remaining[0]='\0';
        //if this is for multiline tags save the portion that might be removed
        if(action==5||action==4){
            struct list * temp_head = NULL;
            add_to_list(maybe_remove, temp, &temp_head);
        }
        else free(temp);
        return action;
    }
}
/*
 *  this is a helper to facilitate removal implemented above
 *  it mainly loops through the line after successive removals to make
 *  sure that all such matches have been completely removed from the untouched
 *  portions of the string
 *  VARIABLES:
 *      edited: if a removal was performed on the string edited is set to true
 *              - edited is used to determine what to changes to the string in function parse_to_words
 */
int parse_helper(char * remaining, char * regex, int reg_len, int action, int * edited, struct list ** maybe_remove){
    
    int parse_complete = 0;
    int open_tag_L, open_tag_R, prev_tag_L;
    open_tag_L = open_tag_R = 0;
    
    int ret;
    while(!parse_complete){
        parse_complete=1;
        prev_tag_L=open_tag_L;
        if(line_check(remaining+prev_tag_L, strlen(remaining)-prev_tag_L, regex, reg_len, strlen(remaining)-prev_tag_L, 0, '\0', reg_len, &open_tag_L, &open_tag_R)){
            parse_complete=0;
            (*edited) =1;
            ret = remove_section(remaining, strlen(remaining), NULL, -1, open_tag_L+prev_tag_L, open_tag_R+prev_tag_L, remaining, action, maybe_remove);
        }
    }
    return ret;
}
/*  this funciton parses the input into strings consisting of only words
 stripping scripts, tags and non words (still includes single alpha characters)
 action --- 0 (regular check), 1 (multi line check),
 */
int parse_to_words(char * file_line, int file_bits_read, int * action, char * remaining, struct list ** maybe_remove, struct list ** maybe_remove_head){
    
    int open_tag_L, open_tag_R, edited;
    open_tag_L = open_tag_R = edited = 0;
    
    //TODO: run through multiple times incase multiple scripts were placed in the same line
    strcpy(remaining, file_line);
    
    //ADD LOOP --- SCRIPTS
    if((*action) == 0 &&
       line_check(remaining, strlen(remaining), "<script[^<]*[>]?", 16,
                  strlen(remaining), 0, '\0', 16, &open_tag_L, &open_tag_R)){
        (*action)=1;
    }
    if((*action) == 1){
        (*action) = remove_section(remaining, strlen(remaining),
                                   "</script>", 9, open_tag_L, open_tag_R, remaining, *action, maybe_remove);
        if(edited==0)remaining[0]='\0';
    }
    if((*action)==1)return 1;
    
    //  remove tags
    parse_helper(remaining, "<[^<>]+>", 8, 2, &edited, maybe_remove);
    
    //  remove special html/xml characters
    //  uncomment the call bellow if all special character of the form bellow should be removed;
    //  parse_helper(remaining, "&[^;]+;", 7, 3, &edited);
    parse_helper(remaining, "&nbsp;", 6, 3, &edited, maybe_remove);
    parse_helper(remaining, "&quot;", 6, 3, &edited, maybe_remove);
    parse_helper(remaining, "&amp;", 5, 3, &edited, maybe_remove);
    parse_helper(remaining, "&lt;", 4, 3, &edited, maybe_remove);
    parse_helper(remaining, "&gt;", 7, 3, &edited, maybe_remove);
    
    //  find end tag of mutiline tags
    if((*action) == 4){
        (*action) = remove_section(remaining, strlen(remaining), "[^>]*>", 6, open_tag_L, open_tag_R, remaining, 4, maybe_remove);
        
        if(*action==4)return 4;
        else {
            //if the end tag is found then nothing between the tags are included
            //this if for the edge case of a multi-line tag not having an end tag
            free_list(maybe_remove_head, 1);
            (*maybe_remove_head) = NULL;
            (*maybe_remove) = NULL;
        }
    }
    //  removes all portions the beginings part of multiline tags
    //  return 4 to shows that until multiline end found then nothing is saved
    while((*action) == 0 && line_check(remaining, strlen(remaining), "<.*", 3,
                                       strlen(remaining), 0, '\0', 3, &open_tag_L, &open_tag_R)){
        parse_helper(remaining, "<.*", 3, 5, &edited, maybe_remove);
        (*action)=4;
        return 4;
    }
    
    //  all non apostrophe symbols
    parse_helper(remaining, "[^\\w'\\s\n]+", 10, 2, &edited, maybe_remove);
    
#ifdef DEBUG_MODE
    printf("GUBED3: %s...\n", remaining);
#endif
    return 0;
}

//  computes has of string for array of size MAX_HASH
long long compute_hash(const char * s) {
    
    if(s==NULL)return -1;
    const int p = 31;
    int len_s = strlen(s);
    const int m = MAX_HASH;
    long long hash_value = 0;
    long long p_pow = 1;
    for (int i =0;i<len_s;i++) {
        hash_value = (hash_value + (s[i] + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_value;
}
/*
 *  checks if string str is in the top 50 AP89 word occurences
 *  used to isolate interesting words
 */
int contained(char * str, char ** array){
    
    for(int i =0; i<50;i++){
        if(strcmp(str,array[i])==0)return 1;
    }
    return 0;
}
/*
 *  Gets all "words" as declared by the specifications from the remainder of lines
 *  These words are denoted by the regex mentioned below. This utilizes the regex functions
 *  and the removal functions. The distinct words are returned in a list
 *  n.b. all words are lowercased as per specification
 */
struct list * get_n_words(struct list ** parsed_lines){
    
    int all_retrieved = 0;
    struct list * itr = (*parsed_lines);
    int start_index, end_index;
    
    struct list * all_words_head = NULL;
    struct list * all_words_end = NULL;
    struct list * removed_sections = NULL;
    
    char * regex = "\\w+'?\\w+";
    while(itr!=NULL){
        
        while(!all_retrieved){
            
            all_retrieved=1;
            int line_len = strlen(itr->line);
            int regex_len = strlen(regex);
            if(line_check(itr->line, line_len, regex, regex_len,line_len, 0, '\0', regex_len, &start_index, &end_index)){
                all_retrieved=0;
                //get the found word and add it to hash if not already in else increment counter
                int sub_len = end_index-start_index;
                char * sub_string = calloc(64, sizeof(char));
                strncpy(sub_string, (itr->line)+start_index,sub_len);
                
                //makes the substring lower case
                for(int i = 0; sub_string[i]; i++){
                    sub_string[i] = tolower(sub_string[i]);
                }
                
                add_to_list(&all_words_end, sub_string, &all_words_head);
                
                //remove the last word found from the string and step the new string to search as what remains
                char * new_line = calloc(line_len, sizeof(char));
                //printf("calloc_get_n2: %p\n", new_line);
                remove_section(itr->line, line_len, regex, regex_len, start_index, end_index, new_line, 3, &removed_sections);
                free(itr->line);
                itr->line = new_line;
            }
        }
        all_retrieved=0;
        itr=itr->next;
    }
    
    free(removed_sections);
    return all_words_head;
}
/*
 *  helper function for freq_hash.
 *  This allocations the space in the words array and sets the appropriate word
 *  and frequency counter
 */
void hash_assign(long long hash_value, struct word_freq ** words, char * sub_string){
    
    words[hash_value] = calloc(1, sizeof(struct word_freq));
    words[hash_value]->valid_word = sub_string;
    words[hash_value]->freq_counter=1;
}
/*
 *  adds the word to fitting location in the hash table
 *  resolving potential hash collision.
 *  hash collisions are resolved by linearly moving through the array and placing
 *  the new entry in the nearest empty slot
 */
int freq_hash(char * sub_string, struct word_freq ** words){
    
    long long hash_value = compute_hash(sub_string);
    
    //If index is taken then the words has either occured or there is a hash collision
    if(words[hash_value]!=NULL){
        //increment if word is in this index
        if (strcmp(sub_string,words[hash_value]->valid_word)==0) {
            words[hash_value]->freq_counter++;
            return 0;
        }
        /*  HASH COLLISION HANDLING
         *  else move to next free slot or the slot where a previous occurence was place
         *  and insert or increment it accordingly
         */
        else {
            //search through hash tablle to see if the value was placed in another box.
            //if value found increment but if NULL found then value does not exist
            while(hash_value<MAX_HASH && words[hash_value]!=NULL){
                if(strcmp(sub_string,words[hash_value]->valid_word)==0){
                    words[hash_value]->freq_counter++;
                    return 0;
                }
                hash_value++;
            }
            
            //if the value is NULL
            if(words[hash_value]==NULL)
                hash_assign(hash_value, words, sub_string);
            //start from the front of the array untill empty spot or found occurence
            else if(hash_value==MAX_HASH){
                hash_value=0;
                while(hash_value<MAX_HASH && words[hash_value]!=NULL){
                    if(strcmp(sub_string,words[hash_value]->valid_word)==0){
                        words[hash_value]->freq_counter++;
                        return 0;
                    }
                    hash_value++;
                }
                if(words[hash_value]==NULL)
                    hash_assign(hash_value, words, sub_string);
            }
        }
    }
    // first occurence of the word so add it to the array and start counter
    else
        hash_assign(hash_value, words, sub_string);
    return 1;
}

void print_list(struct list * alist, int to_print){
    
    int counter = 1;
    while(alist!=NULL && counter<to_print+1){
        /*Comment out the above loop and the print bellow the uncomment the 2 lines below to
        view all possible mono,bi,and trigrams for the files.*/
    /*while(alist!=NULL){
        printf("#%d:\t%d\t%s\n", counter, alist->count, alist->line );*/
        printf("%d %s\n", alist->count, alist->line );
        alist=alist->next;
        counter++;
    }
}
/*
 * this function is the heart of the frequency list
 *  i.e. it hashes the entries and removes uninteresting words when called for
 */
void get_freq_list(int num,  struct list * words, struct word_freq ** freq_list, int check_for_interesting){
    
    int counter = 0;
    struct list * itr = words;
    struct list ** n_array = calloc(num, sizeof(struct list *));
    while(counter<num && itr!=NULL){
        n_array[counter]=itr;
        counter++;
        itr=itr->next;
    }
    
    if(counter==num){
        itr = words;
        int end =0;
        //n_grams are able to be made until there isn't a combination of size n possible
        while(n_array[num-1]!=NULL){
            
            //if there exists a/an uninteresting word/s the this possible n_gram
            //go to successive n_graming until all words are interesting
            if(check_for_interesting){
                int not_inter = 1;
                while(not_inter){
                    not_inter=0;
                    for(int j=0; j<num;j++){
                        if(contained(n_array[j]->line,AP89)){
                            not_inter =1;
                            for(int k=0; k<num;k++){
                                if(n_array[k]->next==NULL)end=1;
                                n_array[k]=n_array[k]->next;
                            }
                        }
                        if(end)break;
                    }
                    if(end)break;
                }
                if(end)break;
            }
            
            //concat words to n_gram expected size
            char * temp = calloc(256, sizeof(char));
            for(int i=0; i<num;i++){
                
                if(i==0)strcpy(temp, n_array[i]->line);
                else {
                    strcat(temp, " ");
                    strcat(temp,n_array[i]->line);
                }
                n_array[i]=n_array[i]->next;
            }
            //hash the n_gram word
            if(!freq_hash(temp, freq_list))free(temp);
        }
    }
    free(n_array);
}
/*
 *  function purposed with freeing the freq_list structure
 *  it clears the structures internals as well
 */
void free_freq_list(struct word_freq *** array){
    for (int i =0; i<MAX_HASH; i++) {
        if((*array)[i]){
            free((*array)[i]->valid_word);
            free((*array)[i]);
        }
    }
    free(*array);
}
/*
 *  function purposed with freeing the list structure
 *  VARIBALES:
 *          inner: this is either 1 or 0. If 1 then free the word memory
 *                  else leave it as it will be use in another structure and freed later
 */
void free_list(struct list ** alist, int inner){
    struct list * temp = *alist;
    int i = 1;
    while(temp!=NULL){
        
        (*alist) = temp;
        temp=temp->next;
        if(inner){
            free((*alist)->line);
        }
        free(*alist);
        i++;
    }
}
/*
 *  primarily just calls opens and reads for files
 *  then calls the hearty functions for finding frequencies
 *  the words frequency arrays are filled via this function
 */
int freq_helper(const char * filename, struct word_freq ** mono_word_freq,
                 struct word_freq ** bi_word_freq, struct word_freq ** tri_word_freq ){
    FILE * fp;
    int lines_count;
    lines_count = 0;
    if(open_file(filename, &fp)==-1)return -1;
    
    struct list * parsed_lines = read_file(fp, NULL, 0, &lines_count);
    fclose(fp);
    
    struct list * all_words;
    all_words = get_n_words(&parsed_lines);
    
    get_freq_list(1, all_words, mono_word_freq, 0);
    get_freq_list(2, all_words, bi_word_freq, 1);
    get_freq_list(3, all_words, tri_word_freq, 1);
    free_list(&parsed_lines,1);
    free_list(&all_words,1);
    return 0;
}
/*
 * poduces expected output as per the specifications and frees the structure when done
 */
void freq_output(struct word_freq *** mono_word_freq, struct word_freq *** bi_word_freq,
                 struct word_freq *** tri_word_freq, int arg_count){
    struct list * freq_list1 = NULL;
    struct list * freq_list2 = NULL;
    struct list * freq_list3 = NULL;
    
    int counter, counter2, counter3, total_words, total_words2, total_words3;
    counter = counter2 = counter3 = total_words = total_words2 = total_words3 = 0;
    
    printf("Total number of documents: %d\n", arg_count-1);
    sort_helper(*mono_word_freq, &counter, &total_words, &freq_list1);
    printf("Total number of words: %d\n",total_words);
    printf("Total number of unique words: %d\n", counter);
    sort_helper(*bi_word_freq, &counter2, &total_words2, &freq_list2);
    printf("Total number of interesting bigrams: %d\n",total_words2);
    printf("Total number of unique interesting bigrams: %d\n", counter2);
    sort_helper(*tri_word_freq, &counter3, &total_words3, &freq_list3);
    printf("Total number of interesting trigrams: %d\n",total_words3);
    printf("Total number of unique interesting trigrams: %d\n\n", counter3);
    
    printf("Top 50 words:\n");
    print_list(freq_list1, 50);
    free_list(&freq_list1,0);
    printf("\n");
    
    total_words=0;
    counter=0;
    printf("Top 20 interesting bigrams:\n");
    print_list(freq_list2, 20);
    free_list(&freq_list2,0);
    printf("\n");
    
    total_words=0;
    counter=0;
    printf("Top 12 interesting trigrams:\n");
    print_list(freq_list3, 12);
    free_list(&freq_list3,0);
    
    free_freq_list(mono_word_freq);
    free_freq_list(bi_word_freq);
    free_freq_list(tri_word_freq);
}
/*
 *  houses frequency data structures and calls the main functions for getting
 *  words frequency related tasks done
 */
int frequency_handler(int arg_count, char * arguments[]){
    
    struct word_freq ** mono_word_freq  = calloc(MAX_HASH, sizeof(struct word_freq *));
    struct word_freq ** bi_word_freq  = calloc(MAX_HASH, sizeof(struct word_freq *));
    struct word_freq ** tri_word_freq  = calloc(MAX_HASH, sizeof(struct word_freq *));
    
    for (int i =0; i<MAX_HASH; i++) {
        mono_word_freq[i]=NULL;
        bi_word_freq[i]=NULL;
        tri_word_freq[i]=NULL;
    }
    //get the frequencies and place them into the given structures
    for(int i=1;i<arg_count; i++){
        if(freq_helper(arguments[i],mono_word_freq, bi_word_freq, tri_word_freq)==-1)return -1;
    }
    
    freq_output(&mono_word_freq, &bi_word_freq, &tri_word_freq, arg_count);
    
    return 0;
}
/*
 *  Iterates through the hash array of word/frequency pairs and sorts them into a list
 *  so it is easier to provide output. Counter and Total words are incremented according
 *  To their mentioned description below
 *  VARIABLES:
 *      n_word_freq: hash array of word/frequency pairs
 *      counter: maintaains count of unique words/n_grams
 *      total_words: maintains count of total words/n_grams
 *      freq_list: list for sorted word/frequency pairs
 *              - passed in empty but populated as mentioned in the description
 */
void sort_helper(struct word_freq ** n_word_freq, int * counter, int * total_words, struct list ** freq_list){
    
    for(int i =0; i<MAX_HASH; i++){
        if(n_word_freq[i]!=NULL){
            (*counter)++;
            (*total_words)+= n_word_freq[i]->freq_counter;
            sort_freq_list(freq_list, n_word_freq[i]->valid_word, n_word_freq[i]->freq_counter, (*counter));
        }
    }
}
/*
 sorts frequency list in decending order according to frequency first the alpha order
 */
void sort_freq_list(struct list ** freq_list, char * word, int freq, int counter){
    
    if((*freq_list)==NULL){
        (*freq_list) = calloc(1, sizeof(struct list));
        (*freq_list)->line = word;
        (*freq_list)->count = freq;
        (*freq_list)->next = NULL;
        (*freq_list)->prev = NULL;
    }
    else {
        struct list * temp = calloc(1, sizeof(struct list));
        temp->line = word;
        temp->count = freq;
        
        struct list * itr = (*freq_list);
        if(itr->count<temp->count || (itr->count==temp->count && strcmp(itr->line,temp->line)>0)){
            temp->next = itr;
            temp->prev = NULL;
            itr->prev = temp;
            (*freq_list)=temp;
            return;
        }
        while(itr->next!=NULL && (itr->next->count>temp->count)){
            itr=itr->next;
        }
        while(itr->next!=NULL && itr->next->count==temp->count && strcmp(itr->next->line,temp->line)<0){
            itr=itr->next;
        }
        if(itr->next==NULL){
            itr->next=temp;
            temp->prev=itr;
            temp->next = NULL;
            return;
        }
        else{
            temp->next = itr->next;
            temp->prev=itr;
            itr->next=temp;
            return;
        }
    }
}

///////////////////////////////////////////////////                      GENERAL                        ///////////////////////////////////////////////////////
/*
 *  reads and directs files to their expected parsing functions
 *  returns and list of the lines which remaing follow the desired parsing/eval
 */
struct list * read_file(FILE * fp, const char * regex_file, int regex_bits_read, int * lines_read){
    
    char * file_line = NULL;
    size_t line_len = 0;
    ssize_t file_bits_read;
    int start, end;
    start = end = 0;
    char * remaining_words = NULL;
    struct list * words = NULL;
    struct list * words_head = NULL;
    struct list * maybe_remove = NULL;
    struct list * maybe_remove_head = NULL;
    int action = 0;
    
    //the file is read line by line and each line is checked in turn.
    while((file_bits_read = getline(&file_line, &line_len, fp)) != -1){
#ifdef FREQ_DEBUG_MODE
        printf("DEBUG: %s\n", file_line);
#endif
        //  for the case of only regex matching
        if(regex_file!=NULL){
            if(line_check(file_line, (int)file_bits_read, regex_file, (int)regex_bits_read,
                          (int)file_bits_read, 0, '\0', (int)regex_bits_read, &start, &end)){
                
                
                char * temp = calloc(MAX_LINE, sizeof(char));
                strcpy(temp, file_line);
                if(temp[0]!='\n'){
                    (*lines_read)++;
                    if(temp[strlen(temp)-1]=='\n')temp[strlen(temp)-1]='\0';
                    add_to_list(&words, temp, &words_head);
                }
                else free(temp);
            }
        }
        //  for frequencies
        else{
            /*memory is allocated in the following cases:
                -start of program
                -the memory previouslly allocated is still in use or will be used
                -memory was modified by something then freed
            */
            if(remaining_words==NULL)
                remaining_words = calloc(MAX_LINE, sizeof(char));
            
            //action is set for specific cases from parse_to_words --- see said function for what each return means
            action = parse_to_words(file_line, (int)file_bits_read, &action,remaining_words, &maybe_remove, &maybe_remove_head);
            if(action==1)
                remaining_words =  realloc(remaining_words, sizeof(int)*MAX_LINE);
            //set the head if maybe_remove was just initialized
            if(maybe_remove_head==NULL && maybe_remove!=NULL)
                maybe_remove_head=maybe_remove;
            
            //free the memory if the possible string to add is empty or does not contain any words
            if(strlen(remaining_words)<1){
                free(remaining_words);
                remaining_words=NULL;
                continue;
            }
            if(!line_check(remaining_words, strlen(remaining_words), "\\w", 2, strlen(remaining_words), 0, '\0', 2, &start, &end)){
                free(remaining_words);
                remaining_words=NULL;
                continue;
            }
            
            add_to_list(&words, remaining_words, &words_head);
            remaining_words=NULL;
        }
#ifdef DEBUG_MODE
        //visually separates each line read
        printf("----------------------------------------------------------\n");
#endif
    }
    if(action==4){
        if(maybe_remove!=NULL){
            if(words_head==NULL)
                words_head=maybe_remove_head;
            else
                words->next = maybe_remove_head;
        }
    }
    
    free(file_line);
    return words_head;
}
/*
 *  adds a value to the defined list structure. Structure is initialized if it was NULL
 */
void add_to_list(struct list ** words, char * remaining_words, struct list ** words_head){
    if((*words)==NULL){
        (*words) = calloc(1, sizeof(struct list));
        (*words_head) = (*words);
        (*words)->line = remaining_words;
        (*words)->next = NULL;
        (*words)->prev = NULL;
    }
    else{
        (*words)->next = calloc(1, sizeof(struct list));
        (*words)->next->prev = (*words);
        (*words) = (*words)->next;
        (*words)->line = remaining_words;
        (*words)->next = NULL;
    }
}
/*
 * opens file with given file name and sets up file pointer
 *  returns -1 if error or 0 otherwise
 */
int open_file(const char * filename, FILE ** fp){
    
    *fp = fopen(filename, "r");
    if (*fp == NULL){
        fprintf(stderr, "ERROR: Invalid arguments");
        return -1;
    }
    
    return 0;
}

#ifndef USE_SUBMITTY_MAIN
int main(int argc, char * argv[]){
    setvbuf( stdout, NULL, _IONBF, 0 );
    
    if(argc < 2){
        fprintf(stderr, "USAGE: a.out <input-file1> [<input-file2> ...]\n");
        return 1;
    }
    
    if(frequency_handler(argc, argv)==-1)return -1;
}
#endif
