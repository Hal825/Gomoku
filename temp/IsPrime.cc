#include <vector>
#include <iostream>

bool IsPrime(int n)
{
    if(n<=1) return false;
    if(n<=3) return true;
    if(n%2 == 0||n%3==0) return false;
    for(int i = 5;i*i<n;i+=6)
    {
        if(n%i==0||n%(i+2)==0)
        {
            return false;
        }
    }
    return true;
}

long long fastPowerRecursive(long long a, long long b, long long mod)
{
    if(b==0) return 1%mod;

    long long half = fastPowerRecursive(a, b/2, mod);
    long long result = (half*half)%mod;

    if(b%2==1){
        result = (result*a) % mod;
    }
    return result;
}

long long fastPower(long long a, long long b, long long mod) 
{
    long long result = 1;
    a%=mod;
    while(b>0){
        if(b&1){
            result = (result*a)%mod;
        }
        a = (a*a)%mod;
        b>=1;
    }
    return result;
}