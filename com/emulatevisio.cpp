#include <iostream>

using namespace std;

int main() {
	int ask;
	while(1) {	
		cin >> ask;
		if(ask==1) {
			cout << "p 345 657,p 324 234" << endl;
		} else if(ask==2) {
			cout << "t 987, 321" << endl;
		} else if(ask ==9) {
			return 0;		
		} else {
			cout << "e" << endl;
		}
	}
}
