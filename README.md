
# 编译课程项目
## 设计

是对论文*A Fast and Low-Overhead Technique to
Secure Programs Against Integer Overflows* 的实现。使用flex+Bison解析ssa输入流，边解析边生成约束图

用```OP```类代表一切可求值的区间运算，用统一的```same_after_eval```方法
对参数进行求值，并返回结果是否改变。简化了后面算法的实现。

e-SSA的处理比较麻烦，目前在一个函数解析完之后，
在控制流图中每次从true/false两个分支开始传播整个控制流图，
并避开定义的节点，把这里面的变量的使用都替换成添加了比较约束的变量

具体实现，在翻译每个基本块时将块内不能解析的变量加入表中，函数处理完之后首先
进行上述传播，结果为```redirection```表，然后再
解析所有块间的引用。

## 使用方法
编译器：C++14
依赖：cmake bison flex 

二进制文件```main```
使用main -h查看帮助
```
Usage:
  %s [-v] [-g <graph_file>] -f <function_name> -I <interval_1> ... -I <interval_n>  <file>
Options:
  -f    Mandatory. You have to specify the entry function name

  -I    Mandatory. You have to specify the input range(s) of the argument(s)
        of the entry function. <interval> should be
          NUMBER,NUMBER
        where NUMBER is a floating point literal including '+infinity'

  -v    Enable verbose mode

  -g    Dump the constraint graph to <graph_file> in DOT language,
        which may be later visualized with GraphViz.
```

## 进度

1, 2, 3, 4, 6, 7

