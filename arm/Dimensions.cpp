#include<iostream>
#include <omp.h>
#include <sys/time.h>
using namespace std;
float A[2000][2000],B[2000][2000],C[2000][2000];
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
			C[i][j]=0;

		}
		A[i][i]=1.0;//对角线元素初始化为1
		B[i][i]=1.0;
		C[i][i]=1.0;

		for(j=i+1;j<n;j++){
			A[i][j]=rand();//上三角元素初始化为随机数
			B[i][j]=A[i][j];
			C[i][j]=A[i][j];

		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				A[i][j]+=A[k][j];//最终每一行的值是上一行的值与这一行的值之和
				B[i][j]+=B[k][j];
				C[i][j]+=C[k][j];


			}
}
void Guass_row(){
    int i,j,k;
    float tmp;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp)
    for(k=0;k<n;k++){
        #pragma omp single
        {
            tmp=A[k][k];
            for(j=k+1;j<n;j++)
                A[k][j]/=tmp;
            A[k][k]=1;
        }
        #pragma omp for
		for(i=k+1;i<n;i++){
			tmp=A[i][k];
			for(j=k+1;j<n;j++)
				A[i][j]-=tmp*A[k][j];
			A[i][k]=0;
		}
    }
}
void Guass_column(){
    int i,j,k;
    float tmp;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k)
    for(k=0;k<n;k++){
        tmp=C[k][k];
        #pragma omp for
        for(j=k+1;j<n;j++)
            C[k][j]/=tmp;
        C[k][k]=1;
		for(i=k+1;i<n;i++){
			tmp=C[i][k];
			#pragma omp for
			for(j=k+1;j<n;j++)
				C[i][j]-=tmp*C[k][j];
			C[i][k]=0;
		}
    }
}
int main(){
	int N=2000,cycle=1,step=10,count=1;
	struct timeval beg1,end1,beg2,end2;
    Initialize(N);
cout<<"threads"<<NUM_THREADS<<endl;
    for(n=10;n<=N;n+=step){
        count=1;
        if(n<=20)
            cycle=1000;
        else
            if(n<=60)
                cycle=100;
            else
                if(n<=200)
                    cycle=10;
                    else
                        cycle=1;
         gettimeofday(&beg1, NULL);
while(count<=cycle){
            Guass_row();
            count++;}
       gettimeofday(&end1, NULL);
       cout <<n<<" "<<count-1<<" "<< (long long)1000000*end1.tv_sec+(long long)end1.tv_usec- (long long)1000000*beg1.tv_sec-(long long)beg1.tv_usec;
       count=1;
gettimeofday(&beg2, NULL);
while(count<=cycle){
            Guass_column();
            count++;}
       gettimeofday(&end2, NULL);
 cout <<" "<< (long long)1000000*end2.tv_sec+(long long)end2.tv_usec- (long long)1000000*beg2.tv_sec-(long long)beg2.tv_usec<<endl;
if(n==100)
            step=100;
        if(n==1000)
            step=500;
	}
}
