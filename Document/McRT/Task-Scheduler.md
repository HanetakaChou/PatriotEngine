## Task Scheduler

### Intel TBB

基于Tag: 2019_U9  

#### Scheduling Algorithm

//在TBB中

一些结构体定义  
```
task //DAG简化得到的树中的节点
    parent //唯一后继 //除root_task以外，都不可能为NULL
    ref_count //前驱的个数
```

//将DAG（Directed Acyclic Graph，有向无环图）中每个节点（即Task）的后继（Successor）个数限制为1，并将该唯一的后继定义为父节点（Parent），从而将DAG简化为树  
//注：在该DAG简化得到的树中，前驱和后继的方向与常规的有向树定义中的方向恰好相反  

//只有没有前驱（Predecessor）的节点（即树中的叶节点）才可能在Deque上 //即被Spawn 

//Spawn Task可以看作是整个系统的原语（Primitive）

//Worker Thread的Spawn Task会添加到Worker Thread的Deque  

//在Task执行完毕（且未recyle）后，会以原子操作的方式递减parent(即唯一的后继）的ref_count  
//当parent的ref_count递减至0时（即parent不再有前驱），parent会被Spawn  

//根据TBB的设计，Task在被创建时，处于DAG中不可能被执行的部分，因此，在set_ref_count中以非原子操作的方式设置引用计数  
//可能同时 也具有 减少原子操作的次数，提升性能 的目的  
//
//在Debug版本中，TBB提供了es_ref_count_active标志位进行错误检查 //该标志位表示是否该Task的ref_count有可能正在被某个线程以原子操作的方式递减 //实际上即等价于是否有前驱  
//在Spawn Task前（prepare_for_spawning），Task的parent的es_ref_count_active会被设置  
//当parent的ref_count递减至0时，parent的es_ref_count_active会被清空  

//注：set_ref_count是多余的，可以基于allocate_child recycle_as_child_of ... 等确定  
//在prepare_for_spawning中，ref_count被同步到0

//recycle（或者说set_parent）必定维持successor的ref_count不变 //因为，set_parent要求successor为NULL
//task::execute内的所有方法都符合es_ref_count_active语义，即不改变ref_count

//TBB基本语义 将ref_count减少至0的 操作者  有义务Spawn //但spawn并不是随时都可以进行

//state
//None -Allocate-> allocate -Spawn-> ready -BypassLoop-> executing -Recycle-> recycled -BypassLoop-> allocate  
//                                                          |->freed //TBB特有 //对内存分配进行了缓存  

//recycle_as_safe_continuation  
//set_ref_count比实际值大1  
//为避免 其它线程 在原Task的execute返回前 steal并execute了该Task的child 且将该Recycle的Task的ref_count递减至0 从而并发执行Recycle为continuation的Task的execute  
//当state从recycled转换为allocat时，该额外的引用计数会被递减 （注意：代码在case recycled:中并没有break，执行到case to_enqueue:下的tally_completion_of_predecessor时，递减引用计数）  
  

//何时停止
work steal的缺陷
//worker thread正在执行剩余的最后1个task
//master thread判断task并没有全部完成 转去执行其它线程的task（即master thread steal 其它线程的thread)
//worker thread执行完毕 //master thread理应可以返回 但是却在执行其它线程的task (显然，task的excute方法内部不可中断)

custom_scheduler.h  
local_wait_for_all  
```
local_wait_for_all
    
    process_bypass_loop
        t //Task Executing
        t_next //Task Bypass

        t_next = t->execute()

        switch(t->state)
            case executing:
            assert(t->ref_count == 0) //t会被释放 //显然不应当再有前驱
                tally_completion_of_predecessor
                    //由于原子操作较慢，进行了优化
                    bool completion_of_predecessor;
                    if(==1)
                    {
                        ref_count = 0
                        completion_of_predecessor = true;
                    }
                    else
                    {
                        if(Atomic_Add(ref_count,-1)>1)
                        {
                            completion_of_predecessor = false;
                        }
                        else
                        {
                            assert(ref_count==0);
                            completion_of_predecessor = true;
                        }
                    }

                    if(completion_of_predecessor)
                    {
                        spawn parent(//Successor)
                    }

                    //Bypass Parent 的优化


```

(2.\[Intel® Software 2019\]/Developer Reference/Task Scheduler/Scheduling Algorithm)


#### Continuation Passing


Continuation-passing Style / Blocking Style (2.\[Intel® Software 2019\]/Developer Reference/Task Scheduler)  

Continuation Passing (2.\[Intel® Software 2019\]/Developer Guide/The Task Scheduler/Useful Task Techniques/Continuation Passing)  

steal children / steal continuations (1.\[McCool 2012\]/8.5 Load Balancing)  

#### Scheduler Bypass
https://software.intel.com/en-us/node/506107  

#### task_list

### Patriot TBB


## Generic Parallel Algorithms

#### Fork-Join

即TaskScheduler天生支持的Spawn  

### Map

Latency

(1.\[McCool 2012\]/8.3 Recursive Implementation Of Map)  

### Pipeline

memory_order  
volatile  
https://en.cppreference.com/w/cpp/atomic/memory_order  


Throughout  

(1.\[McCool 2012\]/9.4.2 Pipeline in Cilk Plus)  

pipeline.cpp

--filter_list------------------------

pipeline
    filter_list

filter
    next_filter_in_pipeline

-------------------------------------

stage_task::execute
```
concrete_filter<T,U,Body> //U=Body(T)
    //部分专用化 //concrete_filter<void,U,Body> //construct //filter_may_emit_null //object_may_be_null 

stage_task : task, task_info
--task_info------ //严格意义上应当叫item_info //因为task会终止（执行到serial阶段，不等于low_token时，put_token) 再次spawn（等于low_token的task，note_down）
    //虽然在逻辑上，
    my_object //token_helper::cast_to_void_ptr //serial_in_order
    my_token //从my_pipeline的token_counter取得 //pipeline内唯一
    my_token_ready //只在first_stage且serial时，才会设置my_token_ready //first stage parallel 或 not first stage（serial or parallel）不会设置 //由于bind-to-item，task_info可以保存到下一个stage（直到遇到serial stage）
    is_valid //在input_buffer中，前一阶段的item到达的速度并非一致
--stage_task-----
    my_pipeline //
        --worker控制--
        input_tokens //parallel_pipeline传入 //在spawn task时 会--（atomic）
        --item控制--
        token_counter //在my_at_start时 不断++（atomic） 为pipeline中每一个item分配一个唯一的my_token
        end_of_input //return NULL 或 control.is_pipeline_stopped -> filter::set_end_of_input

    my_filter //current filter //my_filter = my_filter->next_filter_in_pipeline //不断向后迭代
        my_input_buffer //serial
            task_info *array //
            low_token //prior Token have already been seen //Does "seen" mean spawn ???

    my_at_start //bind to item (1.[McCool 2012]) //first stage(即filter) of pipeline

--Process Current Stage---

//process first stage //第一阶段特殊处理
1.当第一阶段serial时，确保串行化
2.确定pipeline中item的个数
if(my_at_start) //first stage(即filter) of pipeline
{
    if(my_filter->is_serial()) //serial_in_order或serial_out_of_order
    {
        //first stage serial 

        --Execute Filter--
        my_object = (*my_filter)(my_object) //concrete_filter::operator()
            //部分专用化 //concrete_filter<void,U,Body> //flow_control //end_of_input
   
        --End of Input-- //Unlikely
        if !(
                (my_object != NULL) //T非void的filter，返回NULL即表示结束
                || (my_filter->object_may_be_null() && !my_pipeline.end_of_input) 
            ) 
        {
            my_pipeline.end_of_input = true;

            --Worker_Terminate--
            return NULL; //Execute End //超过第一个stage产生的item个数 的Worker是没有意义的
        }

        --item控制--
        if my_filter->is_ordered() //serial_in_order
        {
            my_token = atomic_fetch_add(my_pipeline.token_counter, 1) //only need release semantic???
            my_token_ready = true; 
        }

        --worker控制--
        if atomic_fetch_add(input_tokens,-1) > 1
            spawn stage_task //first stage 

        //if( !my_filter->next_filter_in_pipeline ) // we're only filter in pipeline //unlikely
        //{ 
        //    process another stage
        //}
    }
    else //parallel
    {
        //first stage parallel 

        --End of Input--
        if my_pipeline.end_of_input //May Race Condition
        {

            --Worker_Terminate--
            return NULL
        }
        
        --worker控制--  //与serial区别 先spawn再执行filter           
        if atomic_fetch_add(input_tokens,-1) > 1
            spawn stage_task //first stage 
        
        //注：没有item控制！ //体现在my_token_ready

        --Execute Filter--
        my_object = (*my_filter)(my_object)

        --End of Input--
        if ...
        {
            my_pipeline.end_of_input = true;

            --Worker_Terminate--
            return NULL;
        }

    }

    //mark not first stage ----------------------------------
    my_at_start = false;
}
else //not first stage //能否将 not-first-stage 和 my_filter = my_filter->next_filter_in_pipeline 合并到迭代中
{
    //--process_another_stage--之后 进入此处

    Execute Filter

    //没有my_token_ready

---确保非first stage的serial filter串行化----

    if my_filter is serial //Current Stage Serial //item_info.my_token == low_token才可能执行 //不断spwan Task 与 Worker Terminate 保持平衡 //token的个数在第1阶段确定 //token_index（bind to item）在第一个serial_in_order阶段确定

    //serial阶段可以认为是一个同步点，只有item_info.my_token == low_token的task才会执行

    {
        /*my_filter->my_input_buffer->note_done*/ //Spawn Task
        
        if current-filter not is ordered /*serial_out_of_order*/ 
            ||  item_info.my_token == my_input_buffer.low_token //为何不是必定相等？？？ //与
        {
           --pop-from-input-buffer--

           //array_size maintain a ringbuffer

        }

        if item-poped is_valid //比如 当前item(1) //input_buffer中 item(2) not is_valid | item(3) is_valid
        {
            spawn_task //从当前filter开始 //而非at_start
        } 
        else
        {
            //不spawn_task //由于put_token是互斥的 //如果此处没有spawn（item(2) not is_valid | item(3) is_valid）,那么bind to item(2)的task在执行完上一个stage后 执行put_token时，会process_another_stage
        }

        //bind-to-item //继续向下执行               
        //my_filter = my_filter->next_filter_in_pipeline;

--note_done---------------
        
        my_object = (*my_filter)(my_object) //Execute Filter

        --lock--------------------

        if(token == low_token) //一定成立？？？ //实际测试 !is_order || token == low_token 一定成立 //可能与thread_bound_filter有关（bind-to-stage？）
        {
            ++low_token
            item_info = input_buffer[low_token]
        }

        --unlock------------------

        if(item_info.is_valid)
        {
            spawn_task //current_fitler //next_token
        }

--put_token--------------------

        bool put_token

        --lock-----------------------------

        if(token == low_token)
        {
            put_token = false
        }
        else
        {
            input_buffer[token]= item_info 

            put_token = true
        }

        --unlock---------------------------

        if(!put_token)
        {
            process_another_stage
        }
        else
        {
            worker_terminate //bind-to-item 
        }
    }   
}

//Execute Filter End //接下来不会再ExecuteFilter
//token_counter++ End //接下来不会再 token_counter++

//bind to item //不断向下一个stage迭代 //process_another_stage之后 进入!my_at_start
my_filter = my_filter->next_filter_in_pipeline;

--Analyze Next Stage---

if my_filter != NULL 
{
    if the-next-filter is serial 
    {
        /* my_filter->my_input_buffer->put_token(当前task的item_info) */ //当前task的task_info //即item_info //由于bind-to-item，task_info可以保存到下一个stage（直到遇到serial stage）  

---确保非first stage的serial filter串行化-----
//1.[McCool 2012] 9.4.2 Pipeline in Cilk Plus
    So reducer_consumer joins views using the following rules:
• If the left view is leftmost, its list is empty. Process the list of the right view.
• Otherwise, concatenate the lists.

        /---需要互斥

        Token item_token; //item_info.my_token只在部分条件下才有效  //The terminology ""
        if the-next-filter is orderd //serial_in_order
        {
            if(!item_info.my_token_ready) //first stage parallel不会设置my_token_ready //not first stage（serial or parallel）
            {
                item_info.my_token = high_token++;
                item_info.my_token_ready = true;
            }

            item_token = item_info.my_token;
        }
        else //serial_out_of_order
        {
            item_token = high_token++;
        }

        if(item_token == low_token) //leftmost //以此来保证串行 //put_token需要互斥
        {
            --process_another_stage---------
            
            recycle_to_reexecute //recycle_as_safe_continuation() //set_ref_count(1)
            return NULL;
        }
        else
        {
            //push to inputbuffer

            //grow array

            //push //根据token_index设置is_valid

            //if was_empty then sema_V

            --Worker_Terminate--
            return NULL;
        }
    }
    else
    {
        //Next Stage Is Parallel

        --process_another_stage-------------
        
        recycle_to_reexecute //recycle_as_safe_continuation() //set_ref_count(1)
        return NULL;
    }
}
else
{
    // the-next-filter is NULL


}


```


## Flow Graph  

### Wavefront  

(2.\[Intel® Software 2019\]/Design Patterns/Wavefront)  



## 参考文献
[1.\[McCool 2012\] Michael McCool, James Reinders, Arch Robison. "Structured Parallel Programming: Patterns for Efficient Computation." Morgan Kaufmann Publishers 2012.](http://parallelbook.com/)   

[2.\[Intel 2019\] Intel. "Intel Threading Building Blocks Documentation." Intel Developer Zone 2019.](https://www.threadingbuildingblocks.org/docs/help/index.htm)   
 
[Herlihy 2012] Maurice Herlihy, Nir Shavit. "The Art of Multiprocessor Programming, Revised First Edition" Morgan Kaufmann Publishers 2012.  

Structured Parallel Programming with Deterministic Patterns  