VARP(autoauth, 0, 0, 1);

void j_clientconnected(int cn) {
	if(serverauth && serverauth[0] && autoauth && ((clientinfo*)getclientinfo(cn))->state.aitype==AI_NONE) {
		sendf(cn, 1, "ris", N_REQAUTH, serverauth);
	}
}
