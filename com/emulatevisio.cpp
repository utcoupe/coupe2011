#include <iostream>

using namespace std;

int main() {
	int ask;
	int i = 1;
	while(1) {	
		cin >> ask;
		cin.ignore(); // Censé amélioré la communication
		if(ask==1) {
			switch(i) {
				case 1:
					cout << "p 345 657" << endl;
				break;
				case 2:
					cout << "p 345 -400" << endl;
				break;
				case 3:
					cout << "p 0 657" << endl;
				break;
				case 4:
					cout << "p 0 400" << endl;
				break;
				case 5:
					cout << "p 600 0" << endl;
					i=0;
				break;
			}
			i++;
		} else if(ask==2) {
			cout << "t 987 321" << endl;
		} else if(ask ==9) {
			return 0;		
		} else {
			cout << "e" << endl;
		}
	}
}
