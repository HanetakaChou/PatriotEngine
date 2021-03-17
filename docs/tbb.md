
## Task Scheduler
the task schedulter provide the "task spawn" primitive to implement the parallel patterns  

## Parallel Pattern
the following parallel patterns can be implemented by the "task spawn" primitive

### Pipeline  

in general, the tbb::pipeline is **bind to item**(\[1. McCool 2012\] / CHAPTER 9 Pipeline)  
the task is spawned in the first filter /\*"stage_task::execute" / "if(my_at_start)"\*/ and "reduced" in the serial filter /\*"put_token"\*/ and re-spawned again in the in the serial filter /\*"note_done"\*/

//the serial stage can be implemented as "reduce" //1.\[McCool 2012\]/9.4.2 Pipeline in Cilk Plus

terms between tbb and \[1. McCool 2012\]  
concrete_filter -- stage   
stage_task -- worker  
task_info -- item_info

1.binary tree of filter_node //to support the binary & operation  
make_filter -- filter_node_leaf  
operator& -- filter_node_join  

2.flatten the binary tree and contruct the double link list of concrete_filter //we would begin here had we not support the binary & operation //the pipeline itself is a list rather than a tree and thus the binary & operation should be eliminated  
filter_node(_leaf/_join)::add_to -- pipeline::add_filter //Inorder Traversal  

3.really execute the task  
pipeline_root_task::execute -- stage_task::execute  

we dive into the stage_task::execute  
1."my_token" (the index of the item)   
we estimate the "my_token" in the **first** **serial_in_order** filter (marked by "my_token_ready")       
2."input_tokens" (the "max_number_of_live_tokens" passed to tbb::parallel_pipeline)  
controls the max number of the spawned tasks
the "my_token" exceeds the "input_tokens" is invalid  
```
if is the first filter //"my_at_start"   
  //treat the first filter specially
  //spawn stage_task
  if filter is serial(_in_order/_out_of_order)  
    execute the filter //"my_object = (*my_filter)(my_object)"  
    if not the end of input item    
        //we estimate in the /*first*/ /*serial_in_order*/ filter
        //logically, this code fragment can be eliminated and the "my_token" will be estimaed in the first "put_token"
        if filter is serial_in_order   
          estimate my_token 
          mark estimated //"my_token_ready" 
        endif

        //spawn stage_task
        if not the end of pipeline//"next_filter_in_pipeline"
          if --input_tokens > 0 //not exceed the "max_number_of_live_tokens"

          endif
        else
          //the the end of pipeline
          recycle_as_continuation 
          return this //execute from the first filter again
        endif
    else  
        //the end of input item
        return NULL //task terminate  
    endif  
  else  
  endif  
else //not the first filter

endif
```

## Reference  
\[1. McCool 2012\] [Michael McCool, James Reinders, Arch Robison. "Structured Parallel Programming: Patterns for Efficient Computation." Morgan Kaufmann 2012.](http://parallelbook.com/)  