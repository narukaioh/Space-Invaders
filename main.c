#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>

struct Sprite{
	int x, y, dot;
	int width,height;
};

volatile int xa = 0;
volatile int condicao = 0;
volatile int ya = 26;
volatile int it = 0;
volatile struct Sprite project[1000];
volatile struct Sprite aliens[75];
volatile int score = 0;
volatile int velocidade = 45;
volatile int r = 0;
volatile int al = 0;

void countXa(){
	if(condicao == 0){
	    xa = xa + velocidade;
		if(xa >= 325){
			condicao = 1;
			ya = ya + 35;
		}
	}
	if(condicao == 1){
		xa = xa - velocidade;
		if(xa <= 0){
			condicao = 0;
			ya = ya + 35;
		}
	}
}

END_OF_FUNCTION(countXa);
LOCK_FUNCTION(countXa);
LOCK_VARIABLE(countXa);

void countGun(){
	if(it > 0){
		int i;
		for(i = 0; i < it ; i++){
			project[i].y = project[i].y - 1;
		}
	}if(it > 100){
		it = 0;
	}
}
END_OF_FUNCTION(countGun);
LOCK_FUNCTION(countGun);
LOCK_VARIABLE(countGun);

int startGame();
void formation();
void setAlien(int i,int x,int y);
void configAll();
void shotGun(int x,int y);
int spriteCollide (struct Sprite a, struct Sprite p);
int collid(struct Sprite p);
void getRound();
void restartGame();
void printScore(BITMAP * buffer);
void initStart(BITMAP * buffer,BITMAP * init);
void controllGun(BITMAP * buffer,BITMAP * gun,SAMPLE * music);
void controllNave(BITMAP * buffer,BITMAP * nave,SAMPLE * sound, int x,int y);
int controllAlien(BITMAP * buffer,BITMAP * alien,BITMAP * bg);
int gameControll(BITMAP * nave,BITMAP * buffer,BITMAP * alien,BITMAP * gun,BITMAP * bg,SAMPLE * sound,SAMPLE * music);
void destroyAll(BITMAP * nave,BITMAP * buffer,BITMAP * alien,BITMAP * gun,BITMAP * init,BITMAP * bg,SAMPLE * sound,SAMPLE * music);
int printImg(BITMAP * nave, BITMAP * buffer,BITMAP * alien,BITMAP * gun,SAMPLE * sound,SAMPLE * music,BITMAP * bg, int x,int y);

int main(){
	return startGame();
}

void initStart(BITMAP * buffer,BITMAP * init){
	while(!key[KEY_ENTER]){
		clear(buffer);
		draw_sprite(buffer,init,0,0);
		blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
		if(key[KEY_ESC]){
			exit(0);
		}
	}
}

int gameControll(BITMAP * nave,BITMAP * buffer,BITMAP * alien,BITMAP * gun,BITMAP * bg,SAMPLE * sound,SAMPLE * music){
	int x = 410,y=700,gameOver;
	while(!key[KEY_ESC]){
		gameOver = printImg(nave,buffer,alien,gun,sound,music,bg,x,y);
		if((key[KEY_LEFT])&&(x > 0)){
			x = x-3;
		}else if((key[KEY_RIGHT])&&(x < 950)){
			x = x+3;
		}
		if(al>=75){
			getRound();
			textprintf_ex(screen,font,400,350,makecol(255,255,255),-1,"ROUND %d !!",r);
			rest(5000);
			velocidade = velocidade + 20;
			restartGame();
		}
		if(gameOver == 1){
			textprintf_ex(screen,font,400,350,makecol(255,255,255),-1,"GAME OVER!",0);
			rest(5000);
			break;
		}
	}
}

void destroyAll(BITMAP * nave,BITMAP * buffer,BITMAP * alien,BITMAP * gun,BITMAP * init,BITMAP * bg,SAMPLE * sound,SAMPLE * music){
	destroy_bitmap(nave);
	destroy_bitmap(buffer);
	destroy_bitmap(alien);
	destroy_bitmap(init);
	destroy_bitmap(gun);
	destroy_sample(sound);
	destroy_sample(music);
	destroy_bitmap(bg);
}

int startGame(){
	configAll();

	BITMAP * nave = load_bitmap("img/35x45/nave.bmp",NULL);
	BITMAP * alien = load_bitmap("img/35x45/alien.bmp", NULL);
	BITMAP * gun = load_bitmap("img/35x45/gun.bmp", NULL);
	BITMAP * init = load_bitmap("img/start.bmp", NULL);
	BITMAP * bg = load_bitmap("img/bartop.bmp", NULL);
	BITMAP * buffer = create_bitmap(SCREEN_W,SCREEN_H);
	SAMPLE * sound = load_wav("wav/tiro.wav");
	SAMPLE * music = load_sample("wav/laserHit.wav");

	initStart(buffer,init);
	install_int_ex(countXa,SECS_TO_TIMER(1));
	install_int_ex(countGun,MSEC_TO_TIMER(1));
	gameControll(nave,buffer,alien,gun,bg,sound,music);
	destroyAll(nave,buffer,alien,gun,init,bg,sound,music);
	return 0;
}

void getRound(){
	r = score/750;
}

void restartGame(){
	xa = 0;
	condicao = 0;
	ya = 26;
	it = 0;
	al = 0;	
	formation();
}
void controllNave(BITMAP * buffer,BITMAP * nave,SAMPLE * sound, int x,int y){
	draw_sprite(buffer, nave,x,y);
	int color = 0,s;
	if(!key[KEY_SPACE] == 0){
		it++;
		rest(100);
		shotGun(x+20,y-20);
		s = play_sample(sound, 100, 0, 2000, 0);
	}
}

int controllAlien(BITMAP * buffer,BITMAP * alien,BITMAP * bg){
	int i,j,cont=0,c=0;
	clear_bitmap(buffer);
	draw_sprite(buffer,bg,0,0);
	for(i=0;i<5;i++){
		for(j=0;j<15;j++){
			aliens[c].x = xa+(j*45);
			aliens[c].y = ya+cont;
			if(aliens[c].dot == 0){
				draw_sprite(buffer,alien,xa+(j*45),ya+cont);
				if(aliens[c].y >= 690){
					return 1;
				}
			}else{
				al++;
			}
			c++;
		}
		cont = cont + 45;
	}
}

int printImg(BITMAP * nave, BITMAP * buffer,BITMAP * alien,BITMAP * gun,SAMPLE * sound,SAMPLE * music,BITMAP * bg, int x,int y){
	int valid = controllAlien(buffer,alien,bg);
	if(valid == 1){ return 1; }
	controllNave(buffer,nave,sound,x,y);
	controllGun(buffer,gun,music);
	printScore(buffer);
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W,SCREEN_H);
	clear(buffer);
	return 0;
}

void controllGun(BITMAP * buffer,BITMAP * gun,SAMPLE * music){
	int v,s;
	if(it > 0){
		int u;
		for(u=0;u<it;u++){
			if(project[u].dot == 0){
				v = collid(project[u]);
				if(v == 0){
					draw_sprite(buffer,gun,project[u].x,project[u].y);
				}else{
					s = play_sample(music, 200, 0, 2000, 0);
					score = score + 10;
					project[u].dot = 1;
				}
			}
		}
	}
}

void printScore(BITMAP * buffer){
	textprintf_ex(buffer,font,145,10,makecol(255,255,255),-1,"%d",score);
	if(al<75){
		al = 0;
	}
}

void shotGun(int x,int y){
	project[it-1].x = x;
	project[it-1].y = y;
	project[it-1].width = 5;
	project[it-1].height = 10;
	project[it-1].dot = 0;
}

void setAlien(int i,int x,int y){
	aliens[i].x = x;
	aliens[i].y = y;
	aliens[i].width = 40;
	aliens[i].height = 50;
	aliens[i].dot = 0;
}

void formation(){
	int i,j,c=0,cont=0;
	for(i=0;i<5;i++){
		for(j=0;j<15;j++){
			setAlien(c,xa+(j*45),ya+cont);
			c++;
		}
		cont = cont + 45;
	}
}

void configAll(){
	allegro_init();
	install_timer();
	install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,"OK");
	install_keyboard();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,1000,800,0,0);
	formation();
}

int collid(struct Sprite p){
	int i,col;
	for(i=0;i<75;i++){
		col = spriteCollide(aliens[i],p);
		if((col == 1)&&(aliens[i].dot != 1)){
			aliens[i].dot = 1;
			return 1;
		}
	}
	return 0;
}

int spriteCollide (struct Sprite a, struct Sprite p){
    int left1, left2, right1, 
		right2, top1, top2, 
		bottom1, bottom2;
    left1   = a.x;
    left2   = p.x;
    right1  = a.x + a.width;
    right2  = p.x + p.width;
    top1    = a.y;
    top2    = p.y;
    bottom1 = a.y + a.height;
    bottom2 = p.y + p.height;
    if (bottom1 < top2) return(0);
    if (top1 > bottom2) return(0);
    if (right1 < left2) return(0);
	if (left1 > right2) return(0);
	return(1);
}

END_OF_MAIN()
