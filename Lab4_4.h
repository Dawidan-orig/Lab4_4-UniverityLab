#pragma once
#pragma once

#include <Windows.h>
/*
HDC hdc;
COLORREF erase;

vector <Figure_GrandTree*> trees;
vector <Locust*> locusts;

HWND GetConcoleWindow();

int CalculateDistance(int x1, int y1, int x2, int y2);

vector<Point*> AllNear(Point* obj, int dist);
*/
class IReactive
{
public:
    virtual void Response() = 0;
};

class GameObject // ����������� �����
{
protected:
    // ��������� ���������� � �������� � �� ������������, ������ � ���� ������������ int.
    // origin-point x
    int x;
    // origin-point y
    int y;

public:
    bool toDestroy = false;

    GameObject(int x, int y);
    ~GameObject(void);

    virtual void Draw() = 0; // ����������� �������     

    void ToggleVisibility();
    int getX();
    int getY();

    void Translate(int newX, int newY);

    bool GetVisibilty()
    {
        return isVisible;
    }

private:
    COLORREF color;

protected:
    bool isVisible = false;
};

class Figure_GrandTree : public GameObject
    // ��������� ����������� ������������, ���������������� ��� ������.
{
protected:
    struct treePiece
    {
        COLORREF color;
        int x; // �������� ������������ ��� ��������� ���������
        int y;
        int width; // ��������� ������ ����� �������
        int height; // ��������� ������ ����� �������
    };

    treePiece trunk;
    treePiece leaves;

    int vitality = 1000; // ��� ������ ����� �������� ������� �����.
    int root = 300;

public:
    Figure_GrandTree(int x, int y);
    ~Figure_GrandTree();

    virtual int Damage(int power);
    int rootDamage(int power);

    void rootUpdate();

    void Draw() override;

    virtual int GetConversion();
};

class EatenTree : public Figure_GrandTree
{
public:
    EatenTree(int x, int y);
    ~EatenTree();

    void Draw() override;
};

class WimpTree : public Figure_GrandTree
{
    // ������ ������, ������� ����� ����������.
    // �� ���� ���������� ��� ������, ������ � �����
public:
    WimpTree(int x, int y);
    ~WimpTree();

    int Damage(int power) override;
    int GetConversion() override;
};

class ToughTree : public Figure_GrandTree
{
    // ��� ������ ����� ���������� �������������� �����.
    // ����� � ���������� �� ���� ����������.
protected:
    int power;

public:
    ToughTree(int x, int y, int armor);
    ~ToughTree();

    int Damage(int power) override;

    void Draw() override;
    int GetConversion() override;
};

class MagicTree : public Figure_GrandTree, public IReactive
{
    // ������, ������� �������������� ������ ������������ ��������
    // �������� ����� ���������
public:
    virtual void Response();

    MagicTree(int x, int y);
    ~MagicTree();

    int Damage(int power) override;

    int GetConversion() override;
};

class AngryTree : public ToughTree, public IReactive
{
    // ������, ������� ����� ������� ����� ����������
    // ��� �������� ���� � ������.
public:
    virtual void Response();

    AngryTree(int x, int y, int power);
    ~AngryTree();

    int Damage(int power) override;

    void Draw() override;
    int GetConversion() override;
};

class TriangleTree : public Figure_GrandTree
{
public:
    TriangleTree(int x, int y);
    ~TriangleTree();

    void Draw() override;
};

class Locust : public GameObject
{
protected:
    Figure_GrandTree* hunted = NULL;
    int nutrition = 0;
    COLORREF color;
public:
    // �������, ������� ������ �������.
    Locust(int x, int y);
    ~Locust();

    virtual void Move();

    virtual void Feast();

    void Draw() override;
};

class BigLocust : public Locust
{
public:
    BigLocust(int x, int y);
    ~BigLocust();

    void Feast() override;

    void Draw() override;
};