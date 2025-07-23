#include<iostream>
#include<vector>
using namespace std;
int main(){
    int n,k;
    cin>>n;
    vector<int> a;
    for(int i=0;i<n;i++){
        cin>>k;
        a.push_back(k);
    }
    long long sum1=0,sum2=0;
    if(n%2==0){
        for(int i=0;i<n;i++){
            if(sum1>sum2){
                sum1=sum1-sum2;
                sum2=0;
            }
            else{
                sum2=sum2-sum1;
                sum1=0;
            }
            if(i%2==0)sum1+=a[i];
            else sum2+=a[i];
        }
        if(sum1!=sum2)cout<<"player1 wins";
        else cout<<"draw";
    }
    else{
        if(a.front()>a.back()){
            long long sumd;
            sumd=a.front();
            a.erase(a.begin());
            for(int i=0;i<n-1;i++){
                if(i%2==0)sum1+=a[i];
                else sum2+=a[i];
            }
            if(sum1>sum2+sumd || sum2>sum1+sumd)cout<<"player2 wins";
            else if(sum1<sum2+sumd || sum2<sum1+sumd)cout<<"player1 wins";
            else cout<<"draw";
        }
        else{
            long long sumd;
            sumd=a.back();
            a.pop_back();
            for(int i=0;i<n-1;i++){
                if(i%2==0)sum1+=a[i];
                else sum2+=a[i];
            }
            if(sum1>sum2+sumd || sum2>sum1+sumd)cout<<"player2 wins";
            else if(sum1<sum2+sumd || sum2<sum1+sumd)cout<<"player1 wins";
            else cout<<"draw";
        }

    }
}