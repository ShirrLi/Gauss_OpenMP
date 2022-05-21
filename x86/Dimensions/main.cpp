#include<iostream>
#include <windows.h>
#include <omp.h>
#include <stdlib.h>
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
void Guass_normal(){
    int i,j,k;
    float tmp;
    for(k=0;k<n;k++){
            tmp=A[k][k];
            for(j=k+1;j<n;j++)
                A[k][j]/=tmp;
            A[k][k]=1;
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
/*void Guass_both(){  无法运行：任务嵌套不被允许！
    int i,j,k;
    float tmp;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp)
    for(k=0;k<n;k++){
            tmp=B[k][k];
            #pragma omp for
            for(j=k+1;j<n;j++)
                B[k][j]/=tmp;
            B[k][k]=1;
        #pragma omp for
		for(i=k+1;i<n;i++){
			tmp=B[i][k];
            #pragma omp for
			for(j=k+1;j<n;j++)
				B[i][j]-=tmp*B[k][j];
			B[i][k]=0;
		}
    }
}*/
void Print(int n,float m[][2000]){//打印结果
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			cout<<m[i][j]<<" ";
		cout<<endl;
	}
}
int main(){

	int N=2000,count,cycle=1,step=10;
	LARGE_INTEGER t1,t2,tc1,t3,t4,tc2;
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
        QueryPerformanceFrequency(&tc1);
        QueryPerformanceCounter(&t1);
        while(count<=cycle){
            Guass_row();
            count++;}
        QueryPerformanceCounter(&t2);
        cout<<n<<" "<<count-1<<" "<<((t2.QuadPart - t1.QuadPart)*1000.0 / tc1.QuadPart);
        count=1;
        QueryPerformanceFrequency(&tc2);
        QueryPerformanceCounter(&t3);
        while(count<=cycle){
            Guass_column();
            count++;}
        QueryPerformanceCounter(&t4);
        cout<<" "<<((t4.QuadPart - t3.QuadPart)*1000.0 / tc2.QuadPart)<<endl;
        if(n==100)
            step=100;
        if(n==1000)
            step=500;
    }

}
