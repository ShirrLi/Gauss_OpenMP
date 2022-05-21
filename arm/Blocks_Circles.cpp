#include<iostream>
#include <omp.h>
#include <sys/time.h>
using namespace std;
float A[2000][2000],B[2000][2000];
int n;
int NUM_THREADS=7;
void Initialize(int n)//初始化
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			A[i][j]=0;//下三角元素初始化为零
			B[i][j]=0;

		}
		A[i][i]=1.0;//对角线元素初始化为1
		B[i][i]=1.0;

		for(j=i+1;j<n;j++){
			A[i][j]=rand();//上三角元素初始化为随机数
			B[i][j]=A[i][j];

		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				A[i][j]+=A[k][j];//最终每一行的值是上一行的值与这一行的值之和
				B[i][j]+=B[k][j];

			}
}
int main(){
	int N=2000,cycle=1,step=1;
	struct timeval beg1,end1;
    cout<<"threads"<<NUM_THREADS<<endl;
    //Initialize(N);
    for(int p=1;p<=N/NUM_THREADS;p+=step){
        Initialize(N);
         gettimeofday(&beg1, NULL);
        int i,j,k;
        float tmp;
        #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp)
        for(k=0;k<N;k++){
            #pragma omp single
            {
                tmp=A[k][k];
                for(j=k;j<N;j++)
                    A[k][j]/=tmp;
            }
            #pragma omp for schedule(static,p)
            for(i=k+1;i<N;i++){
                tmp=A[i][k];
                for(j=k+1;j<N;j++)
                    A[i][j]-=tmp*A[k][j];
                A[i][k]=0;
            }
        }
       gettimeofday(&end1, NULL);
       cout <<p<<" "<< (long long)1000000*end1.tv_sec+(long long)end1.tv_usec- (long long)1000000*beg1.tv_sec-(long long)beg1.tv_usec<<endl;
        if(p==10)
            step=10;
        if(p==100)
            step=50;
        if(p==250)
            step=10;
        if(p==280)
            step=1;
	}
}
