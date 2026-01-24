#include <bits/stdc++.h>
using namespace std;

int main()
{
    // char str[100] = "Hello World";
    char dest[100] = "hello!";
    // 常用函数
    // cout << strlen(str) << '\n'; // 字符串长度（不含\0）
    // strcpy(dest, str);  // 复制字符串
    // cout<<dest<<'\n';
    // strcat(dest, str);  // 拼接字符串
    // cout<<dest<<'\n';
    // cout<<strcmp(str,dest)<<'\n';     // 比较字符串

    // cout<<str<<endl<<strchr(str, 'd')<<endl;   // 查找字符首次出现
    // cout<<strstr(str, "World")<<endl;   // 查找子串
    // dest = strtok(str, " "); // 字符串分割
    char str[] = "apple,banana,orange";
    char *token;

    token = strtok(str, ","); // 第一次调用
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, ","); // 后续调用
    }
    vector<pair<int,int>>vp;
    sort(vp.begin(),vp.end(),greater<int>());

    vector<string> split(string s,char delim)
    {
        vector<string> tokens;
        istirngstream iss(s);
        stirng token;
        while(getline(iss,token,delim))
        {
            res.push_backtoke
        }
    }
    return 0;
}