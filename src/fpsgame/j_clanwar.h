// beermod clanwar stuff

void j_startcw(char *clan1, char *clan2, int mode, char *map);
void j_setclans(char *clan1, char *clan2);

// --------------------

void j_startcw(char *clan1, char *clan2, int mode, char *map) {
	j_startmatch(mode, map);
	j_setclans(clan1, clan2);
}
void j_cwcmd(int argc, char **argv, int sender) {
	if(argc < 5) sendf(sender, 1, "ris", N_SERVMSG, tempformatstring("[Error] Usage: %s clan1 clan2 mode map", argv[0]));
	else j_startcw(argv[1], argv[2], j_mode2int(argv[3]), argv[4]);
}
SCOMMAND(cw, PRIV_MASTER, j_cwcmd);
SCOMMAND(clanwar, PRIV_MASTER | ZC_HIDDEN, j_cwcmd);
void j_setclans(char *clan1, char *clan2) {
	loopv(clients) {
		if(clients[i]->state.aitype==AI_NONE && clients[i]->state.state!=CS_SPECTATOR) {
			if(strstr(clients[i]->name, clan1)) {
				if(!isteam(clients[i]->team, "good")) {
					copystring(clients[i]->team, "good", MAXTEAMLEN+1);
					sendf(-1, 1, "riisi", N_SETTEAM, clients[i]->clientnum, clients[i]->team, 1);
				}
			} else if(strstr(clients[i]->name, clan2)) {
				if(!isteam(clients[i]->team, "evil")) {
					copystring(clients[i]->team, "evil", MAXTEAMLEN+1);
					sendf(-1, 1, "riisi", N_SETTEAM, clients[i]->clientnum, clients[i]->team, 1);
				}
			} else forcespectator(clients[i]);
		}
	}
}
