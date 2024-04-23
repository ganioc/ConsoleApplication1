## C++ learning projects
### design patterns
��Դ�����golden standards for c++

#### execute around design pattern
RAII, (Resource Acquisition Is Initialization)

```cpp
std::lock_guard
std::unique_ptr

```

#### Type Erasure
? ����ʲô��?

```cpp
alignas() // �����ڴ�Ķ��뷽ʽ
::new()   // ����global operator new, �������������ص�new
```

#### SFINAE
overload function, ����������,

#### ScopeGuard
error-safe, exception-safe code, RAII make error handling easire,
������, 
�߼�c++�������ԡ�


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
* recLen, 16bit, ����32bit�� record length, �������ٸ�element
* Records[]
* Trailer Text, ASCII string,  

### Long Vector type?
ʲô��Long Vector? �����long vector�Ļ�����numRecords=1, numRecordsΪ32bit > 0xffff;
����numRecords, recLen��Ϊ������16bitֵ��

If recLen > 0xFFFF, numRecords = 1, recLenΪ32bit(ֻ����һ��Ԫ��)

If recLen < 0xFFFF, ����д��numRecords, recLen,


### fstream�Ĳ���
```c
seekg(ƫ����������ַ),// ����ַ����: ios::beg,ios::cur,ios::end 
tellg(), // ������������,���ص�ǰλ��

seekp(), 
tellp(), // �����������
```

### �ļ���ʽ
```shell
05 02 02 00 e8 03 f4 01 f3 01 f2 01 f1 01 f0 01
```

## Graphic Display of Data
Basic wave generation is OK. Will use matlab, or python matplot lib to draw the waveform.

## Chap4 Filtering Routines
Nth order filter, FIR filters, һ���弤�������źţ����������һ����Ӧ���N������֮�����Ӱ�졣
IIR filters, Infinite Impulse Response, 
















