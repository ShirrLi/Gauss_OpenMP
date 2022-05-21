#include<iostream>
#include <windows.h>
#include <omp.h>
using namespace std;
float A[2000][2000],B[2000][2000];
int NUM_THREADS=7;
void Initialize(int n)//��ʼ��
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			A[i][j]=0;//������Ԫ�س�ʼ��Ϊ��
			B[i][j]=0;

		}
		A[i][i]=1.0;//�Խ���Ԫ�س�ʼ��Ϊ1
		B[i][i]=1.0;

		for(j=i+1;j<n;j++){
			A[i][j]=rand();//������Ԫ�س�ʼ��Ϊ�����
			B[i][j]=A[i][j];

		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				A[i][j]+=A[k][j];//����ÿһ�е�ֵ����һ�е�ֵ����һ�е�ֵ֮��
				B[i][j]+=B[k][j];

			}
}
void Print(int n,float m[][2000]){//��ӡ���
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			cout<<m[i][j]<<" ";
		cout<<endl;
	}
}
int main(){

	int N=2000,cycle=1,step=1;
	LARGE_INTEGER t1,t2,tc1,t3,t4,tc2;
    cout<<"threads"<<NUM_THREADS<<endl;
    Initialize(N);
    for(int p=1;p<=N/NUM_THREADS;p+=step){
        //Initialize(N);
        QueryPerformanceFrequency(&tc1);
        QueryPerformanceCounter(&t1);
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
        QueryPerformanceCounter(&t2);
        cout<<p<<" "<<((t2.QuadPart - t1.QuadPart)*1000.0 / tc1.QuadPart)<<endl;
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

