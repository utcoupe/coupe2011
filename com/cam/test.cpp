#include <iostream>
#include <vector>
#include <string>
#include "message.cpp"
#include "message.h"



int main()
{
	vector<Pion> v;
	vector<int> msg;
	
	read(msg);
	cout << "id : " << msg[0] << endl;
	if (msg.size() > 1)
		cout << "cmd : " << msg[1] << endl;
	else
		cout << "cmd : None" << endl;
	if (msg.size() > 2)
	{
		cout << "params : ";
		for (int i=2; i<msg.size();++i)
			cout << msg[i] << " ";
		cout << endl;
	}
	else
		cout << "params : None" << endl;

	v.push_back(createPion(0,300,500));
	v.push_back(createPion(1,500,800));
	
	send(msg[0],v);
	
	return 0;
}

