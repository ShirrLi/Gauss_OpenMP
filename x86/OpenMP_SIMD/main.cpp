#include<iostream>
#include <windows.h>
#include<omp.h>
#include <stdlib.h>
#include<semaphore.h>
#include<xmmintrin.h>
#include<emmintrin.h>
#include<immintrin.h>
#include<ammintrin.h>
using namespace std;
float A[2000][2000],B[2000][2000],C[2000][2000],D[2000][2000];
int n;
const int NUM_THREADS=7;
void Initialize(int n)//初始化
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			//下三角元素初始化为零
			B[i][j]=0;
			D[i][j]=0;
		}
		//对角线元素初始化为1
		B[i][i]=1.0;
		D[i][i]=1.0;
		for(j=i+1;j<n;j++){
			//上三角元素初始化为随机数
			B[i][j]=rand();
			D[i][j]=B[i][j];
		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				//最终每一行的值是上一行的值与这一行的值之和
				B[i][j]+=B[k][j];
				D[i][j]+=D[k][j];
			}
}
void OpenMP_SSE(){
    int i,j,k;
    float tmp[4];
    __m128 t1,t2,t3,t4; //定义4个向量寄存器
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,t4)
    for(k=0;k<n;k++)
    {
         #pragma omp single
        {
        tmp[0]=B[k][k];tmp[1]=B[k][k];tmp[2]=B[k][k];tmp[3]=B[k][k];
        t1=_mm_loadu_ps(tmp); //加载到t1向量寄存器
        for(j=k+1;j+4<=n;j+=4)
        {
            t2=_mm_loadu_ps(B[k]+j); //把内存中从B[k][j]开始的四个单精度浮点数加载到t2寄存器
            t3=_mm_div_ps(t2,t1); //相除结果放到t3寄存器
            _mm_storeu_ps(B[k]+j,t3); //把t3寄存器的值放回内存
        }
        for(j;j<n;j++) //处理剩下的不能被4整除的
            B[k][j]/=B[k][k];
        B[k][k]=1.0;
        //以上完成了对第一个部分的向量化
        }
        #pragma omp for schedule(static,1)
        for(i=k+1;i<n;i++)
        {
            tmp[0]=B[i][k];tmp[1]=B[i][k];tmp[2]=B[i][k];tmp[3]=B[i][k];
            t1=_mm_loadu_ps(tmp);
            for(j=k+1;j+4<=n;j+=4)
            {
                t2=_mm_loadu_ps(B[k]+j);
                t3=_mm_loadu_ps(B[i]+j);
                t4=_mm_mul_ps(t1,t2);
                t3=_mm_sub_ps(t3,t4);
                _mm_storeu_ps(B[i]+j,t3);
            }
            for(j=j;j<n;j++)
                B[i][j]-=B[i][k]*B[k][j];
            B[i][k]=0;
        }
    }
}
void OpenMP_AVX(){
    int i,j,k;
    float tmp[8];
    __m256_u t1,t2,t3,t4; //定义4个向量寄存器
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,t4)
    for(k=0;k<n;k++){
        #pragma omp single
        {
            tmp[0]=D[k][k];tmp[1]=D[k][k];tmp[2]=D[k][k];tmp[3]=D[k][k];tmp[4]=D[k][k];tmp[5]=D[k][k];tmp[6]=D[k][k];tmp[7]=D[k][k];
            t1=_mm256_loadu_ps(tmp); //加载到t1向量寄存器
            for(j=k+1;j+8<=n;j+=8)
            {
                t2=_mm256_loadu_ps(D[k]+j); //把内存中从B[k][j]开始的四个单精度浮点数加载到t2寄存器
                t3=_mm256_div_ps(t2,t1); //相除结果放到t3寄存器
                _mm256_storeu_ps(D[k]+j,t3); //把t3寄存器的值放回内存
            }
            for(j;j<n;j++) //处理剩下的不能被4整除的
                D[k][j]/=D[k][k];
            D[k][k]=1.0;
            //以上完成了对第一个部分的向量化
        }
        #pragma omp for schedule(static,1)
        for(i=k+1;i<n;i++){
            tmp[0]=D[i][k];tmp[1]=D[i][k];tmp[2]=D[i][k];tmp[3]=D[i][k];tmp[4]=D[i][k];tmp[5]=D[i][k];tmp[6]=D[i][k];tmp[7]=D[i][k];
            t1=_mm256_loadu_ps(tmp);
            for(j=k+1;j+8<=n;j+=8)
            {
                t2=_mm256_loadu_ps(D[k]+j);
                t3=_mm256_loadu_ps(D[i]+j);
                t4=_mm256_mul_ps(t1,t2);
                t3=_mm256_sub_ps(t3,t4);
                _mm256_storeu_ps(D[i]+j,t3);
            }
            for(j=j;j<n;j++)
                D[i][j]-=D[i][k]*D[k][j];
            D[i][k]=0;
        }
    }
}
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
	Initialize(2000);
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

		QueryPerformanceFrequency(&tc1);
		QueryPerformanceCounter(&t1);
		while(count<=cycle){
			OpenMP_SSE();
            count++;}
        QueryPerformanceCounter(&t2);
        cout<<n<<" "<<count-1<<" "<<((t2.QuadPart - t1.QuadPart)*1000.0 / tc1.QuadPart);


        count=1;
		QueryPerformanceFrequency(&tc2);
		QueryPerformanceCounter(&t3);
		while(count<=cycle){
			OpenMP_AVX();
            count++;}
        QueryPerformanceCounter(&t4);
        cout<<" "<<((t4.QuadPart - t3.QuadPart)*1000.0 / tc2.QuadPart)<<endl;
		if(n==100)
            step=100;
        if(n==1000)
            step=500;

	}
}

