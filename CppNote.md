1.关于cpp函数返回值与指针的相关问题
在操作符重载过程中，常常可以使用 `&`操作符来返回返回对象的引用。
默认情况下，返回的返回值会对一个临时变量进行值传递，就像实参对形参赋值一样。
函数返回类型后添加`&`操作符，可以返回这个返回值对象的引用，即指针传递。
`&`取地址，`*`取值。
`&`在修饰局部变量的时候解释为当前局部变量是传入参数的别名。
但是`*`在声明的时候作为指针变量声明。
```c++
class Test
{
    //想象地址是信箱的名字，内存是信箱的内容。
    //当前对象是一个有名字的有内容的信箱。
public: 
 Test()
 { 
  return this; //返回的当前对象的地址
  //this就是当前对象的地址，这个地址被值传递传递出去，外部得到一个通过局部变量保存的当前对象的地址。
  //地址内容还是那个内容，但是存放地址的地方已经不一样了。
  //类似于两张署名相同的信封放在不同的信箱里。
 }
 Test&()
 { 
  return *this; //返回的是当前对象本身
  //*this取了当前对象的值，通过引用传递传递出去，则外部得到的是当前对象的别名
 }
 Test()
 { 
  return *this; //返回的当前对象的克隆
  //*this取了当前对象的值，通过值传递传递出去，因此是克隆
  //地址内容并没有被传递出去，但是信件的内容被誊抄了一份放在另一个信箱里
 }
 Test&()
 { 
  return this; //猜测返回的是当前对象地址的别名
  //this取了当前对象的地址，通过引用传递传递出去，得到当前对象地址别名
  //地址内容是那个内容，并且存放地址的信箱也是同一个，但是叫法不一样。
 }
private: //...
};

```
```c++
#include<iostream>
using namespace std;
int main(){
    int a=123;
    //&a表示a在内存中的地址，也就是123在内存中的地址
    cout<<"a: "<<a<<endl<<"a's address："<<&a<<endl;
    //此时p是一个指针，指向a所在的位置
    int *p=&a;
    cout<<"p: "<<p<<endl;
    //声明p之后，在p之前添加*表示p指向内存的值
    cout<<"p's value: "<<*p<<endl;
    //同时p也是 一个变量，在内存中也有一个地址储存它，但其地址不是a的地址
    cout<<"p's address: "<<&p<<endl;
    //试试*&组合使用是什么效果
    cout<<"*&p: "<<*&p<<endl;
    //&p是一个内存地址，*&p表示&p指向地址内存空间的值,在这里表示a的地址
    cout<<"**&p: "<<**&p<<endl;
    //刚才我们已经知道*&p是a的地址，那么**&p就表示a的值
return 0;}
```
2. 关于`const`，一旦使用`const`就是不能更改的
`const`用在变量声明上，一旦初始化就无法更改。
`const`用在函数声明上，表示只读函数，不允许修改类内成员变量。
`const`用在函数形参声明上，不允许修改函数形参的值。
3. 关于文件操作与输入输出流操作
flush标识符能够刷新流缓冲区，在命令行里体现为删除以往打印数据重新打印并刷新。
特点是能够在缓冲区不断删除上一个以flush结尾的数据。
但是在文件流中不起作用，仅产生换行。
```c++
//文件操作示例
#include <fstream>//包含头文件，可以使用文件流操作

int main(){
    ofstream fs;//如果是读取文件就是用ifstream
    fs.open("log.txt",ios::out);//这会自己创建一个文件，因此不用费心创建了
    fs.clear();

    fs<<image_width<<' '<<image_height;//直接跟cout一样进行写入

    fs.close();//注意关闭文件流
}
```
4.关于c++输出流的问题，主要有`cout`、`clog`和`cerr`输出流。
`cout`标准输出流，可以在命令行重定向到某个文件指定输出。
`clog`和`cerr`都是标准错误流，log有缓冲区，err无缓冲区。
5.能够使用`using`关键字来进行别名指定。
6.关于纯虚函数
声明虚函数时在函数声明后加`=0`，告知编译器这是一个虚函数，只是一个函数声明而不具备函数功能，具体函数体留待派生类来实现，因此这里不需要使用`{}`来定义函数体。
```c++

class hittable{
    public:
        virtual bool hit(const ray& r,double t_min,double t_max,hit_record& rec)const=0;
};
```
7. 关于智能指针
传统而言，对于引用类型，c++中使用new 和delete来申请删除内存区域，相对麻烦。
在现代c++中提供了所谓智能指针自动帮你完成内存的回收工作。
主要有两种智能指针：`unique_ptr`和`shared_ptr`，这两个类被定义在`<memory>`头文件中，且需要使用命名空间`std`。
智能指针会在指针变量超出该变量的作用域时自动删除该指针指向的内存。
可以认为`shared_ptr<int>``unique_ptr<int>`是`int *`的高级写法，而新建内存时则可以使用`make_shared<int>()`来申请，其中括号自动调用目标类型的构造函数。
除了定义和声明变量之外，智能指针对象的用法可以简单当作普通指针来使用。
`unique_ptr`顾名思义，就是当你声明了一个`unique_ptr`变量并指向一个内存时，不允许其他`unique_ptr`指针指向同一个内存。因为这会导致另一个`unique_ptr`指向的内存为空。
`shared_ptr`与`unique_ptr`在这方面有些许不同，`shared_ptr`允许多个`shared_ptr`指针指向同一个内存，而不会出现`unique_ptr`出现的问题。
因为`shared_ptr`在另一块内存中自动维护了一个指针计数器，在当前有多个`shared_ptr`指向同一个内存时，某个`shared_ptr`指针超出作用域不会立刻删除该内存，而只会时指针计数器减一。只有当指针计数器知道目前只有一个`shared_ptr`指向这块内存时，才会在这唯一的`shared_ptr`指针超出作用域时执行内存释放的操作。


8. 关于头文件相互引用的问题
下面代码中，射线打到了某个物体上并记录了该物体的`material`信息，希望通过入射角和交点信息，结合`material`本身的行为，知道出射射线将会去哪里，顺便知道一下这个材质的颜色衰减。
现在，在这个`material`类中，虽然没有定义一个`hit_record`的成员，但是某个函数成员使用到了`hit_record`这个类型，某种程度上也可以说`material`的存在需要首先有`hit_record`的存在才成立。
但是，看看`hit_record`类，这个类型本身就希望它能够记录`material`的信息，因此，为了记录这个信息，那么`material`类也需要先于`hit_record`类存在才行。
因此这里存在两个文件相互依赖问题。
解决方法是，其中一方可以直接`include`对方的头文件，但相对的，对方不能引用它的头文件，会导致其中一个类型被解释为未定义。解决方法是，对方在自己的头文件内声明一个同名的类，但不做实现，仅告诉编译器，这里会使用一个未定义的类。
而在main函数的文件中，则必须要引用`material.h`，因为单单引用了`hittable.h`并不包含`material.h`中的任何定义。
只针对目前光线追踪来说，这里其实`material`必须知道`hit_record`的相关信息，但是反过来`hit_record`其实可以不知道`material`，`hit_record`只不过是一个信使，真正关心`material`的是背后的光追递归函数。
因此只要在每次求交时，物体除了传递一个碰撞记录以外，再额外传递一个材质记录就可以了。正如`hit_record`被一个撞击函数使用引用传递传出来一样，材质记录也可以做到在每次求交时被送回到上层函数本身。

```c++
//material.h
#include "hittable.h"

class material{
    public:
        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered
        )const=0;
};
```
```c++
//hittable.h
class material;

struct hit_record{
    point3 p;//position of hit
    vec3 normal;//position of normal
    shared_ptr<material> mat_ptr;
    double t;//t when hit
    
    bool front_face;

    inline void set_face_normal(const ray& r,const vec3& outward_normal){
        front_face=dot(r.direction(),outward_normal)<0;
        normal=front_face?outward_normal:-outward_normal;
    }
};

```