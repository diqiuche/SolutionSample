#include <iostream>
#include <string>
#include <stdlib.h>
#include <winsock2.h>
#include <google/protobuf/message.h>
#include "person.pb.h"
using namespace google::protobuf;
using namespace std;

Message* createMessage(const std::string& typeName)
{
	Message* message = NULL;
	const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
	if (descriptor)
	{
		const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}

int main()
{
	lm::helloworld msg1;
	msg1.set_id(100);
	msg1.set_str("200");
	msg1.set_opt(50);

	cout << msg1.id() << endl;
	cout << msg1.str() << endl;
	cout << msg1.opt() << endl;

	#define CHECK_SUM_VALUE 4569
	typedef int int32_t;
	const int kHeaderLen = sizeof(int32_t);
	

	string result;
	result.resize(kHeaderLen);

	const std::string& typeName = "muduo.Query";
	int32_t nameLen = static_cast<int32_t>(typeName.size()+1);
	int32_t be32 = htonl(nameLen);
	result.append(reinterpret_cast<char*>(&be32), sizeof be32);
	result.append(typeName.c_str(), nameLen);
	bool succeed = msg1.AppendToString(&result);

	if (succeed)
	{
		const char* begin = result.c_str() + kHeaderLen;
		int32_t checkSum = CHECK_SUM_VALUE;//adler32(1, reinterpret_cast<const Bytef*>(begin), result.size()-kHeaderLen);	//º”√‹
		int32_t be32 = ::htonl(checkSum);
		result.append(reinterpret_cast<char*>(&be32), sizeof be32);


		int32_t len = ::htonl(result.size() - kHeaderLen);
		std::copy(reinterpret_cast<char*>(&len),
			reinterpret_cast<char*>(&len) + sizeof len,
			result.begin());
	}
	else
	{
		result.clear();
	}


	string teststring;
	teststring.resize(4);
	int num = 199;

	memcpy((void*)teststring[0], &num, 4);

	cout << teststring << endl;

	typedef lm::helloworld T;
	T mess;
	std::string type_name1 = mess.GetTypeName();
	std::string type_name = T::descriptor()->full_name();
	cout << type_name << endl;

	Message* newQuery = createMessage(type_name);
	T* one = dynamic_cast<T*>(newQuery);

	one->set_id(200);
	one->set_str("300");
	one->set_opt(60);

	cout << one->id() << endl;
	cout << one->str() << endl;
	cout << one->opt() << endl;
	
	delete one;

	cin.get();
	return 0;
}