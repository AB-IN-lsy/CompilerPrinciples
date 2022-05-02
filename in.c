#include <stdio.h>

int gcd(int a, int b) //这是一条注释
{
    if (b)
    {
        return gcd(b, a % b);
    }
    else
    {
        return a;
    }
}
int main()
{
    int x, y;
    x = 6;
    y = x + 2;
    for (int i = 1; i <= 5; i++)
    {
        printf("x:%d y:%d gcd(x, y):%d\n", x, y, gcd(x, y));
        x = y;
        y = x + 2;
    }
    return 0;
}
