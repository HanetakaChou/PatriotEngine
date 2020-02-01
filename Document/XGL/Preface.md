 
图形API（MTLPixelFormat、DXGI_FORMAT、VkFormat）中的Snorm/Unorm实质上 即定点（Fixed Point）数 //小数点在最左侧  

## IEEE 754  
NAN（Not A Number）  
//__builtin_isnanf   
//任何 比较运算 对NAN 都为假 //包括==运算 //或许可以用 !(x == x) 实现isnanf（受编译器优化影响）    

INFINITY //__builtin_isinff

### Rounding Modes  
https://www.gnu.org/software/libc/manual/html_node/Rounding.html  

Machine Epsilon //是一个常数  

//在round to nearest下  
浮点数加法不满足交换律 从小到大排序后相加 误差较小  