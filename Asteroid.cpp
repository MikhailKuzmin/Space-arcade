#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <iostream>
#include <sstream>
using namespace sf;

const int W = 900;
const int H = 600;
RenderWindow app1(VideoMode(W, H), "SpaceSpaceSpace!");
float DEGTORAD = 0.017453f;

void menu(int);
void gamestart(int, int);
void settings();
int num_of_ast;
int kol_pul;
int health;
int level = 1;
int glob_health = 100;
int glob_ammun = 1000;
int vrhp;
int lev_of_dffclt;
int difNum;
int result_ammun;
int ammntn;

class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	std::vector<IntRect> frames;

	Animation() {}

	Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i < count; i++)
			frames.push_back(IntRect(x + i * w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}

	void update()
	{
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};

class Textload
{
public:
	Sprite sprite;
	std::vector<IntRect> frames;
	Textload() {}
	Textload(Text &t, Font &f, Color clr)
	{
		f.loadFromFile("images/CyrilicOld.ttf");
		t.setFillColor(clr);
		t.setStyle(sf::Text::Bold | sf::Text::Underlined);
	}
};
class Textdraw
{
public:
	Sprite sprite;
	std::vector<IntRect> frames;

	Textdraw() {}

	Textdraw(RenderWindow &app1, Text &t, String text, int x, int y)
	{
    	t.setString(text);//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		t.setPosition(x, y);//задаем позицию текста, центр камеры
		app1.draw(t);//рисую этот текст
	}

};

class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1)   //верчение  корабл€
	{
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update() {};

	void draw(RenderWindow &app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		CircleShape circle(R);
		circle.setFillColor(Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
		//app.draw(circle);
	}

	virtual ~Entity() {};
};

class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}
	void  update()
	{
		x += dx;
		y += dy;
		//if (x > W) x = 0;  if (x < 0) x = W;
		//if (y > H) y = 0;  if (y < 0) y = H;
		if (x > W) dx = -dx;  if (x < 0) dx = -dx;
		if (y > H) dy = -dy;  if (y < 0) dy = -dy;
	}

};


class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}
	void  update()
	{
		dx = cos(angle*DEGTORAD) * 6;  //скорость пуль
		dy = sin(angle*DEGTORAD) * 6;
		// angle+=rand()%6-3;
		x += dx;
		y += dy;

		if (x > W || x<0 || y>H || y < 0) life = 0;
	}

};
class bulleten : public Entity
{
public:
	bulleten()
	{
		name = "bulleten";
	}

	void  update()
	{
		dx = cos(angle*DEGTORAD) * 6;  //скорость пуль
		dy = sin(angle*DEGTORAD) * 6;
		// angle+=rand()%6-3;
		x += dx;
		y += dy;



		if (x > W || x<0 || y>H || y < 0) life = 0;
	}

};

class player : public Entity
{
public:
	bool thrust;

	player()
	{
		name = "player";
	}

	void update()
	{
		if (thrust)
		{
			dx += cos(angle*DEGTORAD)*0.2;
			dy += sin(angle*DEGTORAD)*0.2;
		}
		else
		{
			dx *= 0.95;
			dy *= 0.95;
		}

		int maxSpeed = 10;
		float speed = sqrt(dx*dx + dy * dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx / 2;
		y += dy / 2;

		if (x > W) dx = -dx;  if (x < 0) dx = -dx;
		if (y > H) dy = -dy;  if (y < 0) dy = -dy;
	}

};
class villain : public Entity
{
public:
	bool thrust;

	villain()
	{
		name = "villain";
	}

	void update()
	{
		//if (thrust)
		//{
		//dx = rand() % 8 - 4;
		//dy = rand() % 8 - 4;
		dx += (((rand() % 8) *DEGTORAD)*0.2);
		//dy += (((rand() % 8) *DEGTORAD)*0.2);
	//}
	//else
	//{
		//dx *= 0.99;
		//dy *= 0.99;
	//}

		int maxSpeed = 15;
		float speed = sqrt(dx*dx + dy * dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}
		x += dx;
		//y += dy;



		if (x > W) dx = -dx;  if (x < 0) dx = -dx;
		if (y > H) dy = -dy;  if (y < 0) dy = -dy;
	}

};

bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y) <
		(a->R + b->R)*(a->R + b->R);
}


void gamestart(int level, int difficult)
{

	if(level == 1)
	{
		settings();
		num_of_ast = 10 * difficult;
		kol_pul = 20 * difficult;
		health = 1;
		vrhp = 20 * difficult;
	}
	if(level == 2)
	{
		
		num_of_ast = 5 * difficult;
		kol_pul = 50;
		health = 3;
		vrhp = 20 * difficult;
	}
	if(level == 3)
	{
		
		num_of_ast = 1;
		kol_pul = 1000;
		health = 1000;
		vrhp = 10 * difficult;
	}
	if(level == 4)
	{
		
		num_of_ast = 2 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 1 * difficult;
	}
	if(level == 5)
	{
		
		num_of_ast = 0 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 10 * difficult;
	}
	if(level == 6)
	{
		
		num_of_ast = 4 * difficult;
		kol_pul = glob_ammun;  
		health = glob_health;
		vrhp = 10 * difficult;
	}
	if(level == 7)
	{
		
		num_of_ast = 7 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 10 * difficult;
	}
	if(level == 8)
	{
		
		num_of_ast = 5 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 15 * difficult;
	}
	if(level == 9)
	{
		
		num_of_ast = 9 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 15 * difficult;
	}
	if(level == 10)
	{
		
		num_of_ast = 10 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 1 * difficult;
	}
	if (level == 11)
	{
		
		num_of_ast = 10 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 20 * difficult;
	}
	if (level == 12)
	{
	
		num_of_ast = 10 * difficult;
		kol_pul = glob_ammun;
		health = glob_health;
		vrhp = 40 * difficult;
	}


	srand(time(0));
	int hits;
	app1.setFramerateLimit(60);
	Clock gameTimeClock;//переменна€ игрового времени, будем здесь хранить врем€ игры 
	int gameTime = 100;//объ€вили игровое врем€, инициализировали.
	int gameTime2 = 100;//объ€вили игровое врем€, инициализировали.
	Texture t1, t2, t3, t4, t5, t6, t7, t8, t9, menucurs;
	t1.loadFromFile("images/spaceship.png");
	menucurs.loadFromFile("images/cursor.png");
	std::ostringstream lvl;
	lvl << level;
	String lvll;
	lvll = "images/background" + lvl.str() + ".jpg";
	t2.loadFromFile(lvll);
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	if (ammntn == 1)
	{
		t5.loadFromFile("images/fire_blue.png");
	}
	if (ammntn == 2)
	{
		t5.loadFromFile("images/fire_red.png");
	}
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");
	t8.loadFromFile("images/nlo.png");
	t9.loadFromFile("images/fire_blue.png");
	t1.setSmooth(true);
	t2.setSmooth(true);

	Sprite background(t2);
	Sprite cursr(menucurs);
	//background.setScale(0.5f, 0.5f);

	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
	Animation sVillain(t8, 0, 0, 140, 140, 1, 0);
	Animation sVillain_go(t8, 0, 0, 140, 140, 1, 0);
	Animation sBullet2(t9, 0, 0, 32, 64, 16, 0.8);


	std::list<Entity*> entities;

	for (int i = 0; i < num_of_ast; i++)
	{
		asteroid *a = new asteroid();
		a->settings(sRock, rand() % W, rand() % (H - 300), rand() % 360, 25);
		entities.push_back(a);
	}

	player *p = new player();
	p->settings(sPlayer, W / 2, H - 20, 270, 20);
	entities.push_back(p);
	

	villain *v = new villain();
	v->settings(sVillain, 0 + 40, 0 + 70, 90, 20);
	entities.push_back(v);

	hits = 0;
	
	Font font;//шрифт 
	Text text("", font, 14);
	Textload(text, font, sf::Color(255, 0, 255));
	Text texth("", font, 14);
	Textload(texth, font, sf::Color(255, 0, 255));
	Text texthi("", font, 14);
	Textload(texthi, font, sf::Color(255, 0, 255));
	Text texttime("", font, 14);
	Textload(texttime, font, sf::Color(255, 0, 255));
	Text textvr("", font, 14);
	Textload(textvr, font, sf::Color(255, 0, 255));
	SoundBuffer shootBuffer;//создаЄм буфер дл€ звука
	shootBuffer.loadFromFile("Sounds/laser-blast-descend_gy7c5deo.ogg");//загружаем в него звук
	Sound shoot(shootBuffer);//создаем звук и загружаем в него звук из буфера
	SoundBuffer shootBuffer2;//создаЄм буфер дл€ звука
	shootBuffer2.loadFromFile("Sounds/14549_1460040283.ogg");//загружаем в него звук
	Sound shoot2(shootBuffer2);//создаем звук и загружаем в него звук из буфера
	SoundBuffer shootBuffer3;//создаЄм буфер дл€ звука
	shootBuffer3.loadFromFile("Sounds/14550_1460040283.ogg");//загружаем в него звук
	Sound shoot3(shootBuffer3);//создаем звук и загружаем в него звук из буфера
	while (app1.isOpen())
	{
		int pixelPosx = Mouse::getPosition(app1).x;//забираем коорд курсора
		int pixelPosy = Mouse::getPosition(app1).y;//забираем коорд курсора
		cursr.setPosition(pixelPosx - 20, pixelPosy - 20);
		cursr.setScale(0.2f, 0.2f);//чуть уменьшили картинку, => размер стал меньше
		Event event;
		gameTime = 100 - gameTimeClock.getElapsedTime().asSeconds();
		gameTime2 = 10000 - gameTimeClock.getElapsedTime().asMilliseconds();
		while (app1.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Escape)
				{
					
					menu(2);
					
				}
			}
			if (event.type == Event::Closed)
				app1.close();
				

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Space)
				{
					if (kol_pul > 0 && health > 0 && gameTime > 0)
					{
						
						bullet *b = new bullet();
						b->settings(sBullet, p->x, p->y, p->angle, 10);
						entities.push_back(b);
						shoot.play();//играем звук пули
						kol_pul = kol_pul - 1;
						if (level >= 4)
						{
							glob_ammun = glob_ammun - 1;
						}

					}

				}
			}

		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
		if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
		if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;   //надо true l дл€ полЄта
		else p->thrust = false;

		if (gameTime % 1 == 0)
		{
			if (gameTime2 % 40 == 0)
			{

				
				bulleten *bb = new bulleten();
				bb->settings(sBullet2, v->x, v->y, v->angle, 10);
				entities.push_back(bb);

			}
		}
		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "asteroid" && b->name == "bulleten")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;
						hits++;
						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);
						shoot2.play();

						for (int i = 0; i < 2; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new asteroid();
							e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}

					}
				if (a->name == "villain" && b->name == "bullet")
					if (isCollide(a, b))
					{
						if (vrhp > 0)
						{
							vrhp = vrhp - 1;
							b->life = false;
							hits++;
							Entity *e = new Entity();
							e->settings(sExplosion, a->x, a->y);
							e->name = "explosion";
							entities.push_back(e);
							shoot2.play();
						}
						if (vrhp == 0)
						{
							
							a->life = false;
							b->life = false;
							hits++;
							Entity *e = new Entity();
							e->settings(sExplosion, a->x, a->y);
							e->name = "explosion";
							entities.push_back(e);
							shoot3.play();
						}

					}
				if (a->name == "asteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;
						hits++;
						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);
						shoot2.play();

						for (int i = 0; i < 2; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new asteroid();
							e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}

					}
				if (a->name == "player" && b->name == "bulleten")
					if (isCollide(a, b))
					{
						b->life = false;
						if (health > 0)
						{
							health = health - 1;
							if (level >= 4)
							{
								glob_health = glob_health - 1;
							}
							Entity *e = new Entity();
							e->settings(sExplosion_ship, a->x, a->y);
							e->name = "explosion";
							entities.push_back(e);
							p->settings(sPlayer, a->x, a->y, 270, 20);
							p->dx = 0; p->dy = 0;
						}

					}
				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						b->life = false;
						if (health > 0)
						{
							health = health - 1;
							if(level>=4)
							{
								glob_health = glob_health - 1;
							}
							
							Entity *e = new Entity();
							e->settings(sExplosion_ship, a->x, a->y);
							e->name = "explosion";
							entities.push_back(e);
							p->settings(sPlayer, a->x, a->y, 270, 20);
							p->dx = 0; p->dy = 0;
						}

					}

			}

		if (p->thrust)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;

		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;
		if (rand() % 150 == 0)   //как часто по€вл€ютс€ новые астероиды.
		{
			asteroid *a = new asteroid();
			a->settings(sRock, 0, rand() % H, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}

		app1.draw(background);
		std::ostringstream patron;    // объ€вили переменную
		patron << kol_pul;		//занесли в нее число очков, то есть формируем строку
		Textdraw(app1, text, "Ammunition:" + patron.str(), 750, 50);
		std::ostringstream hh;    // объ€вили переменную
		hh << health;		//занесли в нее число очков, то есть формируем строку
		Textdraw(app1, texth, "Health:" + hh.str(), 750, 75);
		std::ostringstream hitt;    // объ€вили переменную
		hitt << hits;		//занесли в нее число очков, то есть формируем строку
		Textdraw(app1, texthi, "Hits:" + hitt.str(), 750, 100);
		std::ostringstream ttime;    // объ€вили переменную
		ttime << gameTime;		//занесли в нее число очков, то есть формируем строку
		Textdraw(app1, texttime, "Time:" + ttime.str(), 750, 125);
		std::ostringstream vrhpp;    // объ€вили переменную
		vrhpp << vrhp;		//занесли в нее число очков, то есть формируем строку
		Textdraw(app1, textvr, "Enemy HP:" + vrhpp.str(), 750, 150);
		for (auto i : entities)
			i->draw(app1);
		if (vrhp == 0)
		{
			//level++;
			menu(3);
			
		}
		if (health == 0)
		{
			//level++;
			menu(4);
			
		}
		app1.draw(cursr);
		app1.setMouseCursorVisible(false);
		app1.display();
	}
}

void menu(int variant) 
{
	bool isMenu1 = 1;
	while (isMenu1)
	{
		
		Texture menuTexture1, menuTexture2, menuTexture3, menuTexture4, menuTextureendv, menuTextureendd, aboutTexture, menuBackground, menucurs, menudiffic, menueasy, menunormal, menuhard;
		if (variant == 1 || variant == 2)
		{
			menuTexture1.loadFromFile("images/newgame.png");
			menuTexture2.loadFromFile("images/difficult.png");
			if (variant == 1)
			{
				menueasy.loadFromFile("images/easy.png");
				menunormal.loadFromFile("images/normal.png");
				menuhard.loadFromFile("images/hard.png");
			}
			if (variant == 2)
			{
				menuTexture4.loadFromFile("images/cont.png");
			}
			menuTexture3.loadFromFile("images/exit.png");
			aboutTexture.loadFromFile("images/about.png");
			menuBackground.loadFromFile("images/backgroundmenu.jpg");
			menucurs.loadFromFile("images/cursor.png");
		}
		if (variant == 3 || variant == 4)
		{
			
			menuTexture1.loadFromFile("images/nextlev.png");
			menuTexture2.loadFromFile("images/backmenu.png");
			menuTexture4.loadFromFile("images/restart.png");
			menuTexture3.loadFromFile("images/exit.png");
			aboutTexture.loadFromFile("images/about.png");
			menuBackground.loadFromFile("images/backgroundmenu.jpg");
			menucurs.loadFromFile("images/cursor.png");
			menuTextureendv.loadFromFile("images/vic.png");
			menuTextureendd.loadFromFile("images/def.png");
		}
		
		Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), menu4(menuTexture4), about(aboutTexture), menuBg(menuBackground), cursr(menucurs), menuendv(menuTextureendv), menuendd(menuTextureendd), menue(menueasy), menun(menunormal), menuh(menuhard);
		bool isMenu = 1;
		int menuNum = 0;
		if (variant == 1 || variant == 2)
		{
			menu1.setPosition(100, 30);
			menu2.setPosition(100, 130);
			if (variant == 1)
			{
				menue.setPosition(100, 230);
				menun.setPosition(250, 230);
				menuh.setPosition(450, 230);
			}
			menu3.setPosition(100, 330);
			if (variant == 2)
			{
				menu4.setPosition(100, 430);
			}  
			menuBg.setScale(0.9f, 0.9f);//чуть уменьшили картинку, => размер стал меньше
			menuBg.setPosition(0, 0);
		}
		if (variant == 3 || variant == 4)
		{
			menu1.setPosition(50, 130);
			menu2.setPosition(50, 230);
			menu3.setPosition(50, 330);
			menu4.setPosition(50, 430);
			if (variant == 3)
			{
				menuendv.setPosition(250, 30);
			}
			if (variant == 4)
			{
				menuendd.setPosition(250, 30);
			}
			menuBg.setScale(0.9f, 0.9f);
			menuBg.setPosition(0, 0);
		}
		//////////////////////////////ћ≈Ќё///////////////////
		while (isMenu)
		{
			
			int pixelPosx = Mouse::getPosition(app1).x;//забираем коорд курсора
			int pixelPosy = Mouse::getPosition(app1).y;//забираем коорд курсора
			cursr.setPosition(pixelPosx - 20, pixelPosy - 20);
			cursr.setScale(0.2f, 0.2f);//чуть уменьшили картинку, => размер стал меньше
			
			menu1.setColor(Color::White);
			menu2.setColor(Color::White);
			menu3.setColor(Color::White);
			menu4.setColor(Color::White);
			menue.setColor(Color::White); menun.setColor(Color::White); menuh.setColor(Color::White);
			menuNum = 0;

			app1.clear(Color(129, 181, 221));
			if (variant == 1 || variant == 2)
			{
				if (IntRect(100, 30, 300, 100).contains(Mouse::getPosition(app1))) { menu1.setColor(Color::Blue); menuNum = 1; }
				if (variant == 1)
				{
					if (IntRect(100, 230, 150, 100).contains(Mouse::getPosition(app1))) { menue.setColor(Color::Blue); difNum = 1; }
					if (IntRect(250, 230, 220, 100).contains(Mouse::getPosition(app1))) { menun.setColor(Color::Blue); difNum = 2; }
					if (IntRect(450, 230, 150, 100).contains(Mouse::getPosition(app1))) { menuh.setColor(Color::Blue); difNum = 3; }
				}
				if (IntRect(100, 330, 300, 100).contains(Mouse::getPosition(app1))) { menu3.setColor(Color::Blue); menuNum = 3; }
				if (variant == 2)
				{
					if (IntRect(100, 430, 300, 100).contains(Mouse::getPosition(app1))) { menu4.setColor(Color::Blue); menuNum = 4; }
				}
			}
			if (variant == 3 || variant == 4)
			{
				if (IntRect(50, 130, 300, 100).contains(Mouse::getPosition(app1))) { menu1.setColor(Color::Blue); menuNum = 1; }
				if (IntRect(50, 230, 300, 100).contains(Mouse::getPosition(app1))) { menu2.setColor(Color::Blue); menuNum = 2; }
				if (IntRect(50, 330, 300, 100).contains(Mouse::getPosition(app1))) { menu3.setColor(Color::Blue); menuNum = 3; }
				if (IntRect(50, 430, 300, 100).contains(Mouse::getPosition(app1))) { menu4.setColor(Color::Blue); menuNum = 4; }

			}
			if (Mouse::isButtonPressed(Mouse::Left) && variant == 1)
			{
				if (menuNum == 1) { gamestart(level, difNum); isMenu = false; isMenu1 = false;
				}
				if (menuNum == 2) { settings(); isMenu = false; isMenu1 = false; }
				if (menuNum == 3) {
					isMenu = false; app1.close(); isMenu1 = false;
				}

			}
			else if (Mouse::isButtonPressed(Mouse::Left) && variant == 2)
			{
				if (menuNum == 1) { gamestart(level, difNum); isMenu = false; isMenu1 = false;
				}
				if (menuNum == 2) { settings(); isMenu = false; isMenu1 = false; }
				if (menuNum == 3) { isMenu = false; isMenu1 = false;  app1.close(); }
				if (menuNum == 4) {
					isMenu = false; isMenu1 = false;
				}
			}
			else if (Mouse::isButtonPressed(Mouse::Left) && variant == 3)
			{
				if (menuNum == 1 && level < 12) {
					level = level + 1;
					gamestart(level, difNum); isMenu = false; isMenu1 = false;
				}
				if (menuNum == 2) { menu(1); }
				if (menuNum == 3) {
					app1.close(); isMenu = false; isMenu1 = false;
				}
				if (menuNum == 4) {
					gamestart(level, difNum); isMenu = false; isMenu1 = false;
				}

			}
			else if (Mouse::isButtonPressed(Mouse::Left) && variant == 4)
			{
				if (menuNum == 1 && level < 12) {
					level = level + 1; gamestart(level, difNum); isMenu = false; isMenu1 = false;
				}
				if (menuNum == 2) { menu(1); }
				if (menuNum == 3) {
					app1.close(); isMenu = false; isMenu1 = false;
				}//exit
				if (menuNum == 4 ) {
					gamestart(level, difNum);  isMenu = false; isMenu1 = false;
				}
			}
			app1.draw(menuBg);
			if (level < 12)
			{
				app1.draw(menu1);
			}
			app1.draw(menu2);
			app1.draw(menu3);
			app1.draw(menu4);
			if (variant == 1)
			{
				app1.draw(menue); app1.draw(menun); app1.draw(menuh);
			} 
			if (variant == 3)
			{
				app1.draw(menuendv);
			}
			if (variant == 4)
			{
				app1.draw(menuendd);
			}
			app1.draw(cursr);
			app1.setMouseCursorVisible(false);
			app1.display();
		}
	}
	////////////////////////////////////////////////////
}
void settings()
{
	int setNum = 0;	bool isSet = 1;
	Texture start, ammunitiontex, setTexture2, ammunitiontex1, ammunitiontex2, setTextureendv, menuTextureendd, aboutTexture, setBackground, menucurs;
	    start.loadFromFile("images/start.png");
	    ammunitiontex.loadFromFile("images/amm.png");
		ammunitiontex1.loadFromFile("images/blue.png"); ammunitiontex2.loadFromFile("images/red.png");
		setTexture2.loadFromFile("images/backmenu.png");
		setBackground.loadFromFile("images/backgroundmenu.jpg");
		menucurs.loadFromFile("images/cursor.png");
		Sprite strt(start), menuBg(setBackground), cursr(menucurs), set2(setTexture2), ammun(ammunitiontex), ammun1(ammunitiontex1), ammun2(ammunitiontex2);
		menuBg.setPosition(0, 0);
		set2.setPosition(100, 500);
		strt.setPosition(200, 400);
		ammun.setPosition(100, 100); ammun1.setPosition(100, 200); ammun2.setPosition(100, 300);
		Event event1;
		int setn1 = 0;
		int setn2 = 0;
		while (isSet)
		{
			int pixelPosx = Mouse::getPosition(app1).x;
			int pixelPosy = Mouse::getPosition(app1).y;
			cursr.setPosition(pixelPosx - 20, pixelPosy - 20);
			cursr.setScale(0.15f, 0.15f);
			set2.setColor(Color::White); ammun1.setColor(Color::White); ammun2.setColor(Color::White);
			if (IntRect(100, 500, 300, 100).contains(Mouse::getPosition(app1))) { set2.setColor(Color::Blue); setNum = 1; }
			if (IntRect(100, 200, 300, 100).contains(Mouse::getPosition(app1))) { ammun1.setColor(Color::Blue); setNum = 2; }
			if (IntRect(100, 300, 300, 100).contains(Mouse::getPosition(app1))) { ammun2.setColor(Color::Blue); setNum = 3; }
			if (IntRect(200, 400, 300, 100).contains(Mouse::getPosition(app1))) { strt.setColor(Color::Blue); setNum = 4; }
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (setNum == 1) { menu(1);  isSet = false; }
				if (setNum == 2) { ammun1.setColor(Color::Green); setn1++; setn2 = 0; }
				if (setNum == 3) { ammun2.setColor(Color::Green); setn2++; setn1 = 0; }
				if (setNum == 4 && level <= 12) {isSet = false;}
			}

			while (app1.pollEvent(event1))
			{
				if (event1.type == Event::KeyPressed)
				{
					if (event1.key.code == Keyboard::Enter)
					{
						isSet = false;  
					}
				}
			}
			app1.draw(menuBg);
			app1.draw(set2);
			app1.draw(cursr);
			app1.draw(ammun);  app1.draw(ammun1); app1.draw(ammun2);
			app1.draw(strt);
			app1.setMouseCursorVisible(false);
			app1.display();
		}
		if (setn1 > 0)
		{
			result_ammun = 1;
		}
		if (setn2 > 0)
		{
			result_ammun = 2;
		}
		if (setn1 == 0 && setn2 == 0)
		{
			result_ammun = 1;
		}
		ammntn = result_ammun;
}
int main()
{
	menu(1);
	return 0;
}


