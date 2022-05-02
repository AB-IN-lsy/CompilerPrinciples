'''
Author: NEFU AB-IN
Date: 2022-05-02 15:32:14
FilePath: \CompilerPrinciples\b.py
LastEditTime: 2022-05-02 19:23:22
'''
from prettytable import PrettyTable

# 构造表头
table = PrettyTable(["步数", "栈", "输入串顶", "剩余输入串", "输出"])

# 构造预测分析表
FAT = {
    ('E', 'i'): 'TX',
    ('E', '('): 'TX',
    ('X', '+'): '+TX',
    ('X', ')'): 'ε',
    ('X', '#'): 'ε',
    ('T', 'i'): 'FY',
    ('T', '('): 'FY',
    ('Y', '+'): 'ε',
    ('Y', '*'): '*FY',
    ('Y', ')'): 'ε',
    ('Y', '#'): 'ε',
    ('F', 'i'): 'i',
    ('F', '('): '(E)',
}

# 文法开始符
start = 'E'

# 非终结符集合
V_n = ['E', 'X', 'T', 'Y', 'F']

# 终结符集合
V_t = ['i', '+', '*', '(', ')']


def PrintStack(stk):  # return 倒序的stack字符串
    s = "".join(stk)
    return s[::-1]


def PrintStr(s, loc):  # return 待分析串
    return s[loc:]


def Error():
    print("分析失败")
    exit(0)


def Analysis(s):
    stack = []  # 栈
    loc = 0  # 代表待分析串s的下标
    flag = True  # 代表是否能继续分析
    cnt = 1  # 步数统计
    s = s + '#'  # 给待分析串末尾打上结束符
    res = ""  # 输出
    stack.append('#')  # 先压入#
    stack.append(start)  # 压入文法开始符号

    while flag:
        s_top = s[loc]  # 目前待分析串的顶
        stk_top = stack[-1]  # 栈顶

        # 首先，根据栈的情况添加这一行
        table.add_row([cnt, PrintStack(stack), s_top, PrintStr(s, loc), res])
        # 判断符号是否符合要求
        if (stk_top not in V_t) and (stk_top not in V_n) and (stk_top != '#'):
            Error()
        if stk_top in V_n:  # 如果栈顶是非终结符，才会有输出
            if (stk_top, s_top) in FAT.keys():  # 如果能构造
                res = stk_top + '->' + FAT[(stk_top, s_top)]  # 构造结果
            else:
                Error()
        if stk_top in V_t:
            res = ""  # 如果栈顶是终结符，无输出

        # 其次，开始进行弹栈分析
        stack.pop()
        if stk_top in V_n:  # 如果是非终结符
            if (stk_top, s_top) in FAT.keys():  # 如果查表查到了
                ans = FAT[(stk_top, s_top)]
                ans = list(filter(lambda x: x != 'ε',
                                  ans))[::-1]  # 将产生式右部转为list，过滤掉ε，并逆序
                stack = stack + ans  # 压入栈
            else:
                Error()
        elif stk_top in V_t:  # 如果是终结符
            if stk_top == s_top:  # 如果 待分析串顶 和 栈顶 匹配
                loc += 1
                s_top = s[loc]
            else:
                Error()
        elif stk_top == '#':  # 如果是结束符
            if stk_top == s_top:
                flag = False
                print("分析成功!")
            else:
                Error()
        else:
            Error()
        cnt += 1


def PrintTable():
    table.align["栈"] = 'r'
    table.align["剩余输入串"] = 'r'
    table.align["输出"] = 'l'
    print(table)


if __name__ == '__main__':
    s = input("请输入分析串(分析串中请不要加空格): ")
    Analysis(s)
    PrintTable()