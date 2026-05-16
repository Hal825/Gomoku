#include <bits/stdc++.h>
using namespace std;

int quick_pow(int a,int b)//a^b
{
    int res = 1;
    while(b)
    {
        if(b&1) res = res*a;
        a = a*a;
        b>>=1;
    }
    return res;
}

int main()
{
    int n;
    cin>>n;
    n = quick_pow(2,n);
    vector<int>county(n);
    for(int i = 0;i<n;i++)
    {
        cin>>county[i];
    }

    return 0;
}