#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256,
	EQ,     //==
	NUM,    //数字
	HEX,    //16进制数字,以0x开头
	REG,
	SYMB,
	NEQ,    // !=
	LE,     // <=
	GE,     // >=
	LAND,   // &&
	LOR,    // ||
	SHL,    // <<
	SHR,    // >>
    NEG,    // -
    MEM,    // *
	/* TODO: Add more token types */

};
enum{
    OPR_VAL, 
    OPRATOR_1OP,
    OPRATOR_2OP,
    BRACKETS,
};
static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"0[Xx][0-9a-fA-F]+",HEX},
	{"[0-9]+",NUM},
	{"\\$[a-z]+",REG},
	{"\\+",'+'},
	{"-",'-'},
	{"\\*",'*'},
	{"/",'/'},
	{"\\(",'('},
	{"\\)",')'},
	{"==",EQ},
	{"!=",NEQ},
	{"<=",LE},
	{">=",GE},
	{"&&",LAND},
	{"\\|\\|",LOR},
	{"&",'&'},
	{"\\|",'|'},
	{"\\^",'^'},
	{"~",'~'},
	{"!",'!'},
	{"<<",SHL},
	{">>",SHR},
	{"[_A-Za-z]+[_A-Za-z0-9]",SYMB},
	{" +", NOTYPE}, // white space
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

#define TOKEN_LEN (64)
#define TOKEN_SIZE (512)
typedef struct token
{
	int type;
	char str[TOKEN_LEN+1];
} Token;

Token tokens[TOKEN_SIZE];
int nr_token;
static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;
	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				// printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

                if(rules[i].token_type!=NOTYPE && nr_token==TOKEN_SIZE){
                    printf("TOKEN SIZE exceed!\n");
                    return false;
                }
				switch (rules[i].token_type)
				{
				case NOTYPE:
				    break;
				case NUM:
				    tokens[nr_token].type=rules[i].token_type;
				    if(substr_len>TOKEN_LEN){
				        printf("Number Length exceed at %d\n%s\n%*.s^\n", position, e, position, "");
				        return false;
				    }
				    memcpy(tokens[nr_token++].str,substr_start,substr_len);
				    tokens[nr_token-1].str[substr_len]='\0';
				    break;
				case HEX:
				    tokens[nr_token].type=rules[i].token_type;
				    if(substr_len>TOKEN_LEN){
				        printf("HEX Number Length exceed at %d\n%s\n%*.s^\n", position, e, position, "");
				        return false;
				    }
				    memcpy(tokens[nr_token++].str,substr_start,substr_len);
				    tokens[nr_token-1].str[substr_len]='\0';
				    break;
				default:
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}
                
				break;
			}
		}
		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true;
}

// 获取运算符的优先度
uint32_t getprio(Token* mytoken){
    switch(mytoken->type){
        case LOR:
            return 12;
        case LAND:
            return 11;
        case '|':
            return 10;
        case '^':
            return 9;
        case '&':
            return 8;
        case EQ:case NEQ:
            return 7;
        case '>':case '<':case LE:case GE:
            return 6;
        case SHL:case SHR:
            return 5;
        case '+':case '-':
            return 4;
        case '*':case '/':case '%':
            return 3;
        case NEG:case MEM:case '!':case '~':
            return 2;
        default:
            return -1;
    }
}
// 获取运算符的类型(操作符/操作数)
uint32_t gettype(Token* mytoken){
    uint32_t temp=getprio(mytoken);
    if(~temp){
        return (temp>2)?OPRATOR_2OP:OPRATOR_1OP;
    }
    switch(mytoken->type){
        case NUM:case REG:case HEX:case SYMB:
            return OPR_VAL;
        case '(':case ')':
            return BRACKETS;
    }
    return -1;
}
// uint32_t eval(int l,int r,bool *success){
//     if(l>r){
//         success=false;
//         return 0;
//     }
//     if(l==r){
//         uint32_t ans=0;
//         switch(tokens[l].type){
//             case NUM:
//                 sscanf(tokens[l].str,"%d",&ans);
//                 break;
//             case HEX:
//                 sscanf(tokens[l].str,"%x",&ans);
//                 break;
//             default:
//                 *success=false;
//         }
//         return ans;
//     }
//     else if(tokens[l].type=='('&& bracketsmatch[l]==r){
//         return eval(l+1,r-1,success);
//     }
//     else {
//         int op=l,curpri=0;
//         for(int j=r-1;j>l;j--){
//             int pri=getprio(l,r,j);
//             if(pri>curpri){
//                 curpri=pri;
//                 op=j;
//             }
//         }
//         if(op!=l){
//             uint32_t ansl=eval(l,op-1,success);
//             if(*success==false) return 0;
//             uint32_t ansr=eval(op+1,r,success);
//             if(*success==false) return 0;
//             switch(tokens[op].type){
//                 case LOR:
//                     return ansl || ansr;
//                 case LAND:
//                     return ansl && ansr;
//                 case '|':
//                     return ansl|ansr;
//                 case '^':
//                     return ansl^ansr;
//                 case '&':
//                     return ansl&ansr;
//                 case EQ:
//                     return ansl==ansr;
//                 case NEQ:
//                     return ansl!=ansr;
//                 case '>':
//                     return ansl>ansr;
//                 case '<':
//                     return ansl<ansr;
//                 case LE:
//                     return ansl<=ansr;
//                 case GE:
//                     return ansl>=ansr;
//                 case SHL:
//                     return ansl<<ansr;
//                 case SHR:
//                     return ansl>>ansr;
//                 case '+':
//                     return ansl+ansr;
//                 case '-':
//                     return ansl-ansr;
//                 case '*':
//                     return ansl*ansr;
//                 case '/':
//                     if(ansr==0){
//                         *success=false;
//                         return 0;
//                     }
//                     return ansl/ansr;
//                 case '%':
//                      if(ansr==0){
//                         printf("Divided by 0\n");
//                         *success=false;
//                         return 0;
//                     }
//                     return ansl%ansr;
//             }
//         }
//         else{
//             uint32_t ans;
//             switch(tokens[op].type){
//                 case '-':
//                     return -eval(l+1,r,success);
//                 case '*':
//                     ans=eval(l+1,r,success);
//                     if(*success==false) return 0;
//                     return vaddr_read((vaddr_t)ans,0,4);
//                 case '!':
//                     return !eval(l+1,r,success);
//             }
//         }
//     }
//     return 0;
// }
uint32_t look_up_fun_symtab(char *, bool *);
uint32_t eval_symb(char *c,bool *success){
    return look_up_fun_symtab(c,success);
}
static int      stack1[TOKEN_SIZE],top1;
static int      stack2[TOKEN_SIZE],top2;
static uint32_t stack3[TOKEN_SIZE],top3;
uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
	top1=top2=0;
	for(int i=0;i<nr_token;i++) {
	    // 和 -(减,NEG)
	    if(tokens[i].type=='-'){
	       // printf("%d\n",gettype(tokens + (i-1)));
	        if(i==0||gettype(tokens + (i-1))==OPRATOR_1OP||gettype(tokens + (i-1))==OPRATOR_2OP||tokens[(i-1)].type==')')
	            tokens[i].type=NEG;
	    }
	    // 区分*(乘,MEM)
	    if(tokens[i].type=='*'){
	       // printf("%d\n",gettype(tokens + (i-1)));
	        if(i==0||gettype(tokens + (i-1))==OPRATOR_1OP||gettype(tokens + (i-1))==OPRATOR_2OP||tokens[(i-1)].type==')')
	            tokens[i].type=MEM;
	    }
	   // printf("%s %d %d\n",tokens[i].str,tokens[i].type,NEG);
	    if(tokens[i].type=='('){
	        stack1[top1++]=i;
	    }
	    else if(tokens[i].type==')'){
	        while(top1 && tokens[ stack1[top1-1] ].type!='(')
	            stack2[top2++]=stack1[--top1];
	        if(top1>0){
	            top1--;
	        }
	        else{
	            puts("( unmathed!!!!!!!");
	            *success=false;
	            return 0;
	        }
	    }
	    else if(gettype(tokens+i)==OPRATOR_1OP||gettype(tokens+i)==OPRATOR_2OP){
	        while(top1 && tokens[ stack1[top1-1] ].type!='(' && getprio(tokens+stack1[top1-1])<=getprio(tokens+i))
	            stack2[top2++]=stack1[--top1];
	        stack1[top1++]=i;
	    }
	    else if(gettype(tokens+i)==OPR_VAL){
	        stack2[top2++]=i;
	    }
	    else{
	        puts("Invaild Expressions!");
	        *success=false;
	        return 0;
	    }
	}
	int checkbrackets=0;
	while(top1){
	    if(tokens[top1-1].type=='(')
	        checkbrackets++;
	   else
	       stack2[top2++]=stack1[top1-1];
	   top1--;
	}
	top3=0;
	for(int i=0;i<top2;i++){
	    if(gettype(tokens + stack2[i] )==OPRATOR_2OP){
	           if(top3<2){
	               *success=false;
	               return 0;
	           }
	           uint32_t ansl=stack3[top3-2],ansr=stack3[top3-1];
	           switch(tokens[ stack2[i] ].type){
                case LOR:
                    stack3[top3-2]=ansl || ansr;
                    break;
                case LAND:
                    stack3[top3-2]=ansl && ansr;
                    break;
                case '|':
                    stack3[top3-2]=ansl|ansr;
                    break;
                case '^':
                    stack3[top3-2]=ansl^ansr;
                    break;
                case '&':
                    stack3[top3-2]=ansl&ansr;
                    break;
                case EQ:
                    stack3[top3-2]=ansl==ansr;
                    break;
                case NEQ:
                    stack3[top3-2]=ansl!=ansr;
                    break;
                case '>':
                    stack3[top3-2]=ansl>ansr;
                    break;
                case '<':
                    stack3[top3-2]=ansl<ansr;
                    break;
                case LE:
                    stack3[top3-2]=ansl<=ansr;
                    break;
                case GE:
                    stack3[top3-2]=ansl>=ansr;
                    break;
                case SHL:
                    stack3[top3-2]=ansl<<ansr;
                    break;
                case SHR:
                    stack3[top3-2]=ansl>>ansr;
                    break;
                case '+':
                    stack3[top3-2]=ansl+ansr;
                    break;
                case '-':
                    stack3[top3-2]=ansl-ansr;
                    break;
                case '*':
                    stack3[top3-2]=ansl*ansr;
                    break;
                case '/':
                    if(ansr==0){
                        printf("Divided by 0\n");
                        *success=false;
                        return 0;
                    }
                    stack3[top3-2]=ansl/ansr;
                    break;
                case '%':
                     if(ansr==0){
                        printf("Divided by 0\n");
                        *success=false;
                        return 0;
                    }
                    stack3[top3-2]=ansl%ansr;
                    break;
	           }
	           top3--;
	    }
	    else if(gettype(tokens + stack2[i] )==OPRATOR_1OP){
	        if(top3<1){
	            *success=false;
	            return 0;
	        }
	        switch(tokens[ stack2[i] ].type){
                case NEG:
                    stack3[top3-1]=-stack3[top3-1];
                    break;
                case MEM:
                    stack3[top3-1]=vaddr_read((vaddr_t)stack3[top3-1],0,4);
                    break;
                case '!':
                    stack3[top3-1]=!stack3[top3-1];
                     break;
                case '~':
                    stack3[top3-1]=~stack3[top3-1];
                     break;
            }
	    }
	    else if(gettype(tokens + stack2[i] )==OPR_VAL){
	        switch(tokens[ stack2[i] ].type){
                case NUM:
                    sscanf(tokens[ stack2[i] ].str,"%u",&stack3[top3]);
                    break;
                case HEX:
                    sscanf(tokens[ stack2[i] ].str,"%x",&stack3[top3]);
                    break;
                case REG:
                    break;
                case SYMB:
                    stack3[top3]=eval_symb(tokens[ stack2[i]].str,success);
                    break;
            }
            top3++;
	    }
	}
	if(top3!=1) *success=false;
	return stack3[0];
}
#undef TOKEN_LEN
#undef TOKEN_SIZE
