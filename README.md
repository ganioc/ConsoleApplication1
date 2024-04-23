## C++ learning projects
### design patterns
资源管理的golden standards for c++

#### execute around design pattern
RAII, (Resource Acquisition Is Initialization)

```cpp
std::lock_guard
std::unique_ptr

```

#### Type Erasure
? 这是什么呢?

```cpp
alignas() // 更改内存的对齐方式
::new()   // 调用global operator new, 而不是类中重载的new
```

#### SFINAE
overload function, 函数的重载,

#### ScopeGuard
error-safe, exception-safe code, RAII make error handling easire,
错误处理, 
高级c++语言特性。


### tests

microbenchmark library

- Google Benchmark library, works fine.

### compute
Use this namespace to write DSP code.

## Compute

```c
recordNum x elements_in_record x elementSize;
```

### File header
* type
* elementSize
* numRecords, 16bit
* recLen, 16bit, 或者32bit， record length, 包含多少个element
* Records[]
* Trailer Text, ASCII string,  

### Long Vector type?
什么是Long Vector? 如果是long vector的话，则numRecords=1, numRecords为32bit > 0xffff;
否则numRecords, recLen都为正常的16bit值。

If recLen > 0xFFFF, numRecords = 1, recLen为32bit(只有这一个元素)

If recLen < 0xFFFF, 正常写入numRecords, recLen,


### fstream的操作
```c
seekg(偏移量，基地址),// 基地址包括: ios::beg,ios::cur,ios::end 
tellg(), // 对输入流操作,返回当前位置

seekp(), 
tellp(), // 对输出流操作
```

### 文件格式
```shell
05 02 02 00 e8 03 f4 01 f3 01 f2 01 f1 01 f0 01
```

## Graphic Display of Data
Basic wave generation is OK. Will use matlab, or python matplot lib to draw the waveform.

## Chap4 Filtering Routines
Nth order filter, FIR filters, 一个冲激，输入信号，不会在最后一个响应后的N个采样之后产生影响。
IIR filters, Infinite Impulse Response, 
















