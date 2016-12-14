#include "include.h"
#include<pthread.h>

pthread_t * thread_handles;
pthread_barrier_t barrier;
int *S ;
int *dist;
int *pre;
int **A ;

void synchronize();

void dijkstra(void* args)
{
   　long thread_rank = (long)args;

     int reminder,quotient;
     int current_process_start_index;
     int current_process_int_num;
     int current_process_end_index;
     reminder = vertexes_number%threads_number;
     quotient = vertexes_number/threads_number;
     if(thread_rank>=reminder)
     {
         current_process_start_index = quotient*thread_rank + reminder;
         current_process_int_num = quotient;
     }else
     {
         current_process_start_index = (quotient+1)*thread_rank;
         current_process_int_num = quotient+1;
     }
     current_process_end_index = current_process_start_index+current_process_int_num-1;

 　　 for(int i=0; i<vertexes_number; i++)
 　　 {
       　　int mindist = INT_MAX;
       　　int u = source_vertex; 　　                  // 找出当前未使用的点j的dist[j]最小值
      　　 for(int j=0; j<vertexes_number; ++j)
      　　    if((!S[j]) && dist[j]<mindist)
      　　    {
         　 　     u = j;                             // u保存当前邻接点中距离最小的点的号码
           　      mindist = dist[j];
       　     }
       　S[u] = 1;
         synchronize();
       　for(int j=current_process_start_index; j<=current_process_end_index; j++)
       　　    if((!S[j]) && A[u][j]<INT_MAX)
       　　    {
           　    　if(dist[u] + A[u][j] < dist[j])     //在通过新加入的u点路径找到离source_vertex点更短的路径
           　    　{
                   　　dist[j] = dist[u] + A[u][j];    //更新dist
                   　　prev[j] = u;                    //记录前驱顶点
            　　    }
        　    　}
        synchronize();
   　　}

}


void init_dijkstra()
{
  S = (int*)malloc(sizeof(int)*vertexes_number);
  dist = (int*)malloc(sizeof(int)*vertexes_number);
  thread_handles = malloc(threads_number * sizeof(pthread_t));

  pre = result_collect;
  A = graph_weight;

  for(int i=0; i<vertexes_number; ++i)
  {
  　　dist[i] = A[source_vertex][i];
  　　S[i] = 0;
  　　if(dist[i] == INT_MAX)
        　　prev[i] = -1;
　　     else
        　　prev[i] = source_vertex;
  }

　 dist[source_vertex] = 0;
　 S[source_vertex] = 1;
}

void finalize_dijkstra()
{
  free(S);
  free(dist);
  free(thread_handles);
}

void synchronize()
{
   pthread_barrier_wait(barrier);
}

void my_pthread_excute()
{
  long thread;

  init_dijkstra();

  pthread_barrier_init(&barrier,NULL, threads_number);
  for(thread=0 ; thread<threads_number; thread++)
    pthread_create(&thread_handles[thread], NULL, Hello, (void *)thread);

  for(thread=0; thread<threads_number; thread++)
    pthread_join(thread_handles[thread], NULL);

  finalize_dijkstra();

 return 0;
}

int main(int argc,char *argv[])
{
    my_init(argc, argv);
    my_pthread_excute();
    print_result(result_collect);
    my_global_free();

    return 0;
}
