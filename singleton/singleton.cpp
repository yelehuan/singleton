//单例注意问题：
//1. 线程安全
//   * 通过一个锁，这样写会稍稍的影响性能，因为每次判断是否为空都需要被锁定,如果很多的线程的话，会造成大量线程的阻塞
//	 * 双重锁定, 
//2. 内存释放，通过智能指针shared_ptr解决

#include "stdafx.h"
#include <iostream>
#include "boost\smart_ptr\shared_ptr.hpp"
#include "boost\thread\mutex.hpp"
#include "boost\thread.hpp"
#include <vld.h>
using namespace std;
using namespace boost;

mutex io_mu;

class singleton
{
public:
	static boost::shared_ptr<singleton> getInstance();
	void getValue(int *value);
	void setValue(int value);
private:
	explicit singleton(){}
	//noncopyable
	singleton(const singleton&){};
	singleton& operator=(const singleton&){};
private:
	int i;
	static mutex mCreateInstanceMu;
	static boost::shared_ptr<singleton> mInstance;
};

boost::shared_ptr<singleton> singleton::mInstance = NULL;
mutex singleton::mCreateInstanceMu;

//boost::shared_ptr<singleton> singleton::getInstance()
//{
//	mutex::scoped_lock lock(mCreateInstanceMu);  //每个线程都会进来试着上锁，在线程较多的情况下影响效率
//	if (mInstance == NULL)
//	{
//		mInstance.reset(new singleton);
//	}
//	return mInstance;
//}

boost::shared_ptr<singleton> singleton::getInstance()
{
	//双重锁定
	//这样只有极低的几率下,越过了if(instance == NULL)的线程才会有进入锁定临界区的可能性，不对阻塞大量的线程
	if (mInstance == NULL)
	{
		{
			mutex::scoped_lock lock(io_mu);
			cout << "id: " << this_thread::get_id() << " instance==NULL" << endl;
		}
		mutex::scoped_lock lock(mCreateInstanceMu);
		if (mInstance == NULL)
		{
			{
				mutex::scoped_lock lock(io_mu);
				cout << "id: " << this_thread::get_id() << " create instance" << endl;
			}
			mInstance.reset(new singleton);
		}
	}
	{
		mutex::scoped_lock lock(io_mu);
		cout << "id: " << this_thread::get_id() << " return instance" << endl;
	}
	return mInstance;
}

void singleton::getValue(int* value)
{
	*value = i;
}

void singleton::setValue(int value)
{
	if (i != value)
	{
		i = value;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	thread single1Thread(&singleton::getInstance);
	thread single2Thread(&singleton::getInstance);
	single1Thread.join();
	single2Thread.join();
	int value = -1;
	singleton::getInstance()->getValue(&value);
	cout << "value: " << value << endl;
	//boost::shared_ptr<singleton> single = singleton::getInstance();
	//single->setValue(5);
	//boost::shared_ptr<singleton> single2 = singleton::getInstance();
	//cout << "ref count: " << single.use_count() << endl;
	//int value = -1;
	//single2->getValue(&value);
	std::system("pause");
	return 0;
}

