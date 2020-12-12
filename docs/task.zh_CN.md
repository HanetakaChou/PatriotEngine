
### Intel TBB Task Scheduler

TBB中Task的树状结构，实际上是一个简化后的DAG（Directed Acyclic Graph，有向无环图）

//将DAG（Directed Acyclic Graph，有向无环图）中每个节点（即Task）的后继（Successor）个数限制为1，并将该唯一的后继定义为父节点（Parent），从而将DAG简化为树  
//注：在该DAG简化得到的树中，前驱和后继的方向与常规的有向树定义中的方向恰好相反  

```
task //DAG简化得到的树中的节点
    parent //唯一后继 //除root_task以外，都不可能为NULL
    ref_count //前驱的个数
```

//只有没有前驱（Predecessor）的节点（即树中的叶节点）才可能在Deque上 //即被Spawn  

FlowGraph取消了 Task树 对后继个数为1的限制   
// https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/general-acyclic-graphs-of-tasks.html