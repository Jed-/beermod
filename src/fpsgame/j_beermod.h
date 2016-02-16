// beermod general stuff

// ------------------
// header
// ------------------

// general

bool j_maploaded = false;
long j_lastmapload = 0;

bool j_checkloaded();
int j_mode2int(char *mode);

void j_changemap();
void j_serverupdate();

char *j_lowerstring(char *arg);

#define J_NUMGAMEMODES 23

const char *j_gamemodes[J_NUMGAMEMODES] = {"ffa", "coop", "teamplay", "insta", "instateam", "effic", "efficteam", "tactics", "tacteam", "capture", "regencapture", "ctf", "instactf", "protect", "instaprotect", "hold", "instahold", "efficctf", "efficprotect", "effichold", "collect", "instacollect", "efficcollect"};

// match

#define J_MATCH_WAIT_TIME 5000

bool j_match = false;
bool j_matchstarted = false;
unsigned j_matchmsg = 0;
struct j_matchplayerinfo {
	int clientnum;
	char name[MAXNAMELEN+1];
	j_matchplayerinfo(int _clientnum, char *_name) {
		clientnum = _clientnum;
		copystring(name, _name && _name[0] ? _name : "unnamed", MAXNAMELEN+1);
	}
};
vector<j_matchplayerinfo> j_matchplayers;

void j_startmatch(int mode, char *map);
void j_checkmatch();
void j_noclients();

void j_getmatchplayers();

// share

void j_sendmaster();
void j_share();

// ------------------
// implementation
// ------------------

bool j_checkloaded() { // check that every player has loaded the map
	bool ready = true;
	loopv(clients) {
		if(clients[i]->state.aitype!=AI_NONE || clients[i]->state.state==CS_SPECTATOR) continue; // don't wait for bots and spectators
		if(!clients[i]->xi.maploaded) {
			ready = false;
			break;
		}
	}
	return j_maploaded = ready;
}
int j_mode2int(char *mode) { // convert mode names to their number
	loopi(J_NUMGAMEMODES) {
		if(!strcmp(j_gamemodes[i], j_lowerstring(mode))) return i;
	}
	return 0;
}
char *j_lowerstring(char *arg) { // make strings lowercase
	string s;
	copystring(s, arg, sizeof(s));
	for(int i=0; s[i]; i++) s[i] = tolower(s[i]);
	return newstring(s);
}

void j_changemap() { // called by server::changemap()
	j_match = false;
}
void j_serverupdate() { // called by server::serverupdate()
	j_checkmatch();
}
void j_noclients() {
	j_match = false;
}
