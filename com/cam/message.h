#ifndef MESSAGE_H_
#define MESSAGE_H_



#define C_SEP1	"|"
#define C_SEP2	"/"
#define C_SEP3	"_"
#define C_SEND	":"

using namespace std;

typedef struct Pion
{
	int type, x, y;
}Pion;

inline void sendPion(const Pion &p);
inline void send(const int id_msg, const vector<Pion> &v); // use to send the result
inline void split(const string &str, vector<int> &v, const string &delimiters, const string::size_type &begin = 0);
inline void read(vector<int> &v); // read the incomming command
inline Pion createPion(const int type, const int x, const int y); // create a new triplet 

#endif /* MESSAGE_H_ */

