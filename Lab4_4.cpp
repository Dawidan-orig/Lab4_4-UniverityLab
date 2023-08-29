using namespace std;

#include<iostream>
#include<windows.h>
#include<string.h>
#include <vector>
#include<algorithm>
#include "Lab4_4.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HDC hdc;
COLORREF erase = RGB(0, 0, 0);

vector <Figure_GrandTree*> trees = {};
vector <EatenTree*> deadTrees = {};
vector <Locust*> locusts = {};

HWND GetConcoleWindow() // Получение окна консоли, в которой и будем рисовать
{
	char str[128];
	// char title[128]="xxxxxxxxxxxxxxxxxx";
	LPWSTR title = (LPWSTR)"xxxxxxxxxxxxxxxxxx";	//новая версия Windows
	GetConsoleTitle((LPWSTR)str, sizeof((LPWSTR)str)); // получить заголовок окна
	SetConsoleTitle(title);						// установить новый заголовок окна
	Sleep(100);									// ждем смены заголовка окна (100 мс);

	HWND hwnd = FindWindow(NULL, (LPWSTR)title);// определяем дескриптор окна
	SetConsoleTitle((LPWSTR)str);				//возвращаем прежний заголовок

	return hwnd;//вернуть дескриптор окна
}//end GetConcolWindow()

int CalculateDistance(int x1, int y1, int x2, int y2)
{
	int posx = x2 - x1;
	int posy = y2 - y1;

	return sqrt(pow(posx, 2) + pow(posy, 2));
}

int AllNear(GameObject* obj, int dist)
{
	vector<GameObject*> res;

	res.resize(trees.size() + locusts.size());
	int amount = 0;

	// Вообще, можно не рассматривать отдельно деревья и сарачу, а перевести всё на Point. Меньше кода писать.
	// Но я оставлю всё как есть. Так нагляднее и проще для понимания.
	for (int i = 0; i < trees.size(); i++)  // Рассматриваем деревья.
	{
		Figure_GrandTree* tree = trees.at(i); // Для каждого...
		if (CalculateDistance(obj->getX(), obj->getY(), tree->getX(), tree->getY()) < dist and tree != obj)
		{
			res.push_back(tree);
			amount++;
		}
	}

	for (int i = 0; i < locusts.size(); i++)  // Рассматриваем Саранчу.
	{
		Locust* loc = locusts.at(i); // Для каждого...
		if (CalculateDistance(obj->getX(), obj->getY(), loc->getX(), loc->getY()) < dist and loc != obj)
		{
			res.push_back(loc);
			amount++;
		}
	}

	return amount;
}

int main()
{
	HWND hwnd = GetConcoleWindow();

	//если дескриптор существует
	if (hwnd != NULL)
	{
		//получим контекст устройства для консольного окна
		hdc = GetWindowDC(hwnd);

		//если контекст существует - можем работать
		if (hdc != 0)
		{
#pragma region init

			Figure_GrandTree* fig = new Figure_GrandTree(300, 300);
			fig->ToggleVisibility();
			fig->Draw();

			WimpTree* wimpy = new WimpTree(100, 300);
			wimpy->ToggleVisibility();
			wimpy->Draw();

			MagicTree* magic = new MagicTree(500, 400);
			magic->ToggleVisibility();
			magic->Draw();

			ToughTree* tough = new ToughTree(700, 300, 10);
			tough->ToggleVisibility();
			tough->Draw();

			AngryTree* angry = new AngryTree(700, 600, 10);
			angry->ToggleVisibility();
			angry->Draw();

			EatenTree* eaten = new EatenTree(-100, -100);
			eaten->ToggleVisibility();

			TriangleTree* tri = new TriangleTree(200, 200);
			tri->ToggleVisibility();
			tri->Draw();

			BigLocust* l = new BigLocust(50, 300);
			l->ToggleVisibility();
			l->Draw();

#pragma endregion

			while (1) // Запуск всего инициализированного!
				if (KEY_DOWN(49))   //цифра 1
					break;

			while (1)
			{
				RECT r = RECT(); // Стираём всё
				r.left = 0;
				r.top = 0;
				r.right = 1670;
				r.bottom = 1670;

				FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(erase)); // Всё на экрене стёрто.

				for (int i = 0; i < trees.size(); i++)  // Рассматриваем деревья.
				{
					Figure_GrandTree* tree = trees.at(i); // Для каждого...

					if (not tree->GetVisibilty()) // Пропускаем выключенные деревья
						continue;

					tree->rootUpdate(); // ...Обноавляем мощность корня
					tree->Draw(); //...Рисуем

					if (tree->toDestroy) {
						EatenTree* eaten = new EatenTree(tree->getX(), tree->getY());
						eaten->ToggleVisibility();
						trees.erase(remove(trees.begin(), trees.end(), tree), trees.end()); // Удаляем дерево из глобального списка, чтобы не обновлять его более
					}
				}

				for (int i = 0; i < deadTrees.size(); i++) // Рассматриваем Мёртвые деревья.
				{
					deadTrees.at(i)->Draw();
				}

				for (int i = 0; i < locusts.size(); i++) // Рассматриваем саранчу.
				{
					Locust* locust = locusts.at(i); // Для каждого...
					locust->Move(); //... Обновляем проверку для движения.

					locust->Draw(); //... рисуем

					if (locust->toDestroy) {
						delete locust; // И удаляем, если он помечен.
					}
				}

				Sleep(250); // Задержка на 1/4 секунды

				if (KEY_DOWN(50)) // Остановка этой симуляции по цифре 2
					break;
			}
		}

		// Освободим контекст устройства для нашего окна   
		ReleaseDC(hwnd, hdc);

	}
}


#pragma region Gameobject
GameObject::GameObject(int x, int y)
{
	isVisible = false; // Инициализируем; Делаем невидимой.
	color = RGB(255, 0, 0); // Делаем точку красной
	this->x = x;
	this->y = y;
}
GameObject::~GameObject()
{

}

void GameObject::ToggleVisibility()
{
	isVisible = !isVisible;
}

int GameObject::getX()
{
	return x;
}
int GameObject::getY()
{
	return y;
}

void GameObject::Translate(int newX, int newY)
{
	x = newX;
	y = newY;
}
#pragma endregion

#pragma region Figure
Figure_GrandTree::Figure_GrandTree(int x, int y) : GameObject(x, y)
{
	trunk = treePiece(); // Собираем ствол
	trunk.color = RGB(128, 64, 48);
	trunk.height = 50;
	trunk.width = 10;
	trunk.x = 0;
	trunk.y = 0;

	leaves = treePiece(); // Собираем листья.
	leaves.color = RGB(0, 255, 0);
	leaves.height = 50;
	leaves.width = 50;
	leaves.x = -25;
	leaves.y = -75;

	trees.push_back(this); // Добавляем дерево в глобальный список
}
Figure_GrandTree::~Figure_GrandTree()
{
	trees.erase(remove(trees.begin(), trees.end(), this), trees.end()); // Удаляем дерево из глобального списка, чтобы не обновлять его более
}

int Figure_GrandTree::GetConversion() { return 0; }

int Figure_GrandTree::Damage(int damage) // Нанесение урона по дереву
{
	if (root > 0)
	{
		return rootDamage(damage);
	}

	vitality -= damage;

	if (vitality <= 0) // Если дерево потеряло всё здоровье...
	{
		toDestroy = true; // То помечаем на удаление.
	}

	leaves.color = leaves.color * 0.95f;

	return damage * 10; // Возвращаем очки питательности
}

int Figure_GrandTree::rootDamage(int damage)
{
	root -= damage;
	return damage * 10;
}

void Figure_GrandTree::rootUpdate()
{
	vitality += root / 100;
}

void Figure_GrandTree::Draw()
{
	if (isVisible)
	{
		RECT r = RECT();
		r.left = x + trunk.x - trunk.width / 2;
		r.top = y + trunk.y - trunk.height / 2;
		r.right = x + trunk.x + trunk.width / 2;
		r.bottom = y + trunk.y + trunk.height / 2;

		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(trunk.color));

		if (root > 0)
		{
			RECT root = RECT();
			root.top = y + trunk.y + trunk.height / 2 - 5;
			root.bottom = y + trunk.y + trunk.height / 2 + 5;
			root.left = x + leaves.x;
			root.right = x + leaves.x + leaves.width;

			FillRect(hdc, &root, (HBRUSH)CreateSolidBrush(trunk.color));
		}

		HPEN Pen = CreatePen(PS_SOLID, 5, leaves.color);
		SelectObject(hdc, GetStockObject(GRAY_BRUSH));
		SelectObject(hdc, Pen);
		Ellipse(hdc, x + leaves.x, y + leaves.y, x + leaves.x + leaves.width, y + leaves.y + leaves.height);
		DeleteObject(Pen);
	}
	else
	{
		RECT r;
		r.left = x + trunk.x - trunk.width / 2;
		r.top = y + trunk.y - trunk.height / 2;
		r.right = x + trunk.x + trunk.width / 2;
		r.bottom = y + trunk.y + trunk.height / 2;

		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(erase));
	}
}
#pragma endregion

#pragma region DeadTree

EatenTree::EatenTree(int x, int y) : Figure_GrandTree(x, y)
{
	vitality = -1;
	root = -1;
	leaves.color = erase;
	trees.erase(remove(trees.begin(), trees.end(), this), trees.end()); // Удаляем дерево из глобального списка, чтобы не обновлять его более
	deadTrees.push_back(this); // Потому что обновляться оно будет отдельно
}
EatenTree::~EatenTree()
{
	deadTrees.erase(remove(deadTrees.begin(), deadTrees.end(), this), deadTrees.end()); // Удаляем дерево из глобального списка, чтобы не обновлять его более
}

void EatenTree::Draw()
{
	Figure_GrandTree::Draw();
}

#pragma endregion

#pragma region Wimp

WimpTree::WimpTree(int x, int y) : Figure_GrandTree(x, y)
{
	vitality = 100; // Сильно ослабляем хилое дерево...
	trunk.width /= 3; // Делаем ствол тоньше...
	leaves.color = RGB(64, 32, 24); // Делаем листья блёклыми...
}

int WimpTree::GetConversion() { return 1; }
WimpTree::~WimpTree()
{

}

int WimpTree::Damage(int damage)
{
	return Figure_GrandTree::Damage(damage) / 10; // Очки питательности должны быть уменьшены при нанесении урона по слабому дереву.
}

#pragma endregion

#pragma region Tough
ToughTree::ToughTree(int x, int y, int armour) : Figure_GrandTree(x, y)
{
	this->power = armour; // У бронированного дерева есть ещё показатель защиты,
	vitality = 300; // Но жизней не так уж и много.
}
ToughTree::~ToughTree()
{

}

int ToughTree::GetConversion() { return 3; }

int ToughTree::Damage(int damage) // Переделываем нанесение урона, так как тут оно работает совсем по другому.
{
	vitality -= damage / power;

	if (vitality <= 0)
	{
		toDestroy = true;
	}
	return damage;
}

void ToughTree::Draw()
{
	Figure_GrandTree::Draw();

	if (isVisible) {
		RECT r = RECT();
		r.left = x + leaves.x;
		r.top = y + leaves.y;
		r.right = x + leaves.x + leaves.width;
		r.bottom = y + leaves.y + leaves.height;
		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(leaves.color));
	}
	else
	{
		RECT r = RECT();
		r.left = x + leaves.x;
		r.top = y + leaves.y;
		r.right = x + leaves.x + leaves.width;
		r.bottom = y + leaves.y + leaves.height;
		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(erase));
	}
}
#pragma endregion

#pragma region Magic
MagicTree::MagicTree(int x, int y) : Figure_GrandTree(x, y)
{
	leaves.color = RGB(0, 255, 255);
	trunk.color = RGB(128, 72, 64);
	vitality = 300;
}
MagicTree::~MagicTree()
{

}

int MagicTree::GetConversion() { return 2; }

int MagicTree::Damage(int damage)
{
	return Figure_GrandTree::Damage(damage) / 10; // В остальном действуем как обычно.
}

void MagicTree::Response() // Главное свойство этого дерева - отправлять прочь саранчу.
{
	int target = rand() % locusts.size();

	int newX = rand() % 1000;
	int newY = rand() % 1000;

	auto iter = locusts.begin();
	advance(iter, target);
	Locust* prey = *iter;
	prey->Translate(newX, newY);
}
#pragma endregion

#pragma region Angry

void AngryTree::Response() 
{
	// Берём ближайшего саранчоида, и кушаем его в ответ.

	if (rand() % 100 > 10) // ...Если повезёт.
		return;

	auto iter = locusts.begin();
	auto savedIteration = locusts.begin();
	int minDistance = CalculateDistance((*iter)->getX(), (*iter)->getY(), x, y);

	advance(iter, 1);

	for (int i = 1; i < locusts.size(); i++)
	{
		int distance = CalculateDistance((*iter)->getX(), (*iter)->getY(), x, y);
		if (minDistance > distance) {
			minDistance = distance;
			savedIteration = iter;
		}

		advance(iter, 1);
	}

	Locust* prey = *savedIteration;

	prey->toDestroy = true; // Все, съели.
	vitality += 10;
}

AngryTree::AngryTree(int x, int y, int power) : ToughTree(x, y, power)
{
	leaves.color = RGB(255, 0, 0); // У злого дерева листья имеют яростный цвет,
	vitality = 50; // Но оно очень хилое!
	root = 50;
}
AngryTree::~AngryTree()
{

}

int AngryTree::GetConversion() { return 4; }

int AngryTree::Damage(int damage)
{
	int res = ToughTree::Damage(damage);

	return res;
}

void AngryTree::Draw()
{
	ToughTree::Draw();
}

#pragma endregion

#pragma region Locust
Locust::Locust(int x, int y) : GameObject(x, y)
{
	locusts.push_back(this);
	color = RGB(30, 255, 30);
}
Locust::~Locust()
{
	locusts.erase(remove(locusts.begin(), locusts.end(), this), locusts.end());
}

void Locust::Move()
{
	if (not hunted == NULL)
		if (hunted->toDestroy) // Если цель, по сути своей, удалена, то нужно забыть о ней.
		{
			hunted = NULL;
		}

	// Выбираем какое-то дерево из списка, да двигаемся к нему.
	if (hunted == NULL)
	{
		auto iter = trees.begin();
		int minDist = 99999999; // Большое число для работы сравнения.
		for (int i = 0;i < trees.size(); i++)
		{
			Figure_GrandTree* possible = *iter;
			int dist = CalculateDistance(possible->getX(), possible->getY(), x, y);
			if (minDist > dist and not possible->toDestroy and possible->GetVisibilty())
			{
				minDist = dist;
				hunted = trees.at(i);
			}
			advance(iter, 1);
		} // Опрделили жертву
	}

	if (hunted == NULL) // Если жертвы всё так же нет, то значит их нет в данный момент вообще.
		return;

	// Жертва точно есть.

	// Двигаемся к ней.

	if (hunted->getX() < x)
		Translate(x - 5, y);
	else
		Translate(x + 5, y);

	if (hunted->getY() < y)
		Translate(x, y - 5);
	else
		Translate(x, y + 5);
	// Двинулись

	// Проверяем, что сарача не стоит внутри чего-то другого
	while (1)
	{
		// Если есть хотя бы одна штука на том же месте, что и саранча...
		if (AllNear(this, 2) > 0)
		{
			// То выталкиваем в случайную сторону
			const int power = 2;

			int xOffset = rand() % 2 == 0 ? -1 : 1;
			int yOffset = rand() % 2 == 0 ? -1 : 1;

			Translate(x + xOffset * power, y + yOffset * power);
		}
		else
			break;
	} // Саранча гарантировано стоит вне других объектов.

	Feast();
}

void Locust::Feast()
{
	// Проверяем дальность до цели и пожинаем её, если она близка.

	if (CalculateDistance(hunted->getX(), hunted->getY(), x, y) < 50)
	{
		nutrition += hunted->Damage(10);
		if(auto reactive = dynamic_cast<IReactive*>(hunted)) // Если дерево реактивное, и что-то делает, когда его кусают
		{
			reactive->Response();
		}

		if (hunted->toDestroy)
			hunted = NULL;
	}
	if (nutrition > 1000)
	{
		nutrition %= 1000;
		Locust* newBorn = new Locust(x, y);
		newBorn->ToggleVisibility();
	}
}

void Locust::Draw()
{
	if (isVisible) {
		SetPixel(hdc, x, y, color);//рисуем
		SetPixel(hdc, x+1, y + 1, color);
		SetPixel(hdc, x + 1, y-1, color);
		SetPixel(hdc, x-1, y, color);
		SetPixel(hdc, x + 2, y + 2, color);
		SetPixel(hdc, x + 2, y - 2, color);
		SetPixel(hdc, x - 2, y + 1,color);
		SetPixel(hdc, x - 2, y - 1, color);
	}
	else {
		SetPixel(hdc, x, y, erase);//стираем
		SetPixel(hdc, x + 1, y + 1, erase);
		SetPixel(hdc, x + 1, y - 1, erase);
		SetPixel(hdc, x - 1, y, erase);
		SetPixel(hdc, x + 2, y + 2, erase);
		SetPixel(hdc, x + 2, y - 2, erase);
		SetPixel(hdc, x - 2, y + 1, erase);
		SetPixel(hdc, x - 2, y - 1, erase);
	}
}
#pragma endregion

#pragma region BigL
BigLocust::BigLocust(int x, int y) : Locust(x, y)
{
	
}

void BigLocust::Feast()
{
	// Проверяем дальность до цели и пожинаем её, если она близка.

	if (CalculateDistance(hunted->getX(), hunted->getY(), x, y) < 50)
	{
		nutrition += hunted->Damage(10);
		if (auto reactive = dynamic_cast<IReactive*>(hunted)) // Если дерево реактивное, и что-то делает, когда его кусают
		{
			reactive->Response();
		}

		if (hunted->toDestroy)
			hunted = NULL;
	}
	if (nutrition > 1000)
	{
		nutrition %= 1000;
		BigLocust* newBorn = new BigLocust(x, y);
		newBorn->ToggleVisibility();
	}
}

void BigLocust::Draw()
{
	if (isVisible) {
		HPEN Pen = CreatePen(PS_SOLID, 5, color);
		SelectObject(hdc, Pen);
		MoveToEx(hdc, x - 5, y - 5, NULL);
		LineTo(hdc, x + 5, y + 5);
		MoveToEx(hdc, x - 5, y + 5, NULL);
		LineTo(hdc, x + 5, y - 5);

		DeleteObject(Pen);
	}
	else {
		HPEN Pen = CreatePen(PS_SOLID, 5, erase);
		SelectObject(hdc, Pen);
		MoveToEx(hdc, x - 5, y - 5, NULL);
		LineTo(hdc, x + 5, y + 5);
		MoveToEx(hdc, x - 5, y + 5, NULL);
		LineTo(hdc, x + 5, y - 5);

		DeleteObject(Pen);
	}
}
#pragma endregion

#pragma region TriangleTree

TriangleTree::TriangleTree(int x, int y) : Figure_GrandTree(x, y) {};

void TriangleTree::Draw()
{
	if (isVisible) {

		RECT r = RECT();
		r.left = x + trunk.x - trunk.width / 2;
		r.top = y + trunk.y - trunk.height / 2;
		r.right = x + trunk.x + trunk.width / 2;
		r.bottom = y + trunk.y + trunk.height / 2;

		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(trunk.color));

		if (root > 0)
		{
			RECT root = RECT();
			root.top = y + trunk.y + trunk.height / 2 - 5;
			root.bottom = y + trunk.y + trunk.height / 2 + 5;
			root.left = x + leaves.x;
			root.right = x + leaves.x + leaves.width;

			FillRect(hdc, &root, (HBRUSH)CreateSolidBrush(trunk.color));
		}


		HPEN Pen = CreatePen(PS_SOLID, 5, leaves.color);
		SelectObject(hdc, Pen);

		MoveToEx(hdc, x+ leaves.x,y+ leaves.y + leaves.height, NULL);
		LineTo(hdc, x+(leaves.x + leaves.width) / 2,y+ leaves.y);
		LineTo(hdc, x+(leaves.x + leaves.width),y+ leaves.y + leaves.height);
		LineTo(hdc, x+leaves.x,y+ leaves.y + leaves.height);

		DeleteObject(Pen);
	}
	else
	{
		RECT r = RECT();
		r.left = x + trunk.x - trunk.width / 2;
		r.top = y + trunk.y - trunk.height / 2;
		r.right = x + trunk.x + trunk.width / 2;
		r.bottom = y + trunk.y + trunk.height / 2;

		FillRect(hdc, &r, (HBRUSH)CreateSolidBrush(trunk.color));
	}
}

#pragma endregion
