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
void Gauss_normal(){//串行算法
	int i,j,k;
	for(k=0;k<n;k++)
    {
		float tmp=A[k][k];
		for(j=k+1;j<n;j++)
			A[k][j]/=tmp;
		A[k][k]=1;
		for(i=k+1;i<n;i++)
		{
			float tmp2=A[i][k];
			for(j=k+1;j<n;j++)
				A[i][j]-=tmp2*A[k][j];
			A[i][k]=0;
		}
	}
}
void Guass_static(){
    int i,j,k;
    float tmp;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp)
    for(k=0;k<n;k++){
        #pragma omp single
        {
            tmp=B[k][k];
            for(j=k+1;j<n;j++)
                B[k][j]/=tmp;
B[k][k]=1;
        }
        #pragma omp for
		for(i=k+1;i<n;i++){
			tmp=B[i][k];
			for(j=k+1;j<n;j++)
				B[i][j]-=tmp*B[k][j];
			B[i][k]=0;
		}
    }
}
int main(){

	int N=2000,count,cycle=1,step=10;
	struct timeval beg1,end1,beg2,end2;
	Initialize(N);
	for(NUM_THREADS=1;NUM_THREADS<=7;NUM_THREADS++){
        cout<<"threads"<<NUM_THREADS<<endl;
        step=10;
        for(n=10;n<=N;n+=step){
            count=1;
            if(n<=50)
                cycle=1000;
            else
                if(n<=70)
                    cycle=100;
                else
                    if(n<=400)
                        cycle=10;
                        else
                            cycle=1;

            gettimeofday(&beg1, NULL);
            while(count<=cycle){
                Gauss_normal();
                count++;}
            gettimeofday(&end1, NULL);
            cout <<n<<" "<<count-1<<" "<< (long long)1000000*end1.tv_sec+(long long)end1.tv_usec- (long long)1000000*beg1.tv_sec-(long long)beg1.tv_usec;

            count=1;
            gettimeofday(&beg2, NULL);
            while(count<=cycle){
                Guass_static();
                count++;}
            gettimeofday(&end2, NULL);
            cout <<" "<< (long long)1000000*end2.tv_sec+(long long)end2.tv_usec- (long long)1000000*beg2.tv_sec-(long long)beg2.tv_usec<<endl;

            if(n==100)
                step=100;
            if(n==1000)
                step=500;

        }
	}
}
