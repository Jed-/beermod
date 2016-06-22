// beermod match stuff

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
		// always get lists of players to prevent problems while changing mastermode
		vector<j_matchplayerinfo> j_lastmatchplayers = gamepaused ? (j_getmatchplayers(), j_matchplayers) : j_matchplayers; // old player list
		j_getmatchplayers(); // get new player list
		// if locked, auto-pause when a player leaves, but only if others can unpause the game
		if(mastermode >= 2) { // if locked, check if any players leave
			bool hasmaster = false, hasadmin = false;
			loopv(clients) { // check if any players are holding privilege
				if(clients[i]->state.state==CS_SPECTATOR || clients[i]->state.aitype!=AI_NONE) continue;
				if(clients[i]->privilege>=PRIV_ADMIN) hasadmin = true;
				if(clients[i]->privilege>=PRIV_MASTER) hasmaster = true;
				if(hasadmin) break;
			}
			if(hasadmin || (!restrictpausegame && hasmaster)) { // ensure players have the power to unpause the game themselves before pausing
				loopv(j_lastmatchplayers) { // for each player in old list
					bool found = false;
					loopvj(j_matchplayers) { // for each player in new list
						if(j_lastmatchplayers[i].clientnum==j_matchplayers[j].clientnum) {
							found = true;
							break;
						}
					}
					if(!found && !gamepaused) { // player not found: pause game
						pausegame(true);
						sendf(-1, 1, "ris", N_SERVMSG, tempformatstring("[match] game paused because player %s (%d) left", j_lastmatchplayers[i].name, j_lastmatchplayers[i].clientnum));
						break;
					}
				}
			}
		}
	}
}
void j_getmatchplayers() { // players involved in the match: all non-spectators
	j_matchplayers.shrink(0);
	loopv(clients) {
		if(clients[i]->state.state!=CS_SPECTATOR) {
			j_matchplayerinfo mp(clients[i]->clientnum, clients[i]->name);
			j_matchplayers.add(mp);
		}
	}
}
