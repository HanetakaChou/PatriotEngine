

## Pipeline  

between tbb and \[1. McCool 2012\]  
filter -- stage   
task -- worker  
task_info -- item  

1.binary tree of filter_node //to support the binary & operation  
make_filter -- filter_node_leaf  
operator& -- filter_node_join  

2.flatten the binary tree and contruct the double link list of concrete_filter //we would begin here had we not support the binary & operation //the pipeline itself is a list rather than a tree and thus the binary & operation should be eliminated  
filter_node(_leaf/_join)::add_to -- pipeline::add_filter  

3.really execute the task  
pipeline_root_task::execute -- stage_task::execute  

we dive into the stage_task::execute  
in general:  
1.treat the first filter particularly —— we determinate the token_index (the index of the item) in the first filter //bind to item(\[1. McCool 2012\] / CHAPTER 9 Pipeline)  
```
if my_at_start //the first filter
  if my_filter->is_serial() //the serial(_in_order/_out_of_order) filter    
    my_object = (*my_filter)(my_object) //execute the filter  
    if my_object || ( my_filter->object_may_be_null() && !my_pipeline.end_of_input) //not the end of input  
    
    else  //the end of input
        return NULL //task terminate //the token_index exceeds the input_tokens (the number of tasks spawned) in the first stage is invalid  
    endif  
  else  
  endif  
else //not the first filter

endif
```

## Reference  
\[1. McCool 2012\] [Michael McCool, James Reinders, Arch Robison. "Structured Parallel Programming: Patterns for Efficient Computation." Morgan Kaufmann 2012.](http://parallelbook.com/)  