#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x, y, session_id;
    char tag[4], *nickname;
} Player;

Player start_session(int); // get ip as a session id
void move(Player*, int, int);
void kill_session(Player*); // 1m wait and kill the session if there's no request from the ip

#endif