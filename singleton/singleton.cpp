//����ע�����⣺
//1. �̰߳�ȫ
//   * ͨ��һ����������д�����Ե�Ӱ�����ܣ���Ϊÿ���ж��Ƿ�Ϊ�ն���Ҫ������,����ܶ���̵߳Ļ�������ɴ����̵߳�����
//	 * ˫������, 
//2. �ڴ��ͷţ�ͨ������ָ��shared_ptr���

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
//	mutex::scoped_lock lock(mCreateInstanceMu);  //ÿ���̶߳�������������������߳̽϶�������Ӱ��Ч��
//	if (mInstance == NULL)
//	{
//		mInstance.reset(new singleton);
//	}
//	return mInstance;
//}

boost::shared_ptr<singleton> singleton::getInstance()
{
	//˫������
	//����ֻ�м��͵ļ�����,Խ����if(instance == NULL)���̲߳Ż��н��������ٽ����Ŀ����ԣ����������������߳�
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

