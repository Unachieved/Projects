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
        return 1;}
    if(file_len<=0 && regex_len!=0)return 0;
        
    int match_response = 0;
    int condition = 0;
    char * remaining_regex;
    char * remaining_file_line;
    int remaining_file_len = file_len;
    int ret =-1;
    // maintains the counter for how often the "*" or "+" repetition has been applied
    int used_count = 0;
    int reg_char_len = 0;
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
    if(regex[reg_char_len] == '+')condition=1;
    else if(regex[reg_char_len] == '*')condition=2;
    else if(regex[reg_char_len] == '?')condition=3;

    printf("CONDITION: %d\n", condition);
    if(1){
        int a_cond;
        a_cond = (condition!=0)?1:0;
        strcpy(remaining_regex, (regex+reg_char_len+a_cond));
        match_response = match(reg_char, file_char, reg_char_len);
        int rem_reg_len;
        (condition==2)? rem_reg_len = regex_len-(reg_char_len+1): 0;
         
        //if character matches then adjust remaining regex
        if(match_response){
            printf("MATCH\n");
            if(regex_len == constant_regex_len && (condition<=1 || rep_count<1)) {*start_loc = constant_file_len-file_len;}
            
            //MISSING MULTI
             if(condition==2 && rem_reg_len==0 && remaining_file_len==0){
                 line_free(remaining_regex,remaining_file_line,reg_char);
                 
                 if(*end_loc<constant_file_len-remaining_file_len-1)
                     *end_loc = constant_file_len-remaining_file_len-1;
                     
                 return 1;
             }
            
            //MISSING MULTI, PLUS
            if(rep_char == '*'||rep_char =='+')used_count=rep_count;
            else used_count=0;
             
            char * arg3; char arg7;
            int arg6, arg8 = regex_len;
            if(condition<=1){
                arg3=remaining_regex;
                arg6=0; arg7='\0';
                arg8-=reg_char_len;
                if(condition==1)arg8-=1;
            }
            else{
                arg3=regex;
                arg6=used_count+1;
                if(condition==2)arg7='*';
                else arg7='+';
            }
            //ret = line_check(remaining_file_line, constant_file_len, "remaining_regex|*+:regex", constant_regex_len, remaining_file_len, "0|*+:used_conut+1", "'\0'|*:'*'|+:'+'", "regex_len|'~:-(reg_char_len'|'?:+1')", start_loc, end_loc);
            ret = line_check(remaining_file_line, constant_file_len, arg3, constant_regex_len, remaining_file_len, arg6, arg7, arg8, start_loc, end_loc);
        }
    
        //MISSING QUESTION, MULTI, PLUS
        if(condition!= 0 && ret!=1 && (condition!=3 || (condition==3 && rep_count!=0)))
            ret = line_check(file_line, constant_file_len, remaining_regex, constant_regex_len, file_len, 0, '\0', regex_len-(reg_char_len+1), start_loc, end_loc);
     
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
    }
    
    line_free(remaining_regex,remaining_file_line,reg_char);
    return 0;
}
