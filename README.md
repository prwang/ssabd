
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

## 算法的局限性

* 这是一个前向的数据流算法，对于一个比较操作，只能推出比较之后的范围，不能得到
比较之前的范围。对于一个算术运算操作，能从两个操作数的范围得知结果的范围，
但不能已知结果的范围反推操作数的范围。t5为了得出ret的210的下界，不仅要得到
最后一次t++之后大于100，而且要得到t++之前大于99，算法不能实现。
* 算法之所以是线性时间，是因为一切“多次”都被认为是无穷次。t9要求计数某事件发生的次数，
如果算法能做到这一点，则可以构造出数据让他不是线性时间的。因此也不能得出精确范围。
