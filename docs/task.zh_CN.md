
### Intel TBB Task Scheduler
    
[Intel® Threading Building Blocks / Developer Reference / Task Scheduler / Scheduling Algorithm](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-reference/task-scheduler/scheduling-algorithm.html)       
     
[Intel® Threading Building Blocks / Developer Guide / The Task Scheduler / How Task Scheduling Works](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/how-task-scheduling-works.html)       

[Intel® Threading Building Blocks / Developer Guide / General Acyclic Graphs of Tasks](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/general-acyclic-graphs-of-tasks.html)

严格意义上的Task Graph理应是一个DAG(Directed Acyclic Graph，有向无环图)      
而TBB的核心结构——Task Tree——是一个将DAG中的节点的后继(Successor)个数限制为1后，退化得到的树，每个Task唯一的后继被定义为父节点(task::parent)     
//注：在TBB的有向树中，前驱(Predecessor)和后继的方向与有向树(Directed Tree)惯例中的定义恰好相反  

```
task //后继个数限制为1的DAG节点  
    parent //唯一后继 //除root_task以外，都不可能为NULL
    ref_count //前驱的个数
```                    
    
TBB同时支持spawn和enqueue两种策略，但是，在一般情况下，我们只使用spawn；因此，我们也只对spawn进行介绍          
   
```   
custom_scheduler.local_wait_for_all(parent, child) #src/tbb/custom_scheduler.h
    t = child #直接将child用于Scheduler Bypass，不过可能为NULL
    custom_scheduler.process_bypass_loop   
    if(1 == parent.ref_count) return    
    t = custom_scheduler.receive_or_steal_task  
    if(NULL == t) return
```   
//注："if(1 == parent.ref_count) return"是唯一的判定wait的task的是否满足条件的代码，可以看出，Work-Stealing策略的根本缺陷在于，当wait的task的满足条件时，wait函数可能并不会立即返回，而是在执行其它的task（比如，假设第一次判定不成功，那么需要经历custom_scheduler.receive_or_steal_task和custom_scheduler.process_bypass_loop后，才会进行第二次判定）

To Do: **此处应有SVG图**    

只有ref_count为0的task（即DAG中没有前驱的节点，即树中的叶节点）才可能在deque上（即可能被spawn）   
    
在custom_scheduler.process_bypass_loop中     
在task.execute后，会用tally_completion_of_predecessor递减task.parent的ref_count    
当task.parent的ref_count递减至0时，会用Scheduler Bypass或generic_scheduler.local_spawn将task添加到thread's own deque中        
      
TBB允许手动控制task的ref_count从而达到模拟多个后继的目的     
即在task中存放一个后继数组，在task.execute中，对数组中的所有后继模拟进行tally_completion_of_predecessor操作      
即用task.decrement_ref_count递减task的ref_count，当ref_coun递减至0时，用task::spawn来spawn task //这个过程中可以借鉴tally_completion_of_predecessor中的SchedulerTraits::has_slow_atomic和Scheduler Bypass的优化     

// FlowGraph用于解决Task Tree中后继个数为1的限制   

wait方法 //所有方法都能转化为task.spawn_and_wait_for_all     

```         
task.spawn_and_wait_for_all(child) #include/tbb/task.h     
    custom_scheduler.wait_for_all(this, child) #src/tbb/custom_scheduler.h   
        custom_scheduler.local_wait_for_all(this, child)      
```    

```         
task.wait_for_all() #include/tbb/task.h     
    custom_scheduler.wait_for_all(this, NULL) #src/tbb/custom_scheduler.h   
        custom_scheduler.local_wait_for_all(this, NULL)      
--- Equivalent ---      
task.wait_for_all() 
    task.spawn_and_wait_for_all(NULL)
```    

```
task::spawn_root_and_wait(root) #include/tbb/task.h
    generic_scheduler.local_spawn_root_and_wait(root) #src/tbb/scheduler.cpp
        empty_task dummy   
        root.set_parent(dummy)  
        dummy.set_ref_count(2)   
        custom_scheduler.local_wait_for_all(dummy, root) #src/tbb/custom_scheduler.h    
--- Equivalent ---      
task::spawn_root_and_wait(root)
    dummy = new(task::allocate_root())empty_task 
    root.set_parent(dummy)
    dummy.set_ref_count(2)   
    dummy.spawn_and_wait_for_all(root)
```  




 

