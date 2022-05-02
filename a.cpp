/*
 * @Author: NEFU AB-IN
 * @Date: 2022-05-01 21:51:05
 * @FilePath: \CompilerPrinciples\a.cpp
 * @LastEditTime: 2022-05-02 09:13:11
 */
#include <bits/stdc++.h>
using namespace std;

const int SIZE = 4096; //默认大小

// C语言关键字
string keywords[35] = {"auto",     "break",  "case",     "char",   "const",    "continue", "default",
                       "do",       "double", "else",     "enum",   "extern",   "float",    "for",
                       "goto",     "if",     "int",      "long",   "register", "return",   "short",
                       "signed",   "sizeof", "static",   "struct", "switch",   "typedef",  "union",
                       "unsigned", "void",   "volatile", "while"}; //已经按字典序排好，直接二分查找

//标记符
enum
{
    Num = 128,
    Fun,
    Sys,
    Glo,
    Loc,
    Id,
    Char,
    Else,
    Enum,
    If,
    Int,
    Return,
    Sizeof,
    While,
    Assign,
    Cond,
    Lor,
    Lan,
    Or,
    Xor,
    And,
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    Shl,
    Shr,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Inc,
    Dec,
    Brak,
    Var,
    Separator,
    Keyw,
    Other
};

typedef struct node
{
    int key;  //
    int type; //种别码
    char value[12];
    string cn_type;    // 中文种别码
    struct node *next; //链表
} node;

int token;              // 当前标记
char *pos;              // 指向源代码字符串指针
int poolsize;           // 默认文本/数据大小
int line;               // 源码行号
FILE *infile, *outfile; //读入文件.c,输出文件.txt
struct node *head;      //链表头

struct node *get_next(struct node *p) //用于词法分析，获取下一个标记，它将自动忽略空白字符
{
    int key = 0, type, i, j;
    char value[12];
    string cn_type;
    memset(value, 0, sizeof(value));
    while (1)
    {
        token = *pos;
        pos++;
        if (token == '\n') //换行符号，line值+1
        {
            ++line;
        }
        else if (token == ' ') //忽略空格
        {
            if (*pos == ' ') //指针指向下一个字符
            {
                pos++;
            }
        }
        else if (token == '#') //处理宏定义，不支持#include<stdio.h>
        {
            while (*pos != 0 && *pos != '\n')
            {
                pos++;
            }
        }
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') ||
                 (token == '_')) // 解析标识符,从32个标识符中查找
        {
            i = 0;
            value[i] = token;
            while ((*pos >= 'a' && *pos <= 'z') || (*pos >= 'A' && *pos <= 'Z') || (*pos >= '0' && *pos <= '9') ||
                   (*pos == '_'))
            {
                value[i + 1] = *pos;
                pos++;
                i++;
            }
            value[i + 1] = '\0'; // 查询已有的标识符
            int l = 0, r = 31;   //二分查找现存在的标识符
            while (l <= r)
            {
                int mid = (l + r) >> 1;
                if (value == keywords[mid]) //查找完成
                {
                    type = Var;
                    key = mid + 1;
                    cn_type = "关键字";
                    break;
                }
                else if (value < keywords[mid])
                    r = mid - 1;
                else
                    l = mid + 1;
            }
            if (type != Keyw)
            {
                key = 39;
                cn_type = "标识符";
                type = Var;
            }
            break;
        }
        else if (token >= '0' && token <= '9') // 识别数字
        {
            int num = token - '0';
            value[0] = token;
            if (num > 0)
            {
                i = 1;
                while ((*pos >= '0' && *pos <= '9'))
                {
                    value[i] = *pos;
                    num = num * 10 + (*pos - '0');
                    *pos++;
                }
            }
            else
            {
                num = '0';
            }
            key = 40;
            type = Num;
            cn_type = "数字";
            value[i] = '\0';
            break;
        }
        else if (token == '/') // 注释
        {
            if (*pos == '/')
            {
                value[0] = '/';
                value[1] = '/';
                cn_type = "注释";
                while (*pos != 0 && *pos != '\n') //识别出注释的话，直接把后面的当前行忽略了,两个直接相连
                {
                    pos++;
                }
                break;
            }
            else // 没有两个直接相连，就是除号
            {
                key = 44;
                type = Div;
                cn_type = "除法运算符";
                value[0] = token;
                break;
            }
        }
        else if (token == '+')
        {
            if (*pos == '+') //识别自增
            {
                pos++;
                key = 45;
                type = Inc;
                cn_type = "自增";
                value[0] = value[1] = '+';
                break;
            }
            else //单纯的加法
            {
                key = 41;
                type = Add;
                cn_type = "加法运算符";
                value[0] = '+';
                break;
            }
        }
        else if (token == '-')
        {
            if (*pos == '-') //识别自减
            {
                pos++;
                key = 46;
                type = Dec;
                cn_type = "自减运算符";
                value[0] = value[1] = '-';
                break;
            }
            else //减法
            {
                key = 42;
                type = Sub;
                cn_type = "减法运算符";
                value[0] = '-';
                break;
            }
        }
        else if (token == '*') //乘法
        {
            key = 43;
            type = Mul;
            cn_type = "乘法运算符";
            value[0] = '*';
            break;
        }
        else if (token == '=')
        {
            if (*pos == '=') //等于号
            {
                pos++;
                type = Eq;
                cn_type = "关系双界限符";
                value[0] = value[1] = '=';
                break;
            }
            else //赋值语句
            {
                key = 47;
                type = Assign;
                cn_type = "关系单界限符";
                value[0] = '=';
                break;
            }
        }
        else if (token == '!')
        {
            if (*pos == '=') //不等于号
            {
                pos++;
                key = 48;
                type = Ne;
                cn_type = "关系双界限符";
                value[0] = '!';
                value[1] = '=';
                break;
            }
        }
        else if (token == '<')
        {
            if (*pos == '=') //<=
            {
                pos++;
                key = 49;
                type = Le;
                cn_type = "关系双界限符";
                value[0] = '<';
                value[1] = '=';
                break;
            }
            else if (*pos == '<') //左移<<
            {
                pos++;
                key = 50;
                type = Shl;
                value[0] = value[1] = '<';
                cn_type = "移位运算符";
                break;
            }
            else //小于号
            {
                key = 51;
                type = Lt;
                value[0] = '<';
                cn_type = "关系单界限符";
                break;
            }
        }
        else if (token == '>')
        {
            if (*pos == '=') //大于等于>=
            {
                pos++;
                key = 52;
                type = Ge;
                value[0] = '>';
                value[1] = '=';
                cn_type = "关系双界限符";
                break;
            }
            else if (*pos == '>') //右移
            {
                pos++;
                key = 53;
                type = Shr;
                cn_type = "移位运算符";
                value[0] = value[1] = '>';
                break;
            }
            else //大于号
            {
                key = 54;
                type = Gt;
                value[0] = '>';
                cn_type = "关系单界限符";
                break;
            }
        }
        else if (token == '|')
        {
            if (*pos == '|') //或运算
            {
                pos++;
                key = 55;
                type = Lor;
                cn_type = "关系双界限符";
                value[0] = value[1] = '|';
                break;
            }
            else //按位或
            {
                key = 56;
                type = Or;
                cn_type = "或运算符";
                value[0] = '|';
                break;
            }
        }
        else if (token == '&')
        {
            if (*pos == '&') // and
            {
                pos++;
                key = 58;
                type = Lan;
                cn_type = "关系双界限符";
                value[0] = value[1] = '&';
                break;
            }
            else //按位与
            {
                key = 59;
                type = And;
                cn_type = "与运算符";
                value[0] = '&';
                break;
            }
        }
        else if (token == '^') //异或
        {
            key = 60;
            type = Xor;
            value[0] = '^';
            break;
        }
        else if (token == '%') //取模
        {
            key = 61;
            type = Mod;
            value[0] = '%';
            cn_type = "取模运算符";
            break;
        }
        else if (token == '[') //
        {
            key = 62;
            type = Brak;
            value[0] = '[';
            cn_type = "单界限符";
            break;
        }
        else if (token == '?')
        {
            key = 63;
            type = Cond;
            value[0] = '?';
            cn_type = "单界限符";
            break;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' ||
                 token == ']' || token == ',' || token == ':' || token == '"' || token == '\\')
        {
            key = 64;
            type = Separator;
            cn_type = "单界限符";
            value[0] = token;
            break;
        }
        else if (token == 0)
        {
            break;
        }
        else
        {
            printf("第%d行出现错误!\n", line);
            key = 65;
            type = Other;
            value[0] = token;
            break;
        }
    }
    p->key = key;
    strcpy(p->value, value);
    p->cn_type = cn_type;
    p->type = type;
    node *q = new node;
    q->next = NULL;
    p->next = q;
    return p;
}

void lexical(struct node *p, FILE *outfile) // lex分析器
{
    printf("\n分析成功！产生的二元组序列: \n");
    p = get_next(p); // 获取下一个标记
    for (int i = 1; token > 0; i++)
    {
        printf("< %s , %s >\n", p->cn_type.c_str(), p->value);
        fprintf(outfile, "< %s , %s >\n", p->cn_type.c_str(), p->value);
        p = p->next;
        get_next(p);
    }
}

bool read_text_c() //读取c程序
{
    int i;
    char filename[30];
    head = new node;
    head->next = NULL;
    printf("输入进行语法分析的C语言程序:");
    cin >> filename;
    infile = fopen(filename, "r");
    if (!infile)
    {
        cout << "打开程序文件失败!\n";
        return 0;
    }
    else
    {
        cout << "成功打开文件，开始分析\n";
        pos = (char *)malloc(SIZE);
        outfile = fopen("output.txt", "w");
        i = fread(pos, 1, SIZE, infile); //算有多少字符
        pos[i] = 0;                      // 添加末尾控制符EOF
        return 1;
    }
}

int main()
{
    if (read_text_c()) //读入正确,开始分析
    {
        fclose(infile);
        lexical(head, outfile);
        fclose(outfile);
    }
    return 0;
}
