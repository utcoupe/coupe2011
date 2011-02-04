#include <iostream>
#include <string>


using namespace std;

int main() {
	string ask;
	int i = 1;
	while(1) {	
		cin >> ask;
		if (ask == "I")
			cout << "I,camb" << endl;
		else
		{
			cout << ask << ",p 300 300" << endl;
			//int a = 3/0;
		}
	}
	
	return 0;
}
