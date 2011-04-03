
void cmd(int id_from, int id_cmd, int *args, int sizeArgs);
void error(int id);
int getSharp(int id);			// 1
void ledOn();			// 2
void ledOff();			// 3
void surveiller(int id_msg, char id_sharp);
void indexError();		// -1
void init_sharps();
void check_sharps();

