#include<iostream>
#include"test.h"
using namespace std;


class A:public Slot
{
public:
	void fun555cc()
	{
		cout << 555 << endl;
	}
	Signal<void(int)> signal;

};

class B:public Slot
{
public:
	void funcc(int a)
	{
		cout << a<<endl;
	}
	void funcc22()
	{
		cout << 22 << endl;
	}
	Signal<void()> signal;
};
;

int main()
{
	A a;
	B b;
	connect(a.signal, &b, &B::funcc);
	a.signal(20);

}