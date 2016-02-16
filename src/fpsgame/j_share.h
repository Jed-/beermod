void j_share() {
	loopv(clients) {
		if(clients[i]->state.state!=CS_SPECTATOR && clients[i]->state.aitype==AI_NONE && clients[i]->privilege < PRIV_MASTER) {
			clients[i]->privilege = PRIV_MASTER;
		}
	}
	j_sendmaster();
}
void j_sharecmd(int argc, char **argv, int sender)
{
    j_share();
}
SCOMMAND(share, PRIV_MASTER, j_sharecmd);
void j_sendmaster() {
	loopv(clients) {
		if(clients[i]->state.aitype!=AI_NONE) continue;
		packetbuf p(MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
		putint(p, N_CURRENTMASTER);
		putint(p, mastermode);
		loopvj(clients)
			if(clients[j]->privilege >= PRIV_MASTER && z_canseemypriv(clients[j], clients[i])) {
				putint(p, clients[j]->clientnum);
				putint(p, clients[j]->privilege);
			}
		sendpacket(clients[i]->clientnum, 1, p.finalize());
	}
}
