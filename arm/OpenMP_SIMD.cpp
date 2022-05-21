#include<iostream>
#include<omp.h>
#include <sys/time.h>
#include<arm_neon.h>
using namespace std;
float D[2000][2000];
int n;
const int NUM_THREADS=7;
void Initialize(int n)//初始化
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			
			D[i][j]=0;
		}
		
		D[i][i]=1.0;
		for(j=i+1;j<n;j++){
			D[i][j]=rand();//上三角元素初始化为随机数
			
		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				
				D[i][j]+=D[k][j];
			}
}
void OpenMP_Neon()
{
    int i,j,k;
	float32_t tmp[4];
    float32x4_t t1,t2,t3,t4; //定义4个向量寄存器
	#pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,t4)
    for(k=0;k<n;k++)
    {
		#pragma omp single
        {
		tmp[0]=D[k][k];tmp[1]=D[k][k];tmp[2]=D[k][k];tmp[3]=D[k][k];
        t1=vld1q_f32(tmp); //加载到t1向量寄存器
        for(j=k+1;j+4<=n;j+=4)
        {
            t2=vld1q_f32(D[k]+j); //把内存中从B[k][j]开始的四个单精度浮点数加载到t2寄存器
            t3=vdivq_f32(t2,t1); //相除结果放到t3寄存器
            vst1q_f32(D[k]+j,t3); //把t3寄存器的值放回内存
        }
        for(j;j<n;j++) //处理剩下的不能被4整除的
            D[k][j]/=D[k][k];
        D[k][k]=1.0;
        //以上完成了对第一个部分的向量化
		}
		#pragma omp for schedule(static,1)
        for(i=k+1;i<n;i++)
        {
            tmp[0]=D[i][k];tmp[1]=D[i][k];tmp[2]=D[i][k];tmp[3]=D[i][k];
            t1=vld1q_f32(tmp);
            for(j=k+1;j+4<=n;j+=4)
            {
                t2=vld1q_f32(D[k]+j);
                t3=vld1q_f32(D[i]+j);
                t4=vmulq_f32(t1,t2);
                t3=vsubq_f32(t3,t4);
                vst1q_f32(D[i]+j,t3);
            }
            for(j=j;j<n;j++)
                D[i][j]-=D[i][k]*D[k][j];
            D[i][k]=0;
        }
    }
}
int main(){
	int N=2000,count,cycle=1,step=10;
	struct timeval beg2,end2;
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

		gettimeofday(&beg2, NULL);
		while(count<=cycle){
			OpenMP_Neon();
            count++;}
        gettimeofday(&end2, NULL);
		cout<<n<<" "<<count-1<<" "<< (long long)1000000*end2.tv_sec+(long long)end2.tv_usec- (long long)1000000*beg2.tv_sec-(long long)beg2.tv_usec<<endl;
    
		if(n==100)
            step=100;
        if(n==1000)
            step=500;

	}
}
