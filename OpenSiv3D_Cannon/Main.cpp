# include <Siv3D.hpp> // Siv3D v0.6.14

class object {
public:
	virtual ~object() = 0;
	virtual void Draw() = 0;
	virtual void move() = 0;
};

object::~object(){}

class Cannon :public object {
public:
	Cannon(){}
	Cannon(Vec2 _d1, Vec2 _d2, Vec2 _u1, Vec2 _u2, Vec2 _bp1, Vec2 _bp2):
		d1(_d1), d2(_d2), u1(_u1),u2(_u2), bp1(_bp1), bp2(_bp2){
		this->linebp = { bp1, bp2 };
	}
	virtual void Draw() {
		lined = { d1, d2 };
		lineu = { u1, u2 };
		linedd = { d1,u1 };
		lineuu = { d2, u2 };
		if (Rotated == 0) {
			lined.draw(8);
			lineu.draw(8);
			linedd.draw(8);
			lineuu.draw(8);
		}
		else {
			dr = lined.length();
			ur = lineu.length();
			d2.x = d1.x + dr * cos(Math::ToRadians(-Rotated));
			d2.y = d1.y + dr * sin(Math::ToRadians(-Rotated));
			u2.x = u1.x + ur * cos(Math::ToRadians(-Rotated));
			u2.y = u1.y + ur * sin(Math::ToRadians(-Rotated));
			lined.draw(8);
			lineu.draw(8);
			linedd.draw(8);
			lineuu.draw(8);
		}
	}
	virtual void move() {
		if (KeyUp.pressed()) {
			if (Rotated < 90) {
				Rotated++;
				if (Rotated > 90)Rotated = 90;
			}
		}
		if (KeyDown.pressed()) {
			if (Rotated > 0) {
				Rotated--;
				if (Rotated < 0)Rotated = 0;
			}

		}

	}
	bool Flag() {
		return launch_flag;
	}
	const Vec2 bPos() {
		return d2;
	}
	double retRad() {
		return Rotated;
	}
protected:
	Line lined, lineu, linedd, lineuu, linebp;
	double Rotated = 0;
	double dr, ur, br;
	Vec2 ballV0, d1, d2, u1, u2, bp1, bp2;
	bool launch_flag = false;
};

class CannonBall :public object {
public:
	virtual void Draw() {		
		Ball.draw();
	}
	virtual void move() {
	}
	void calc_ball(Cannon& cannon) {

		if (KeySpace.up()) {
			ball = cannon.bPos();
			Ball = { ball, 15 };
			if (KeySpace.pressedDuration() <= 1s) 
				ballV0 = { 100, -100 };
			else if (KeySpace.pressedDuration() > 1s && KeySpace.pressedDuration() <= 3s) 
				ballV0 = { 150, -150 };
			else if (KeySpace.pressedDuration() > 3s && KeySpace.pressedDuration() <= 7s)
				ballV0 = { 200, -200 };
			else if (KeySpace.pressedDuration() > 7s)
				ballV0 = { 250, -250 };
			g = 0.8;
			ballV0.x = ballV0.x * cos(Math::ToRadians(cannon.retRad()));
			ballV0.y = ballV0.y * sin(Math::ToRadians(cannon.retRad()));
		}
		ballV0.y += g;
		ball += ballV0 * Scene::DeltaTime();
		Ball = { ball, 15 };
	}
	void Draw_power() {
		RectF{ 20, 550, (KeySpace.pressedDuration().count()* 50), 10}.draw();
		RectF{ 20, 580, 350, 10 }.draw(ColorSpace::Palette::Red);
	}
	Circle Pos() {
		return Ball;
	}
	void reset_ball(Cannon& cannon) {
		ball = { -100, -100 };
		Ball = { ball, 0 };
		ballV0 = { 0, 0 };
		g = 0;
	}
private:
	Vec2 ball, ballV0;
	double g = 0;
	Circle Ball ;
	double space_count = 0;
	double pressedtime = 0;
};

class Target :public object {
public:
	Target(double _x, double _y) {
		this->x = _x;
		this->y = _y;
		targetPos = { x, y };
		target_m = { targetPos, 20, 100 };
	}
	virtual void Draw() {
		target_m.draw();
	}
	virtual void move(){}
	int hit(CannonBall Ball) {
		Circle ball = Ball.Pos();
		int retNum = 0;
		ball.intersects(target_m) ? retNum = 1 : retNum = 0;
		if (ball.x >= 800 || ball.y >= 600) {
			retNum = 2;
		}
		return retNum;
	}
	void target_reset() {
		targetPos = { Random(300, 750), Random(100., 550.) };
		target_m = { targetPos, 20, 100 };
	}
private:
	double x, y;
	Vec2 targetPos;
	RectF target_m;
};


void Main()
{
	Scene::SetBackground(Color(0, 0, 0));
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	Vec2 d1(30, 550), u1(30, 520), d2(90, 550), u2(90, 520), bp1(30, 535), bp2(90, 535);
	String Game_State = U"game";
	String text1 = U"Hit!";
	String text2 = U"missed!";
	String restart = U"Re start Press 'R'";
	String quit = U"Quit Press 'Q'";
	Cannon cannon(d1,d2,u1,u2,bp1,bp2);
	CannonBall ball;
	Target target(Random(300, 750), Random( 100., 550.));
	Font title{ 100 };
	Font over_font{ 60 };


	while (System::Update())
	{
		if (Game_State == U"game") {
			cannon.Draw();
			ball.Draw();
			target.Draw();
			cannon.move();
			ball.calc_ball(cannon);
			ball.Draw_power();
   			if (target.hit(ball) == 1) {
				Game_State = U"over";
			}
			else if (target.hit(ball) == 2) {
				Game_State = U"over";
			}
		}
		else if (Game_State == U"over") {
			if (target.hit(ball) == 1)
				title(text1).draw(Arg::center(400, 100));
			else
				title(text2).draw(Arg::center(400, 100));
			over_font(restart).draw(Arg::center(400, 300));
			over_font(quit).draw(Arg::center(400, 400));
			if (KeyR.down()) {
				target.target_reset();
				ball.reset_ball(cannon);
				Game_State = U"game";
			}
			else if (KeyQ.down()) {
				System::Exit();
			}
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
