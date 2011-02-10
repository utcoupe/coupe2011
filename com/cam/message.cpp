#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "message.h"


using namespace std;



inline void sendPion(const Pion &p)
{
	// send the type and the locatiion of one piece
	cout << p.type << C_SEP3 << p.x << C_SEP3 << p.y;
}

inline void send(const int id_msg, const vector<Pion> &v)
{
	cout << id_msg << C_SEP1; // send id of msg
	// send the type and location of pieces
	for (int p=0;p<v.size()-1; ++p)
	{
		sendPion(v[p]);
		cout << C_SEP2;
	}
	sendPion(v[v.size()-1]);
	cout << endl;
}

inline void split(const string &str, vector<int> &v, const string &delimiters, const string::size_type &begin)
{
	// clear vector
	v.clear();
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, begin);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        v.push_back(atoi(str.substr(lastPos, pos - lastPos).c_str()));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


inline void read(vector<int> &v)
{
	string msg;
	// read message
	cin >> msg;
	// split it and put it in v
	split(msg,v,C_SEND);
}


inline Pion createPion(const int type, const int x, const int y)
{
	Pion pion;
	pion.type = type;
	pion.x = x;
	pion.y = y;
	return pion;
}

