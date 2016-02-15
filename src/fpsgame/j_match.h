// beermod match stuff

// ------------------
// header
// ------------------

// ------------------
// implementation
// ------------------

void j_startmatch(int mode, char *map) { // start match
	if(mode==1) return; // coop
	changemap(map, mode); // start map
	j_match = true; // match is on
	j_matchstarted = false;
	j_matchmsg = 0;
	j_lastmapload = 0;
	j_maploaded = 0;
	j_getmatchplayers();
	pausegame(true); // set pause
}
void j_matchcmd(int argc, char **argv, int sender)
{
    if(argc <= 2 || !argv[0] || !argv[1] || !argv[2]) sendf(sender, 1, "ris", N_SERVMSG, tempformatstring("[Error] Usage: %s mode map", argv[0]));
    else j_startmatch(j_mode2int(argv[1]), argv[2]);
}
SCOMMAND(match, PRIV_MASTER, j_matchcmd);
void j_checkmatch() { // check if game is match and do stuff
	if(!j_match) return; // not match
	if(!j_maploaded) {
		if(j_checkloaded()) { // map loaded
			j_lastmapload = totalmillis;
			sendf(-1, 1, "ris", N_SERVMSG, "[match] all players loaded map");
		}
		else return; // map not loaded
	}
	if((totalmillis - j_lastmapload) <= J_MATCH_WAIT_TIME) { // countdown
		if((totalmillis - j_lastmapload) > (j_matchmsg * 1000)) { // seconds passed
			sendf(-1, 1, "ris", N_SERVMSG, tempformatstring("[match] starting in %d seconds", (J_MATCH_WAIT_TIME - (j_matchmsg * 1000)) / 1000));
			j_matchmsg++;
		}
	} else { // countdown finished
		if(!j_matchstarted) { // have to start match
			j_matchstarted = true;
			pausegame(false);
			sendf(-1, 1, "ris", N_SERVMSG, "[match] game started!");
		}
	}
	if(j_matchstarted) { // game is running
		if(mastermode >= 2) { // if locked, check if any players leave
			vector<int> j_lastmatchplayers = j_matchplayers; // old player list
			j_getmatchplayers(); // get new player list
			loopv(j_lastmatchplayers) { // for each player in old list
				bool found = false;
				loopvj(j_matchplayers) { // for each player in new list
					if(j_lastmatchplayers[i]==j_matchplayers[j]) {
						found = true;
						break;
					}
				}
				if(!found) { // player not found: pause game
					pausegame(true);
					sendf(-1, 1, "ris", N_SERVMSG, tempformatstring("[match] game paused because client %d left", j_lastmatchplayers[i]));
					break;
				}
			}
		}
	}
}
void j_getmatchplayers() { // players involved in the match: all non-spectators
	j_matchplayers.shrink(0);
	loopv(clients) {
		if(clients[i]->state.state!=CS_SPECTATOR) j_matchplayers.add(clients[i]->clientnum);
	}
}
